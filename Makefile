CXX := clang++
CXXFLAGS := -std=c++17 -Wall -Wextra -O3

# Find all source files recursively
SRCS := $(shell find game_engine -name "*.cpp")

INCLUDE_DIRS := -Igame_engine \
                -Ivendor \
                -Ivendor/rapidjson \
                -Ivendor/rapidjson/include \
                -Ivendor/glm \
                -Ivendor/glm/detail \
                -Ivendor/glm/ext \
                -Ivendor/glm/gtc \
                -Ivendor/glm/gtx \
                -Ivendor/glm/simd \
                -Ivendor/SDL2 \
				-Ivendor/SDL2_image \
				-Ivendor/SDL2_ttf \
				-Ivendor/SDL2_mixer \
				-Ivendor/LuaBridge \
                -Ivendor/lua \
				-Ivendor/box2d


LDFLAGS := -llua5.4 -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf

TARGET := game_engine_linux

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(TARGET)

.PHONY: all clean

