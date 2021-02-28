rm -rf build/android
mkdir -p build/android

export ANDROID_HOME="Users/huangquanyong/Library/Android/sdk/"
export ANDROID_NDK_HOME="Users/huangquanyong/Library/Android/sdk/ndk-bundle"

cd ./external/SDL/build-scripts
sh ./androidbuild.sh com.yapre ../../../src/main.cpp

mv ../build/com.yapre ../../../build/android

