. ./platform/emscripten/setup_emsdk.sh

setup_emsdk
emscripten_path="./tools/emsdk"
source "$emscripten_path/emsdk_env.sh"

rm -r ./build/emscripten
mkdir -p ./build/emscripten
cd ./build/emscripten
cmake ../..  \
    -DYAPRE_BUILD_PLATFORM=emscripten \
    -DCMAKE_TOOLCHAIN_FILE="$emscripten_path/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" 
cmake --build .
