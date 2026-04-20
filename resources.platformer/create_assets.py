#!/usr/bin/env python3
"""Generate sprite assets for the platformer demo.

Everything here is plain PIL — no external art is tracked. Run this once
after cloning to populate resources.platformer/images/.
"""
from __future__ import annotations
import os
from PIL import Image, ImageDraw, ImageFilter

HERE = os.path.dirname(os.path.abspath(__file__))
IMG = os.path.join(HERE, "images")
os.makedirs(IMG, exist_ok=True)


def save(img: Image.Image, name: str) -> None:
    img.save(os.path.join(IMG, name))
    print("  ", name)


def rounded(size, color, radius=8):
    img = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.rounded_rectangle([0, 0, size - 1, size - 1], radius, fill=color)
    return img


# --- Player: blue body, skin-tone head, two eyes, small highlight -----------
def player():
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    # body
    d.rounded_rectangle([16, 28, 48, 58], 6, fill=(50, 130, 220, 255))
    d.rounded_rectangle([16, 28, 48, 36], 4, fill=(80, 160, 240, 255))  # shirt hl
    # legs
    d.rectangle([20, 54, 28, 62], fill=(30, 90, 160, 255))
    d.rectangle([36, 54, 44, 62], fill=(30, 90, 160, 255))
    # head
    d.ellipse([18, 6, 46, 34], fill=(255, 210, 170, 255))
    # hair
    d.chord([18, 4, 46, 22], 180, 360, fill=(90, 60, 40, 255))
    # eyes
    d.ellipse([25, 16, 30, 22], fill=(30, 30, 30, 255))
    d.ellipse([35, 16, 40, 22], fill=(30, 30, 30, 255))
    d.ellipse([27, 17, 29, 19], fill=(255, 255, 255, 255))
    d.ellipse([37, 17, 39, 19], fill=(255, 255, 255, 255))
    # smile
    d.arc([27, 22, 37, 28], 0, 180, fill=(150, 70, 70, 255), width=1)
    save(img, "player.png")


# --- Enemy: rounded red blob with angry eyes -------------------------------
def enemy():
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse([6, 10, 58, 58], fill=(200, 60, 60, 255))
    d.ellipse([6, 10, 58, 26], fill=(230, 100, 100, 255))  # top highlight
    # feet
    d.ellipse([10, 52, 22, 62], fill=(130, 30, 30, 255))
    d.ellipse([42, 52, 54, 62], fill=(130, 30, 30, 255))
    # angry brows
    d.polygon([(16, 22), (30, 28), (14, 30)], fill=(60, 10, 10, 255))
    d.polygon([(48, 22), (34, 28), (50, 30)], fill=(60, 10, 10, 255))
    # eyes
    d.ellipse([18, 28, 28, 38], fill=(255, 255, 255, 255))
    d.ellipse([36, 28, 46, 38], fill=(255, 255, 255, 255))
    d.ellipse([22, 32, 26, 36], fill=(20, 20, 20, 255))
    d.ellipse([40, 32, 44, 36], fill=(20, 20, 20, 255))
    # fang
    d.polygon([(28, 40), (32, 48), (36, 40)], fill=(255, 255, 255, 255))
    save(img, "enemy.png")


# --- Coin: gold disc with inner shine --------------------------------------
def coin():
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse([6, 6, 58, 58], fill=(220, 160, 30, 255))
    d.ellipse([12, 12, 52, 52], fill=(255, 210, 60, 255))
    d.ellipse([20, 20, 44, 44], fill=(255, 235, 120, 255))
    # dollar-ish mark
    d.rectangle([30, 18, 34, 46], fill=(200, 140, 20, 255))
    d.arc([22, 20, 42, 32], 0, 270, fill=(200, 140, 20, 255), width=2)
    d.arc([22, 32, 42, 44], 90, 360, fill=(200, 140, 20, 255), width=2)
    save(img, "coin.png")


# --- Platform: stone-gray with grass top -----------------------------------
def platform():
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    # dirt body
    d.rectangle([0, 14, 63, 63], fill=(120, 85, 55, 255))
    # dark base
    d.rectangle([0, 50, 63, 63], fill=(80, 55, 35, 255))
    # grass top
    d.rectangle([0, 0, 63, 14], fill=(80, 180, 70, 255))
    d.rectangle([0, 12, 63, 16], fill=(50, 140, 50, 255))
    # tufts
    for x in (4, 20, 38, 54):
        d.polygon([(x, 12), (x + 2, 6), (x + 4, 12)], fill=(60, 160, 60, 255))
    save(img, "platform.png")


# --- Moving platform: blue stone -------------------------------------------
def platform_moving():
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.rounded_rectangle([0, 0, 63, 63], 6, fill=(80, 130, 180, 255))
    d.rectangle([0, 0, 63, 16], fill=(120, 170, 220, 255))
    # bolts
    for x in (8, 56):
        d.ellipse([x - 4, 28, x + 4, 36], fill=(40, 70, 110, 255))
    save(img, "platform_moving.png")


# --- Spike trap: dark triangles --------------------------------------------
def spike():
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    # three upward-pointing spikes
    for i, (x0, x1) in enumerate([(2, 22), (22, 42), (42, 62)]):
        d.polygon([(x0, 60), (x1, 60), ((x0 + x1) // 2, 10)], fill=(90, 90, 100, 255))
        d.polygon([(x0 + 2, 60), ((x0 + x1) // 2, 16), ((x0 + x1) // 2 - 2, 60)],
                  fill=(140, 140, 150, 255))
    save(img, "spike.png")


# --- Flag: checkered banner on a pole --------------------------------------
def flag():
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    # pole
    d.rectangle([14, 4, 20, 60], fill=(80, 60, 40, 255))
    # banner
    d.polygon([(20, 8), (56, 16), (20, 26)], fill=(230, 60, 60, 255))
    d.polygon([(20, 28), (56, 36), (20, 46)], fill=(230, 200, 60, 255))
    save(img, "flag.png")


# --- Particle dot: soft radial gradient -----------------------------------
def particle():
    size = 16
    img = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    for r in range(size // 2, 0, -1):
        alpha = int(255 * (1 - r / (size / 2)) ** 2)
        d.ellipse([size // 2 - r, size // 2 - r, size // 2 + r, size // 2 + r],
                  fill=(255, 255, 255, alpha))
    save(img, "particle.png")


# --- Background: soft sky with distant hills -------------------------------
def background():
    w, h = 1280, 720
    img = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    # gradient sky
    for y in range(h):
        t = y / h
        r = int(135 * (1 - t) + 255 * t * 0.6)
        g = int(206 * (1 - t) + 230 * t * 0.7)
        b = int(235 * (1 - t) + 200 * t * 0.9)
        ImageDraw.Draw(img).line([(0, y), (w, y)], fill=(r, g, b, 255))
    # hills
    d = ImageDraw.Draw(img)
    d.ellipse([-200, h - 260, 600, h + 100], fill=(130, 190, 120, 255))
    d.ellipse([500, h - 200, 1100, h + 100], fill=(110, 170, 110, 255))
    d.ellipse([900, h - 300, 1500, h + 100], fill=(100, 160, 100, 255))
    # clouds
    for (cx, cy) in [(200, 140), (700, 90), (1100, 170)]:
        for dx, dy, r in [(0, 0, 40), (40, -10, 30), (-40, 0, 30), (20, 10, 35)]:
            d.ellipse([cx + dx - r, cy + dy - r, cx + dx + r, cy + dy + r],
                      fill=(255, 255, 255, 220))
    img = img.filter(ImageFilter.SMOOTH)
    save(img, "background.png")


if __name__ == "__main__":
    print("Writing sprites to", IMG)
    player()
    enemy()
    coin()
    platform()
    platform_moving()
    spike()
    flag()
    particle()
    background()
    print("Done.")
