rm -r ./build/mac
mkdir -p ./build/mac
cd ./build/mac
cmake ../.. -DYAPRE_BUILD_PLATFORM=mac -DCMAKE_BUILD_TYPE=Release
cmake --build .
