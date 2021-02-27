
emscripten_path="~/emsdk"

rm -r ./build
mkdir ./build
cd ./build
cmake -DYAPRE_BUILD_PLATFORM=emscripten -DCMAKE_TOOLCHAIN_FILE="$emscripten_path/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" ..
cmake --build .
