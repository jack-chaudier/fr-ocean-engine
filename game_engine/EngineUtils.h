//
//  EngineUtils.h
//  game_engine
//
//  Created by Jack Gaffney on 1/28/25.
//

#ifndef EngineUtils_h
#define EngineUtils_h

#include <iostream>
#include <cstdio>
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include "Logger.hpp"
#include "EngineException.hpp"

/**
 * @class EngineUtils
 * @brief Utility functions for engine operations.
 */
class EngineUtils {
public:
    /**
     * @brief Read and parse a JSON file.
     * @param path Path to the JSON file.
     * @param out_document Document to populate with parsed JSON.
     * @throws ConfigurationException if file cannot be opened or parsed.
     */
    static void ReadJsonFile(const std::string& path, rapidjson::Document& out_document)
    {
        FILE* file_pointer = nullptr;
#ifdef _WIN32
        fopen_s(&file_pointer, path.c_str(), "rb");
#else
        file_pointer = fopen(path.c_str(), "rb");
#endif

        if (file_pointer == nullptr) {
            LOG_FATAL("Failed to open file: " + path);
            throw ConfigurationException("Cannot open file: " + path);
        }

        char buffer[65536];
        rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
        out_document.ParseStream(stream);
        std::fclose(file_pointer);

        if (out_document.HasParseError()) {
            LOG_FATAL("JSON parse error in file: " + path);
            throw ConfigurationException("JSON parse error in file: " + path);
        }
    }
};

#endif /* EngineUtils_h */
