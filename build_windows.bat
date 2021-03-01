rmdir /S /Q .\build\windows
mkdir .\build\windows

cd .\build\windows
cmake ../..
cmake --build .
