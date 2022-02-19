. ./build_scripts/build_bgfx_tools.sh

compile_shader()
{
    if [ ! -d "./data/bgfx_shader/$3" ]
    then
        mkdir -p "./data/bgfx_shader/$3"
    fi
    
    ./tools/bgfx/shaderc \
    -f ./data/bgfx_shader/$1.sc -o ./data/bgfx_shader/$3/$1.bin  \
    --platform $3 --type vertex --verbose -i ./ -p $4
    
    ./tools/bgfx/shaderc \
    -f ./data/bgfx_shader/$2.sc -o ./data/bgfx_shader/$3/$2.bin  \
    --platform $3 --type fragment --verbose -i ./ -p $4
}

compile_shader v_simple f_simple osx metal
compile_shader v_simple f_simple ios metal
compile_shader v_simple f_simple asmjs 300_es
compile_shader v_simple f_simple android 300_es
compile_shader v_simple f_simple windows 150