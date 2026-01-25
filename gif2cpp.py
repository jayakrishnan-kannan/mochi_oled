#!/usr/bin/env python3
import re
import sys
import os
from PIL import Image, ImageSequence, ImageFilter, ImageOps

WIDTH = 128
HEIGHT = 64
BYTES_PER_FRAME = WIDTH * HEIGHT // 8


def invert_byte(b):
    return b ^ 0xFF


THRESHOLD = 140  # tweak 120–160 depending on GIF
MIN_PIXEL_AREA = 2  # remove tiny dots


def img_to_xbm_bytes(img):
    # 1. Resize already done outside
    img = img.convert("L")  # grayscale

    # 2. Kill sparkle noise (very important)
    img = img.filter(ImageFilter.GaussianBlur(radius=1))

    # 3. Hard threshold (manual)
    img = img.point(lambda p: 255 if p > THRESHOLD else 0, mode="1")

    # 4. Morphological cleanup (remove dots)
    img = img.filter(ImageFilter.MinFilter(3))  # erosion
    img = img.filter(ImageFilter.MaxFilter(3))  # dilation

    pixels = img.load()
    data = []

    for y in range(HEIGHT):
        for xb in range(WIDTH // 8):
            byte = 0
            for bit in range(8):
                x = xb * 8 + bit
                if pixels[x, y] == 255:  # white pixel ON
                    byte |= 1 << bit

            # invert if OLED expects 0 = white
            data.append(byte)

    return data


def make_deltas(prev, curr):
    deltas = []
    for i, (a, b) in enumerate(zip(prev, curr)):
        if a != b:
            deltas.append((i, b))
    return deltas


def main(gif_path, out_cpp):
    gif = Image.open(gif_path)

    frames = []
    for f in ImageSequence.Iterator(gif):
        f = f.resize((WIDTH, HEIGHT), Image.NEAREST)
        frames.append(img_to_xbm_bytes(f))

    base = frames[0]
    delta_frames = [
        make_deltas(frames[i - 1], frames[i]) for i in range(1, len(frames))
    ]

    with open(f"include/{out_cpp}.h", "w") as f:
        f.write("#pragma once\n")
        f.write("#include <stdint.h>\n")
        f.write('#include "animate.h"\n\n')
        f.write(f"extern const uint8_t {out_cpp}_frame[1024];\n")
        f.write(f"extern const delta_t* {out_cpp}_delta_frames[];\n")
        f.write(f"extern const uint16_t {out_cpp}_delta_counts[];\n")

    with open(f"src/{out_cpp}.cpp", "w") as f:
        f.write("#include <stdint.h>\n")
        f.write('#include "' + out_cpp + '.h"\n')
        f.write("#include <Arduino.h>\n\n")

        # Base frame
        f.write(f"const uint8_t {out_cpp}_frame[1024] PROGMEM = {{\n")
        for i in range(0, len(base), 16):
            line = ", ".join(f"0x{b:02X}" for b in base[i : i + 16])
            f.write(f"  {line},\n")
        f.write("};\n\n")

        # Delta frames
        for i, deltas in enumerate(delta_frames):
            f.write(f"const delta_t frame_{i+1}_deltas[] PROGMEM = {{\n")
            for idx, val in deltas:
                f.write(f"  {{{idx}, 0x{val:02X}}},\n")
            f.write("};\n\n")

            f.write(f"const uint16_t frame_{i+1}_delta_count = {len(deltas)};\n\n")

        # Frame table
        f.write(f"const delta_t* {out_cpp}_delta_frames[] = {{\n")
        f.write("  NULL,\n")
        for i in range(1, len(frames)):
            f.write(f"  frame_{i}_deltas,\n")
        f.write("};\n\n")

        f.write(f"const uint16_t {out_cpp}_delta_counts[] = {{\n")
        f.write("  0,\n")
        for i in range(1, len(frames)):
            f.write(f"  frame_{i}_delta_count,\n")
        f.write("};\n")
        f.write(f"const uint16_t {out_cpp}_frame_count = {len(frames)};\n\n")
        f.write(f"const expression {out_cpp}_expression {{\n")
        f.write(f"  {out_cpp}_frame,\n")
        f.write(f"  {out_cpp}_delta_frames,\n")
        f.write(f"  {out_cpp}_delta_counts,\n")
        f.write(f"  {out_cpp}_frame_count\n")
        f.write("};\n\n")

    print(f"[OK] Generated {out_cpp}")


def cpp_to_gif(input_cpp, output_gif, fps=10):
    with open(input_cpp) as f:
        data = f.read()

    frame_count = int(re.search(r"FRAME_COUNT\s+(\d+)", data).group(1))

    # --- Parse base frame ---
    base_match = re.search(r"base_frame\[1024\][^{]*\{([^}]*)\}", data, re.S)
    base_bytes = [
        int(x, 16) for x in re.findall(r"0x[0-9A-Fa-f]{2}", base_match.group(1))
    ]

    frames = [base_bytes.copy()]

    # --- Parse delta frames ---
    for i in range(1, frame_count):
        delta_match = re.search(
            r"frame_{}_deltas\[\][^{{]*\{{([^}}]*)\}}".format(i), data, re.S
        )

        deltas = re.findall(
            r"\{\s*(\d+)\s*,\s*0x([0-9A-Fa-f]{2})\s*\}",
            delta_match.group(1),
        )

        new_frame = frames[-1].copy()
        for idx, val in deltas:
            new_frame[int(idx)] = int(val, 16)

        frames.append(new_frame)

    # --- Convert frames to GIF ---
    images = []
    for frame in frames:
        img = Image.new("P", (WIDTH, HEIGHT))

        palette = [0, 0, 0, 255, 255, 255] + [0, 0, 0] * 254
        img.putpalette(palette)

        pix = img.load()

        byte_idx = 0
        for y in range(HEIGHT):
            for xb in range(WIDTH // 8):
                # 🔁 INVERT BACK
                byte = invert_byte(frame[byte_idx])
                byte_idx += 1

                for bit in range(8):
                    x = xb * 8 + bit
                    pix[x, y] = 1 if (byte >> bit) & 1 else 0

        images.append(img)

    images[0].save(
        output_gif,
        save_all=True,
        append_images=images[1:],
        duration=int(1000 / fps),
        loop=0,
    )

    print(f"[OK] GIF reconstructed: {output_gif}")


if __name__ == "__main__":

    match len(sys.argv):
        case 2:
            if sys.argv[1].endswith((".mp4", ".gif")):
                # media_to_cpp(sys.argv[1], sys.argv[2])
                filename = os.path.basename(sys.argv[1])

                name_without_ext = os.path.splitext(filename)[0]

                main(sys.argv[1], name_without_ext)
            else:
                cpp_to_gif(sys.argv[1], sys.argv[2])
        case 3:
            main(sys.argv[1], sys.argv[2])
        case _:
            print("Usage: gif2cpp.py input.gif output.cpp")
            sys.exit(1)
