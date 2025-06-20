//
//  AudioDB.cpp
//  game_engine
//
//  Created by Jack Gaffney on 2/11/25.
//

#include <vector>
#include <string>
#include "AudioDB.hpp"

void AudioDB::Init() {
    AudioHelper::Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    AudioHelper::Mix_AllocateChannels(50);
}

void AudioDB::PlayChannel(int channel, const std::string &audio_clip_name, bool does_loop) {
    int loop = does_loop ? -1 : 0;
    if (loaded_audio.find(audio_clip_name) == loaded_audio.end()) {
        std::string pathWav = "resources/audio/" + audio_clip_name + ".wav";
        std::string pathOgg = "resources/audio/" + audio_clip_name + ".ogg";
        
        std::string f2l;
        
        if (std::filesystem::exists(pathWav)) {
            f2l = pathWav;
        }
        else if (std::filesystem::exists(pathOgg)) {
            f2l = pathOgg;
        }
        else {
            std::cout << "error: failed to play audio clip " << audio_clip_name;
            exit(0);
        }
        loaded_audio[audio_clip_name] = AudioHelper::Mix_LoadWAV(f2l.c_str());
        
        if (!loaded_audio[audio_clip_name]) {
            std::cout << "error: failed to play audio clip " << audio_clip_name;
            exit(0);
        }
    }
    AudioHelper::Mix_PlayChannel(channel, loaded_audio[audio_clip_name], loop);
}

void AudioDB::HaltChannel(int channel) {
    AudioHelper::Mix_HaltChannel(channel);
}

void AudioDB::SetVolume(int channel, float volume) {
    int vol = static_cast<int>(volume);
    AudioHelper::Mix_Volume(channel, vol);
}
