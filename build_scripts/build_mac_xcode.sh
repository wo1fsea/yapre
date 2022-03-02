rm -r ./build/xcode
mkdir -p ./build/xcode
cd ./build/xcode
cmake ../.. -DYAPRE_BUILD_PLATFORM=mac -GXcode
open ./yapre.xcodeproj

