"""Bounded readers for verified Larax & Zaco resource containers.

All inputs are bytes; offsets are zero-based file offsets. Unknown fields and
raw pixel/name bytes remain unchanged. See docs/resource-formats.md for evidence
and limitations, especially the underfilled level planes. No filesystem or
network access occurs here.
"""
import struct


class _Reader:
    def __init__(self, data):
        self.data = data
        self.pos = 0

    def take(self, count):
        if count < 0 or count > len(self.data) - self.pos:
            raise ValueError(f"truncated at {self.pos:#x}: need {count} bytes")
        start = self.pos
        self.pos += count
        return self.data[start:self.pos]

    def u8(self):
        return self.take(1)[0]

    def u16(self):
        return struct.unpack("<H", self.take(2))[0]

    def eof(self):
        if self.pos != len(self.data):
            raise ValueError(f"trailing bytes at {self.pos:#x}")


def parse_spr(data):
    """Read counted, variable-size, row-major indexed sprites, requiring EOF."""
    reader = _Reader(data)
    rows = []
    for index in range(reader.u8()):
        offset = reader.pos
        width, height = reader.u8(), reader.u8()
        if not width or not height:
            raise ValueError(f"unsupported zero sprite dimension at {offset:#x}")
        rows.append(dict(index=index, offset=offset, width=width, height=height,
                         pixels=reader.take(width * height)))
    reader.eof()
    return rows


def parse_pal(data):
    """Read exactly 256 RGB triples with six-bit components; retain RGB6."""
    if len(data) != 768 or any(value > 63 for value in data):
        raise ValueError("expected exactly 768 bytes of RGB6 palette data")
    return list(struct.iter_unpack("BBB", data))


def rgb6_to_rgb8(entry):
    """Preview convention: round each RGB6 component to the range 0..255."""
    if len(entry) != 3 or any(not isinstance(v, int) or not 0 <= v <= 63 for v in entry):
        raise ValueError("expected three integer RGB6 components in 0..63")
    return tuple((value * 255 + 31) // 63 for value in entry)


def parse_scores(data):
    """Preserve unsigned score bits, length, and all eight stored name bytes."""
    reader = _Reader(data)
    rows = []
    for _ in range(reader.u8()):
        offset = reader.pos
        score = struct.unpack("<I", reader.take(4))[0]
        length, storage = reader.u8(), reader.take(8)
        if length > 8:
            raise ValueError(f"name length exceeds fixed storage at {offset:#x}")
        rows.append(dict(offset=offset, score_raw_u32=score, name_length=length,
                         name_storage=storage, name_bytes=storage[:length]))
    reader.eof()
    return rows


def parse_son(data):
    """Read six-byte tone/tick records, retaining the two uninterpreted bytes."""
    reader = _Reader(data)
    rows = []
    for index in range(reader.u16()):
        offset = reader.pos
        tone = reader.u16()
        gate, step = reader.u8(), reader.u8()
        rows.append(dict(index=index, offset=offset, tone_parameter=tone,
                         gate_ticks=gate, step_ticks=step,
                         opaque_tail=reader.take(2), stop=(tone == 30000)))
    reader.eof()
    return rows


def _rle_runs(payload):
    if len(payload) % 3:
        raise ValueError("incomplete RLE triple")
    return ((value, count)
            for control, a, b in struct.iter_unpack("BBB", payload)
            for value, count in ((a, (control >> 4) + 1), (b, (control & 15) + 1)))


def rle_prefix(payload):
    """Expand only the complete, declared payload; never read or invent a tail."""
    output = bytearray()
    for value, count in _rle_runs(payload):
        output.extend(bytes([value]) * count)
    return bytes(output)


def rle_strict(payload, required):
    """Return exactly required logical bytes or reject underfill.

    Like the original, stop at the requested output boundary (even inside a run).
    Unlike the original, validate triple framing, do not overread, and do not
    perform inclusive extra stores. Extra complete runs need not be consumed.
    """
    if not isinstance(required, int) or required < 0:
        raise ValueError("required output size must be a nonnegative integer")
    runs = _rle_runs(payload)  # Validate the entire framing even for required=0.
    output = bytearray()
    if required == 0:
        return bytes(output)
    for value, count in runs:
        output.extend(bytes([value]) * min(count, required - len(output)))
        if len(output) == required:
            return bytes(output)
    raise ValueError(f"underfilled plane: {len(output)} of {required} bytes")


def parse_levels(data):
    """Read concatenated level records to EOF, not a count-prefixed file.

    Each plane exposes compressed bytes, their complete logical prefix, the
    required size, missing bytes, and any excess logical output. Underfill is
    reported, not repaired; rle_strict is the separate full-plane decoder.
    """
    reader = _Reader(data)
    rows = []
    while reader.pos < len(data):
        offset = reader.pos
        width, height = reader.u16(), reader.u16()
        if not width or not height:
            raise ValueError(f"unsupported zero level dimension at {offset:#x}")
        tile, required_count, opaque = reader.u8(), reader.u16(), reader.u8()
        planes = []
        for required in (width * height, 2 * width * height):
            size = reader.u16()
            payload_offset = reader.pos
            payload = reader.take(size)
            prefix = rle_prefix(payload)
            planes.append(dict(offset=payload_offset, compressed=payload,
                               prefix=prefix, required=required,
                               missing=max(0, required - len(prefix)),
                               excess=max(0, len(prefix) - required)))
        words_offset = reader.pos
        words = (reader.u16(), reader.u16())
        trailers = []
        for stride in (30, 7, 14):
            count_offset = reader.pos
            count = reader.u8()
            trailers.append(dict(stride=stride, count_offset=count_offset,
                                 records=[reader.take(stride) for _ in range(count)]))
        rows.append(dict(offset=offset, end=reader.pos, width=width, height=height,
                         counted_tile_id=tile, required_count=required_count,
                         opaque_header_byte=opaque, planes=planes,
                         words_offset=words_offset, opaque_words=words,
                         trailers=trailers))
    reader.eof()
    return rows
