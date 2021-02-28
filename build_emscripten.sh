
emscripten_path="~/emsdk"

rm -r ./build/emscripten
mkdir -p ./build/emscripten
cd ./build/emscripten
cmake ../..  \
    -DYAPRE_BUILD_PLATFORM=emscripten \
    -DCMAKE_TOOLCHAIN_FILE="$emscripten_path/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" 
cmake --build .
