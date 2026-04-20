//
//  AudioDB.cpp
//  game_engine
//
//  Created by Jack Gaffney on 2/11/25.
//

#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include "AudioDB.hpp"
#include "Logger.hpp"
#include "EngineException.hpp"
#include "ConfigManager.hpp"

void AudioDB::Init() {
    if (AudioHelper::Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0) {
        std::string error = Mix_GetError();
        LOG_FATAL("SDL_mixer initialization failed: " + error);
        throw AudioException("SDL_mixer initialization failed: " + error);
    }
    if (AudioHelper::Mix_AllocateChannels(50) < 0) {
        std::string error = Mix_GetError();
        LOG_FATAL("SDL_mixer channel allocation failed: " + error);
        throw AudioException("SDL_mixer channel allocation failed: " + error);
    }
}

void AudioDB::PlayChannel(int channel, const std::string &audio_clip_name, bool does_loop) {
    int loop = does_loop ? -1 : 0;
    if (loaded_audio.find(audio_clip_name) == loaded_audio.end()) {
        std::string pathWav = ConfigManager::GetResourcesPath() + "audio/" + audio_clip_name + ".wav";
        std::string pathOgg = ConfigManager::GetResourcesPath() + "audio/" + audio_clip_name + ".ogg";

        std::string f2l;

        if (std::filesystem::exists(pathWav)) {
            f2l = pathWav;
        }
        else if (std::filesystem::exists(pathOgg)) {
            f2l = pathOgg;
        }
        else {
            LOG_FATAL("Audio clip missing: " + audio_clip_name);
            throw ResourceNotFoundException("audio clip", audio_clip_name);
        }
        loaded_audio[audio_clip_name] = AudioHelper::Mix_LoadWAV(f2l.c_str());

        if (!loaded_audio[audio_clip_name]) {
            LOG_FATAL("Failed to load audio clip: " + audio_clip_name);
            throw AudioException("Failed to load audio clip: " + audio_clip_name);
        }
    }
    AudioHelper::Mix_PlayChannel(channel, loaded_audio[audio_clip_name], loop);
}

void AudioDB::HaltChannel(int channel) {
    AudioHelper::Mix_HaltChannel(channel);
}

void AudioDB::SetVolume(int channel, float volume) {
    int vol = static_cast<int>(std::clamp(volume, 0.0f, 1.0f) * 128);
    AudioHelper::Mix_Volume(channel, vol);
}

void AudioDB::Shutdown() {
    if (!IsAutograderMode()) {
        for (auto& pair : loaded_audio) {
            if (pair.second) {
                Mix_FreeChunk(pair.second);
            }
        }
    }
    loaded_audio.clear();
    AudioHelper::Mix_CloseAudio();
}

bool AudioDB::IsAutograderMode() {
#ifdef _WIN32
    char* val = nullptr;
    size_t length = 0;
    _dupenv_s(&val, &length, "AUTOGRADER");
    if (val) {
        free(val);
        return true;
    }
    return false;
#else
    const char* autograder_mode_env_variable = std::getenv("AUTOGRADER");
    return autograder_mode_env_variable != nullptr;
#endif
}
