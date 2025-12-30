#!/usr/bin/env python3
"""Create placeholder sprites for Platformer demo."""

import os

try:
    from PIL import Image, ImageDraw
except ImportError:
    print("PIL not installed. Run: pip install Pillow")
    exit(1)

script_dir = os.path.dirname(os.path.abspath(__file__))
images_dir = os.path.join(script_dir, "images")
os.makedirs(images_dir, exist_ok=True)

# Player (blue character)
img = Image.new('RGBA', (64, 64), (0, 0, 0, 0))
draw = ImageDraw.Draw(img)
draw.rectangle([16, 24, 48, 56], fill=(0, 120, 200, 255))
draw.ellipse([20, 8, 44, 32], fill=(255, 200, 150, 255))
img.save(os.path.join(images_dir, 'player.png'))

# Platform (brown)
img = Image.new('RGBA', (64, 64), (139, 90, 43, 255))
img.save(os.path.join(images_dir, 'platform.png'))

# Enemy (red)
img = Image.new('RGBA', (64, 64), (0, 0, 0, 0))
draw = ImageDraw.Draw(img)
draw.ellipse([8, 8, 56, 56], fill=(200, 50, 50, 255))
draw.ellipse([18, 20, 28, 32], fill=(255, 255, 255, 255))
draw.ellipse([36, 20, 46, 32], fill=(255, 255, 255, 255))
img.save(os.path.join(images_dir, 'enemy.png'))

# Coin (yellow circle)
img = Image.new('RGBA', (64, 64), (0, 0, 0, 0))
draw = ImageDraw.Draw(img)
draw.ellipse([8, 8, 56, 56], fill=(255, 215, 0, 255))
draw.ellipse([16, 16, 48, 48], fill=(255, 235, 100, 255))
img.save(os.path.join(images_dir, 'coin.png'))

print("Platformer assets created in:", images_dir)
