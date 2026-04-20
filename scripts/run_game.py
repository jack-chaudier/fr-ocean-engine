#!/usr/bin/env python3
import argparse
import os
import platform
import subprocess
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))

GAMES = {
    "platformer": "resources.platformer",
    "demo": "resources.demo",
}


def run(cmd, cwd=None):
    print("+", " ".join(cmd))
    return subprocess.call(cmd, cwd=cwd)

def read_cache_source(cache_path):
    try:
        with open(cache_path, "r", encoding="utf-8") as handle:
            for line in handle:
                if line.startswith("CMAKE_HOME_DIRECTORY:INTERNAL="):
                    return line.split("=", 1)[1].strip()
    except OSError:
        return None
    return None

def resolve_build_dir(build_dir):
    cache_path = os.path.join(build_dir, "CMakeCache.txt")
    if not os.path.exists(cache_path):
        return build_dir

    cached_source = read_cache_source(cache_path)
    if cached_source and os.path.abspath(cached_source) != ROOT:
        fallback_dir = os.path.join(ROOT, "build-local")
        print(
            "Warning: build cache points to a different source directory.\n"
            f"Using fallback build dir: {fallback_dir}"
        )
        return fallback_dir

    return build_dir


def build_engine(build_dir, config, generator):
    build_dir = resolve_build_dir(build_dir)
    os.makedirs(build_dir, exist_ok=True)
    configure_cmd = ["cmake", "-S", ROOT, "-B", build_dir]
    if generator:
        configure_cmd.extend(["-G", generator])
    configure_cmd.append(f"-DCMAKE_BUILD_TYPE={config}")
    if run(configure_cmd) != 0:
        return 1
    return run(["cmake", "--build", build_dir, "--config", config])


def get_binary_path(build_dir):
    exe = "game_engine.exe" if platform.system() == "Windows" else "game_engine"
    return os.path.join(build_dir, "bin", exe)


def main():
    parser = argparse.ArgumentParser(description="Build and run FR-Ocean example games.")
    parser.add_argument("game", nargs="?", default="platformer", choices=GAMES.keys())
    parser.add_argument("--list", action="store_true", help="List available games")
    parser.add_argument("--all", action="store_true", help="Run all games sequentially")
    parser.add_argument("--no-build", action="store_true", help="Skip build step")
    parser.add_argument("--config", default="Release", choices=["Debug", "Release", "RelWithDebInfo", "MinSizeRel"])
    parser.add_argument("--build-dir", default=os.path.join(ROOT, "build"))
    parser.add_argument("--generator", help="CMake generator override (e.g., Ninja, \"Visual Studio 17 2022\")")
    parser.add_argument("--resources", help="Override resources path")
    args = parser.parse_args()

    if args.list:
        print("Available games:")
        for key in GAMES:
            print(" -", key)
        return 0

    if args.all:
        games_to_run = list(GAMES.keys())
    else:
        games_to_run = [args.game]

    if not args.no_build:
        if build_engine(args.build_dir, args.config, args.generator) != 0:
            return 1

    build_dir = resolve_build_dir(args.build_dir)
    binary = get_binary_path(build_dir)
    if not os.path.exists(binary):
        print(f"Error: binary not found at {binary}", file=sys.stderr)
        return 1

    for game in games_to_run:
        resources_path = args.resources or os.path.join(ROOT, GAMES[game])
        result = run([binary, "--resources", resources_path])
        if result != 0:
            return result
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
