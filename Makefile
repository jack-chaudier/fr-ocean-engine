.PHONY: build play demo test screenshots assets clean reconfigure help

BIN := build/bin/game_engine

help:
	@echo "Targets:"
	@echo "  make build         Configure & build the engine (Release)"
	@echo "  make play          Run the platformer sample game"
	@echo "  make demo          Run the feature-demo sample game"
	@echo "  make test          Run CTest smoke tests"
	@echo "  make screenshots   Regenerate docs/screenshots/*.png"
	@echo "  make assets        Regenerate generated PNG sprites (requires uv or Pillow)"
	@echo "  make reconfigure   Re-run cmake configure"
	@echo "  make clean         Delete the build/ directory"

build: $(BIN)

$(BIN):
	cmake --preset release
	cmake --build --preset release

reconfigure:
	cmake --preset release

play: build
	$(BIN) --resources resources.platformer/

demo: build
	$(BIN) --resources resources.demo/

test: build
	cd build && ctest --output-on-failure

screenshots: build
	@mkdir -p docs/screenshots
	$(BIN) --resources resources.platformer/ --scene title   --screenshot docs/screenshots/title.png   --self-check 90
	$(BIN) --resources resources.platformer/ --scene level1  --screenshot docs/screenshots/level1.png  --self-check 120
	$(BIN) --resources resources.platformer/ --scene level2  --screenshot docs/screenshots/level2.png  --self-check 120
	$(BIN) --resources resources.platformer/ --scene victory --screenshot docs/screenshots/victory.png --self-check 90
	$(BIN) --resources resources.demo/       --screenshot docs/screenshots/demo.png     --self-check 120

assets:
	@command -v uv >/dev/null 2>&1 && \
	  { uv run --with Pillow python3 resources.platformer/create_assets.py; \
	    uv run --with Pillow python3 resources.demo/create_assets.py; } || \
	  { python3 -c "import PIL" 2>/dev/null && \
	    { python3 resources.platformer/create_assets.py; \
	      python3 resources.demo/create_assets.py; } || \
	    { echo "Need 'uv' or Pillow installed. Try: pip install Pillow"; exit 1; }; }

clean:
	rm -rf build
