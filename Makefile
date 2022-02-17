.PHONY: clean release debug build rebuild serve dump

all: rebuild

rebuild: clean build

build:
	emcmake cmake -S . -B build
	cmake --build ./build --verbose

serve:
	cd ./build/html && python -m SimpleHTTPServer

dump:
	wasm2wat ./build/html/wasm/web_api.wasm -o web_api.wat

clean:
	@rm -rf build

release: clean
	emcmake cmake -S . -B build -DCMAKE_BUILD_TYPE=RELEASE
	cmake --build ./build --clean-first --verbose

debug: clean
	emcmake cmake -S . -B build -DCMAKE_BUILD_TYPE=DEBUG
	cmake --build ./build --clean-first --verbose
