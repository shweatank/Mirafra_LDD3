import re

input_file = "video_frames.h"     # your current file
output_file = "all_frames.h"      # new combined file

# Read original file
with open(input_file, "r") as f:
    content = f.read()

# Remove Arduino-specific PROGMEM (not needed on Linux)
content = content.replace("PROGMEM", "")

# Extract all frame names
frame_names = re.findall(r'const\s+unsigned\s+char\s+(\w+)\s*\[', content)

# Start output content
out = "#define PROGMEM\n\n"
out += content + "\n\n"
out += "// Array of all frames\n"
out += "const unsigned char* frames[] = {\n"
for name in frame_names:
    out += f"    {name},\n"
out += "};\n"
out += "const int total_frames = sizeof(frames) / sizeof(frames[0]);\n"

# Save new file
with open(output_file, "w") as f:
    f.write(out)

print(f"Created {output_file} with {len(frame_names)} frames.")
