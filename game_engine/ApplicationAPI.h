#ifndef APPLICATION_API_HPP
#define APPLICATION_API_HPP

#include <cstdlib>
#include <chrono>
#include <thread>
#include <string>
#include <iostream>

#ifdef _WIN32
    #define PLATFORM_COMMAND "start "
#elif defined(__APPLE__)
    #define PLATFORM_COMMAND "open "
#else
    #define PLATFORM_COMMAND "xdg-open "
#endif

namespace ApplicationAPI {
    inline void Quit() { std::exit(0); }

    inline void Sleep(int milliseconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    inline int GetFrame() { return Helper::GetFrameNumber(); }

    inline void OpenURL(const std::string& url) {
        std::string command = PLATFORM_COMMAND + url;
        std::system(command.c_str());
    }
}

#endif // APPLICATION_API_HPP
