//
//  AudioDB.hpp
//  FR-Ocean Engine
//
//  Audio playback system using SDL_mixer for sound effects and music with
//  channel-based mixing and volume control.
//
//  Created by Jack Gaffney on 2/11/25.
//

#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include "SDL2_mixer/SDL_mixer.h"
#include "AudioHelper.h"

/**
 * @class AudioDB
 * @brief Audio playback system for sound effects and music using SDL_mixer.
 *
 * AudioDB manages audio loading, caching, and playback via SDL_mixer's channel system.
 * It supports multiple simultaneous sounds, looping, and per-channel volume control.
 *
 * Features:
 * - Automatic audio file loading and caching (WAV, OGG, MP3)
 * - Multi-channel mixing (16 channels by default)
 * - Looping sound support
 * - Per-channel volume control (0.0 to 1.0)
 * - Channel management (play, stop, volume)
 *
 * Channel System:
 * - Multiple sounds can play simultaneously on different channels (0-15)
 * - Each channel can play one sound at a time
 * - Use channel -1 to auto-select the first available channel
 *
 * Usage (from Lua):
 * ```lua
 * Audio.Play(0, "explosion", false)      -- Play once on channel 0
 * Audio.Play(1, "background_music", true) -- Loop on channel 1
 * Audio.SetVolume(0, 0.5)                            -- Set channel 0 to 50% volume
 * Audio.Halt(1)                               -- Stop channel 1
 * ```
 *
 * @note AudioDB uses static methods for global access from Lua scripts
 */
class AudioDB {
public:
    /**
     * @brief Initializes SDL_mixer audio system.
     *
     * @throws AudioException if SDL_mixer initialization fails
     *
     * @note Called automatically by Engine constructor
     */
    static void Init();

    /**
     * @brief Shuts down SDL_mixer and releases cached audio.
     */
    static void Shutdown();

    /**
     * @brief Plays an audio clip on a specific channel.
     *
     * @param channel Channel number (0-15, or -1 for auto-select first available)
     * @param audio_clip_name Audio filename without extension (e.g., "explosion")
     * @param does_loop If true, audio loops infinitely; if false, plays once
     *
     * @note Audio files are loaded from resources/audio/ and cached for reuse
     * @throws ResourceNotFoundException if the audio clip is missing
     * @throws AudioException if the audio clip fails to load
     */
    static void PlayChannel(int channel, const std::string & audio_clip_name, bool does_loop);

    /**
     * @brief Stops playback on a specific channel.
     *
     * @param channel Channel number to stop (0-15)
     *
     * @note This immediately stops the sound and frees the channel
     */
    static void HaltChannel(int channel);

    /**
     * @brief Sets the volume for a specific channel.
     *
     * @param channel Channel number (0-15)
     * @param volume Volume level (0.0 = silent, 1.0 = full volume)
     *
     * @note Volume is clamped to [0.0, 1.0] range internally
     */
    static void SetVolume(int channel, float volume);

private:
    /// Audio cache: filename -> Mix_Chunk* (loaded sound data)
    static inline std::unordered_map<std::string, Mix_Chunk*> loaded_audio;

    static bool IsAutograderMode();

};

