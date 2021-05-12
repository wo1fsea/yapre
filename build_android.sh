rm -rf ./build/android
mkdir -p ./build/android

cp -r ./platform/android/run.yapre ./build/android/run.yapre

ln -s ../../../../../src ./build/android/run.yapre/app/jni/src
ln -s ../../../../../external ./build/android/run.yapre/app/jni/external
ln -s ../../../../../../../data ./build/android/run.yapre/app/src/main/assets/data

