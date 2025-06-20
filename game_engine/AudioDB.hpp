//
//  AudioDB.hpp
//  game_engine
//
//  Created by Jack Gaffney on 2/11/25.
//

#ifndef AudioDB_hpp
#define AudioDB_hpp

#include <string>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include "SDL2_mixer/SDL_mixer.h"
#include "AudioHelper.h"

class AudioDB {
public:
    static void Init();
    static void PlayChannel(int channel, const std::string & audio_clip_name, bool does_loop);
    static void HaltChannel(int channel);
    static void SetVolume(int channel, float volume); // New method for volume control
    
private:
    static inline std::unordered_map<std::string, Mix_Chunk*> loaded_audio;
    
};

#endif /* AudioDB_hpp */
