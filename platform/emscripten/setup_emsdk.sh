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

        ./emsdk install latest
        ./emsdk activate latest
        
        cd ../..

    fi
}
