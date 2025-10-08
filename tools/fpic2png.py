import sys
import os
import struct
from PIL import Image

if __name__ == "__main__":
	if len(sys.argv) != 3:
		print("Usage: %s [image name] [image out]" % (sys.argv[0],))
		sys.exit(-1)

	f = open(sys.argv[1], "rb")
	data = f.read()

	_, width, height = struct.unpack("<QQQ", data[0:24])

	im = Image.new(mode="RGBA", size=(width, height))
	pixels = im.load()

	for y in range(height):
		for x in range(width):
			idx = ((x + y * width) * 4) + 24
			b, g, r, a = struct.unpack("<BBBB", data[idx:(idx + 4)])
			pixels[x, y] = (r, g, b, a)

	f.close()
	im.save(sys.argv[2], "PNG")

