rm -r ./build/ios
mkdir -p ./build/ios
cd ./build/ios
cmake ../.. \
    -GXcode -DYAPRE_BUILD_PLATFORM=ios \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_SYSROOT=iphonesimulator
# cmake .. -GXcode -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_SYSROOT=iphoneos
