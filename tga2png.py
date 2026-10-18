#!/usr/bin/env python3
"""TGA -> PNG 转换器（只用 Python 标准库，无需 PIL/ImageMagick）。

用法:
    python3 tga2png.py framebuffer.tga            # 生成 framebuffer.png
    python3 tga2png.py framebuffer.tga out.png

支持 tinyrenderer 会生成的 TGA 格式：
  - 类型 2  : 未压缩真彩色（24/32 位）
  - 类型 10 : RLE 压缩真彩色（24/32 位）
  - 类型 3  : 未压缩灰度（8 位，用于 zbuffer.tga）
  - 类型 11 : RLE 压缩灰度（8 位）
  - 底部/顶部原点均可
"""
import struct
import sys
import zlib


def read_tga(path):
    with open(path, "rb") as f:
        data = f.read()
    (idlen, cmap_type, dtype, _cmap_org, _cmap_len, _cmap_depth,
     _x, _y, w, h, bpp, desc) = struct.unpack("<BBBHHBHHHHBB", data[:18])
    off = 18 + idlen

    if cmap_type != 0:
        raise ValueError("带调色板的 TGA 暂不支持")
    if dtype not in (2, 3, 10, 11):
        raise ValueError(f"只支持类型 2/3/10/11，当前类型={dtype}")
    if bpp not in (8, 24, 32):
        raise ValueError(f"只支持 8/24/32 位，当前 {bpp} 位")

    nbytes = bpp // 8
    npx = w * h
    pixels = bytearray(npx * 3)          # 统一转成 RGB

    def put(idx, px):
        o = idx * 3
        if nbytes == 1:                   # 灰度：一个字节，复制到 R/G/B
            pixels[o] = pixels[o + 1] = pixels[o + 2] = px[0]
        else:                             # 真彩色：BGR(A) → RGB
            b, g, r = px[0], px[1], px[2]
            pixels[o], pixels[o + 1], pixels[o + 2] = r, g, b

    if dtype in (2, 3):                   # 未压缩（真彩色 / 灰度）
        for i in range(npx):
            put(i, data[off + i * nbytes: off + (i + 1) * nbytes])
    else:                                 # RLE
        i = 0
        while i < npx:
            hdr = data[off]
            off += 1
            count = (hdr & 0x7F) + 1
            if hdr & 0x80:                # 连续重复同一个像素
                px = data[off: off + nbytes]
                off += nbytes
                for _ in range(count):
                    put(i, px)
                    i += 1
            else:                         # 原样像素
                for _ in range(count):
                    put(i, data[off: off + nbytes])
                    off += nbytes
                    i += 1

    # desc bit5 = 1 表示顶部原点；= 0 表示底部原点，需要垂直翻转
    if not (desc & 0x20):
        stride = w * 3
        rows = [pixels[r * stride:(r + 1) * stride] for r in range(h)]
        pixels = bytearray().join(reversed(rows))

    return w, h, bytes(pixels)


def write_png(path, w, h, rgb):
    def chunk(tag, payload):
        c = struct.pack(">I", len(payload)) + tag + payload
        return c + struct.pack(">I", zlib.crc32(tag + payload) & 0xFFFFFFFF)

    raw = bytearray()
    stride = w * 3
    for y in range(h):
        raw.append(0)                     # 每行的过滤器字节
        raw += rgb[y * stride:(y + 1) * stride]

    png = b"\x89PNG\r\n\x1a\n"
    png += chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0))
    png += chunk(b"IDAT", zlib.compress(bytes(raw), 9))
    png += chunk(b"IEND", b"")
    with open(path, "wb") as f:
        f.write(png)


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(1)
    src = sys.argv[1]
    dst = sys.argv[2] if len(sys.argv) > 2 else src.rsplit(".", 1)[0] + ".png"
    w, h, rgb = read_tga(src)
    write_png(dst, w, h, rgb)
    print(f"{src} ({w}x{h}) -> {dst}")


if __name__ == "__main__":
    main()
