#!/usr/bin/env python3
"""
Create placeholder sprite assets for the Space Shooter example.
Requires: pip install Pillow

Run: python3 create_assets.py
"""

import os

try:
    from PIL import Image, ImageDraw
except ImportError:
    print("Pillow is required. Install with: pip install Pillow")
    exit(1)

# Get the directory where this script is located
script_dir = os.path.dirname(os.path.abspath(__file__))
images_dir = os.path.join(script_dir, "images")

# Ensure images directory exists
os.makedirs(images_dir, exist_ok=True)

# Create player sprite (blue triangle/ship shape)
player = Image.new('RGBA', (32, 32), (0, 0, 0, 0))
draw = ImageDraw.Draw(player)
draw.polygon([(16, 0), (0, 31), (32, 31)], fill=(0, 150, 255, 255))
draw.polygon([(16, 8), (8, 31), (24, 31)], fill=(0, 100, 200, 255))
player.save(os.path.join(images_dir, 'player.png'))
print("Created: player.png")

# Create enemy sprite (red inverted triangle)
enemy = Image.new('RGBA', (32, 32), (0, 0, 0, 0))
draw = ImageDraw.Draw(enemy)
draw.polygon([(16, 31), (0, 0), (32, 0)], fill=(255, 50, 50, 255))
draw.polygon([(16, 24), (8, 0), (24, 0)], fill=(200, 30, 30, 255))
enemy.save(os.path.join(images_dir, 'enemy.png'))
print("Created: enemy.png")

# Create player bullet (cyan rectangle)
bullet = Image.new('RGBA', (8, 16), (0, 0, 0, 0))
draw = ImageDraw.Draw(bullet)
draw.rectangle([1, 0, 6, 15], fill=(0, 255, 255, 255))
draw.rectangle([2, 0, 5, 15], fill=(150, 255, 255, 255))
bullet.save(os.path.join(images_dir, 'bullet.png'))
print("Created: bullet.png")

# Create enemy bullet (orange rectangle)
enemy_bullet = Image.new('RGBA', (8, 16), (0, 0, 0, 0))
draw = ImageDraw.Draw(enemy_bullet)
draw.rectangle([1, 0, 6, 15], fill=(255, 150, 0, 255))
draw.rectangle([2, 0, 5, 15], fill=(255, 200, 100, 255))
enemy_bullet.save(os.path.join(images_dir, 'enemy_bullet.png'))
print("Created: enemy_bullet.png")

print("\nAll placeholder sprites created successfully!")
print("\nNote: You still need to add a font file:")
print("  - Download OpenSans-Regular.ttf from https://fonts.google.com/specimen/Open+Sans")
print("  - Place it in: " + os.path.join(script_dir, "fonts", "OpenSans-Regular.ttf"))
