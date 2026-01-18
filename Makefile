CXX = g++
CXXFLAGS = $(shell pkg-config --cflags sdl3)
LDFLAGS = $(shell pkg-config --libs sdl3)

ASSETS_SRC := assets
ASSETS_DST := build/assets

build/app: machinelove.cpp clean assets
	mkdir -p build
	mkdir -p build/assets
	cp -r assets/* build/assets/
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)
	
assets:
	mkdir -p $(ASSETS_DST)
	cp -r $(ASSETS_SRC)/* $(ASSETS_DST)/

clean:
	rm -rf build

run: build/app
	./build/app