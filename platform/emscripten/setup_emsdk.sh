setup_emsdk()
{
    if [ ! -d "./tools/emsdk" ]
    then
        if [ ! -d "./tools" ]
        then
            mkdir ./tools
        fi
        
        cd ./tools
        git clone https://github.com/emscripten-core/emsdk.git
        
        cd ./emsdk
        git pull
        
        # ./emsdk install latest
        # ./emsdk activate latest
        ./emsdk install 3.1.25
        ./emsdk activate 3.1.25
        
        cd upstream/emscripten/cache/sysroot/include
        mv ./SDL ./_SDL
        cd ../../../../..
        
        cd ../..
        
    fi
}
