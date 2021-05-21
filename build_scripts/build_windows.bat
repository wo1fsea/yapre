rmdir /S /Q .\build\windows
mkdir .\build\windows

cd .\build\windows
cmake ../.. -DYAPRE_BUILD_PLATFORM=windows
cmake --build .
