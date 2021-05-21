rm -r ./build/linux
mkdir -p ./build/linux
cd ./build/linux
cmake ../.. -DYAPRE_BUILD_PLATFORM=linux
cmake --build .
