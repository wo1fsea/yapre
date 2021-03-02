setup_emsdk()
{
    if [ ! -d "./tools/emsdk" ]
    then
        if [ ! -d "./tools" ]
        then
            mkdir ./tools
        fi

        pushd ./tools        
        git clone https://github.com/emscripten-core/emsdk.git
        
        pushd ./emsdk
        git pull

        ./emsdk install latest
        ./emsdk activate latest
        
        popd
        popd

    fi
}
