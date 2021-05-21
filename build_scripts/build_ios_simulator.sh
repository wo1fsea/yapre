rm -r ./build/ios_simulator
mkdir -p ./build/ios_simulator
cd ./build/ios_simulator
cmake ../.. \
    -GXcode -DYAPRE_BUILD_PLATFORM=ios \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_SYSROOT=iphonesimulator

open ./yapre.xcodeproj


