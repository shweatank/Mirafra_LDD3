from PIL import Image
import os

# ==== CONFIG ====
INPUT_DIR = "frames"      # Folder containing your images
OUTPUT_FILE = "video_frames.h"
IMG_WIDTH = 32           # OLED width
IMG_HEIGHT = 32           # OLED height
BACKGROUND_WHITE = False   # If True, white=0, black=1
# =================

def image_to_bitmap(image_path):
    img = Image.open(image_path).convert("1")  # 1-bit mode
    img = img.resize((IMG_WIDTH, IMG_HEIGHT), Image.LANCZOS)

    pixels = img.load()
    bitmap_data = []

    for page in range(IMG_HEIGHT // 8):  # SSD1306 pages
        for x in range(IMG_WIDTH):
            byte = 0
            for bit in range(8):
                y = page * 8 + bit
                pixel = pixels[x, y]
                if BACKGROUND_WHITE:
                    bit_val = 0 if pixel == 255 else 1
                else:
                    bit_val = 1 if pixel == 255 else 0
                byte |= (bit_val << bit)
            bitmap_data.append(byte)

    return bitmap_data

def main():
    files = sorted([f for f in os.listdir(INPUT_DIR) if f.lower().endswith(('.png', '.jpg', '.jpeg'))])

    with open(OUTPUT_FILE, "w") as f:
        f.write("// All video frames bitmaps\n\n")
        for idx, file in enumerate(files):
            path = os.path.join(INPUT_DIR, file)
            data = image_to_bitmap(path)

            f.write(f"const unsigned char frame_{idx}[] PROGMEM = {{\n")
            for i, byte in enumerate(data):
                f.write(f"0x{byte:02X}, ")
                if (i + 1) % 16 == 0:
                    f.write("\n")
            f.write("\n};\n\n")

        # Optional: array of pointers to frames
        f.write(f"const unsigned char* all_frames[{len(files)}] PROGMEM = {{\n")
        for idx in range(len(files)):
            f.write(f"    frame_{idx},\n")
        f.write("};\n")

    print(f"✅ Done! {len(files)} frames saved in {OUTPUT_FILE}")

if __name__ == "__main__":
    main()
