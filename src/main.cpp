#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <iostream>
#include <SDL.h>
#include <SDL_main.h>
#include <glad/glad.h>

#include "core.h"
#include "audio.h"
#include "renderer.h"

static const int SCREEN_FULLSCREEN = 0;
static const int SCREEN_WIDTH = 800;
static const int SCREEN_HEIGHT = 600;
static const char *WINDOW_CAPTION = "yapre";

static SDL_Window *window = NULL;
static SDL_GLContext maincontext;



void PrintSdlError() {
    auto error_message = SDL_GetError();
    std::cout << error_message << std::endl;
}

void PrintSdlInfo() {
    std::cout << "OpenGL loaded" << std::endl;
    std::cout << "Vendor:" << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer:" << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version:" << glGetString(GL_VERSION) << std::endl;
}

void SetupSdlGl() {
    /*
    // Request an OpenGL 4.5 context (should be core)
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Also request a depth buffer
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Default OpenGL is fine.
    SDL_GL_LoadLibrary(NULL);
    */
}


int main(int argc, char *args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        PrintSdlError();
        return 0;
    }
    atexit(SDL_Quit);

    SetupSdlGl();
    
    if (SCREEN_FULLSCREEN) {
        window = SDL_CreateWindow(
                WINDOW_CAPTION, 
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                0, 0,
                SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
    } else {
        window = SDL_CreateWindow(
                WINDOW_CAPTION, 
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                SCREEN_WIDTH,SCREEN_HEIGHT,
                SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
                // SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
    }

    if (window == NULL) {
        PrintSdlError();
        return 0;
    }

    maincontext = SDL_GL_CreateContext(window);
    if (maincontext == NULL) {
        PrintSdlError();
        return 0;
    }

    gladLoadGLLoader(SDL_GL_GetProcAddress);

    PrintSdlInfo();

    SDL_GL_SetSwapInterval(1);
    
    glEnable(GL_DEPTH_TEST);

    int w, h;
    // SDL_GL_GetDrawableSize(window, &w, &h);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    

    if(! yapre::renderer::Init())
    {
        return 0;
    }
   
    if(! yapre::audio::Init())
    {
        return 0;
    }

    if(! yapre::core::Init())
    {
        return 0;
    }
    
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(
            [](){
                yapre::core::Update();
                yapre::renderer::RenderFrame();
                
                SDL_GL_SwapWindow(window);
            },
            0, 1);
#else
    while (yapre::core::Update()) {
        yapre::renderer::RenderFrame();
        SDL_GL_SwapWindow(window);
    } 
#endif

    yapre::core::Deinit();
    yapre::renderer::Deinit();
    // yapre::audio::Deinit();
    return 0;
}

