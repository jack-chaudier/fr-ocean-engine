#!/usr/bin/env python3
"""Generate the tiny sprite set the demo uses."""
import os
from PIL import Image, ImageDraw

HERE = os.path.dirname(os.path.abspath(__file__))
IMG = os.path.join(HERE, "images")
os.makedirs(IMG, exist_ok=True)


def dot(filename, color, size=32):
    img = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    for r in range(size // 2, 0, -1):
        a = int(255 * (1 - r / (size / 2)) ** 1.4)
        d.ellipse([size // 2 - r, size // 2 - r, size // 2 + r, size // 2 + r],
                  fill=(*color, a))
    img.save(os.path.join(IMG, filename))


dot("ball.png", (110, 200, 255))
dot("mover.png", (255, 180, 80))
dot("spark.png", (255, 255, 255), size=16)
print("Wrote sprites to", IMG)
