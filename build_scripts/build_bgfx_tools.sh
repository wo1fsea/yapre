if [ ! -d "./tools/bgfx" ]
then
    if [ ! -d "./tools" ]
    then
        mkdir ./tools
    fi
    
    mkdir -p ./tools/bgfx/build
    cd ./tools/bgfx/build
    
    cmake ../../../external/bgfx.cmake/ -DBGFX_BUILD_EXAMPLES=OFF -DBGFX_INSTALL=OFF -DBGFX_CUSTOM_TARGETS=OFF
    cmake --build .

    cp texturec texturev shaderc geometryc geometryv ../
    cd ..

    rm -rf ./build
    
    cd ../../..
fi
