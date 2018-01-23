#!/usr/bin/python3

import sys, struct
from PIL import Image

if len(sys.argv) == 3:
    print("\nReading: " + sys.argv[1])
    out = open(sys.argv[2], "wb")
elif len(sys.argv) == 2:
    out = sys.stdout
else:
    print("Usage: png2fb.py infile [outfile]")
    sys.exit(1)

im = Image.open(sys.argv[1])

if im.mode == "RGB":
    pixelSize = 3
elif im.mode == "RGBA":
    pixelSize = 4
else:
    sys.exit('not supported pixel mode: "%s"' % (im.mode))

pixels = im.tobytes()
pixels2 = b''
for i in range(0, len(pixels) - 1, pixelSize):
    pixel888 = pixels[i:i+3]
    r = int(float(pixel888[0]) * 31.0 / 255.0 + 0.5)
    g = int(float(pixel888[1]) * 63.0 / 255.0 + 0.5)
    b = int(float(pixel888[2]) * 31.0 / 255.0 + 0.5)
    pixel565 = (r << (5 + 6)) | (g << 5) | b

    pixels2 += struct.pack('>H', pixel565)
if out == sys.stdout:
    sys.stdout.buffer.write(pixels2)
else:
    out.write(pixels2)
out.close()
