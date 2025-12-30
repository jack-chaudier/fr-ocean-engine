#!/usr/bin/env python3
"""Create placeholder audio for Puzzle Match demo."""

import os

script_dir = os.path.dirname(os.path.abspath(__file__))
audio_dir = os.path.join(script_dir, "audio")
os.makedirs(audio_dir, exist_ok=True)

# Create empty placeholder audio files (game will work without actual audio)
# In a real game, you'd have actual .wav or .ogg files

print("Audio directory created at:", audio_dir)
print("Note: Add click.wav, match.wav, invalid.wav for sound effects")
