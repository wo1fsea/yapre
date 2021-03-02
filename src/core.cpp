#include "core.h"

#define GLM_FORCE_RADIANS 1

#include <iostream>

#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include "camera.h"
#include "model.h"
#include "shader.h"

namespace yapre
{
    namespace core {
        // camera
        static Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
        static Shader *ourShader;
        static Model *ourModel;

        static const int SCREEN_FULLSCREEN = 0;
        static const int SCREEN_WIDTH = 800;
        static const int SCREEN_HEIGHT = 600;
        static const char *WINDOW_CAPTION = "yapre";

        static SDL_Window *window = NULL;
        static SDL_GLContext maincontext;

        static uint32_t current_ticks = 0;
        static uint32_t last_ticks = 0;

        void PrintSdlError() {
            auto error_message = SDL_GetError();
            std::cout << error_message << std::endl;
        }

        void setup_sdl_gl() {
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
        }

        void PrintSdlInfo() {
            std::cout << "OpenGL loaded" << std::endl;
            std::cout << "Vendor:" << glGetString(GL_VENDOR) << std::endl;
            std::cout << "Renderer:" << glGetString(GL_RENDERER) << std::endl;
            std::cout << "Version:" << glGetString(GL_VERSION) << std::endl;
        }

        bool Init() {
            // Initialize SDL
            if (SDL_Init(SDL_INIT_VIDEO) < 0) {
                PrintSdlError();
                return false;
            }
            atexit(SDL_Quit);

            setup_sdl_gl();

            // Create the window
            if (SCREEN_FULLSCREEN) {
                window = SDL_CreateWindow(
                        WINDOW_CAPTION, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0,
                        SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL);
            } else {
                window = SDL_CreateWindow(WINDOW_CAPTION, 
                        SDL_WINDOWPOS_UNDEFINED,
                        SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                        SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_ALWAYS_ON_TOP);
            }

            if (window == NULL) {
                PrintSdlError();
                return false;
            }

            maincontext = SDL_GL_CreateContext(window);
            if (maincontext == NULL) {
                PrintSdlError();
                return false;
            }

            gladLoadGLLoader(SDL_GL_GetProcAddress);

            // Check OpenGL properties
            PrintSdlInfo();

            // Use v-sync
            SDL_GL_SetSwapInterval(1);

            // tell stb_image.h to flip loaded texture's on the y-axis (before loading
            // model).
            stbi_set_flip_vertically_on_load(true);

            // configure global opengl state
            // -----------------------------
            glEnable(GL_DEPTH_TEST);

            int w, h;
            SDL_GL_GetDrawableSize(window, &w, &h);
            glViewport(0, 0, w, h);

            // build and compile shaders
            // -------------------------
            ourShader = new Shader("../data/shaders/model.vs", "../data/shaders/model.fs");

            // load models
            // -----------
            ourModel = new Model("../data/model/backpack/backpack.obj");

            return true;
        }

        void Deinit() {}

        void RenderFrame() {
            // render
            // ------
            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // don't forget to enable shader before setting uniforms
            ourShader->use();

            // view/projection transformations
            glm::mat4 projection =
                glm::perspective(glm::radians(camera.Zoom),
                        (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            ourShader->setMat4("projection", projection);
            ourShader->setMat4("view", view);

            // render the loaded model
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(
                    model,
                    glm::vec3(0.0f, 0.0f,
                        0.0f)); // translate it down so it's at the center of the scene
            model = glm::scale(
                    model,
                    glm::vec3(1.0f, 1.0f,
                        1.0f)); // it's a bit too big for our scene, so scale it down
            ourShader->setMat4("model", model);
            ourModel->Draw(*ourShader);

            SDL_GL_SwapWindow(window);
        }


        bool Update() {
            SDL_Event event;
            SDL_PollEvent(&event);

            current_ticks = SDL_GetTicks();
            double delta_ticks = 0;
            if (last_ticks != 0) {
                delta_ticks = current_ticks - last_ticks;
                delta_ticks /= 1000.;
            }

            last_ticks = current_ticks;

            if (event.type == SDL_KEYDOWN) {
                // Select surfaces based on key press
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        camera.ProcessKeyboard(Camera_Movement::FORWARD, delta_ticks);
                        break;

                    case SDLK_DOWN:
                        camera.ProcessKeyboard(Camera_Movement::BACKWARD, delta_ticks);
                        break;

                    case SDLK_LEFT:
                        camera.ProcessKeyboard(Camera_Movement::LEFT, delta_ticks);
                        break;

                    case SDLK_RIGHT:
                        camera.ProcessKeyboard(Camera_Movement::RIGHT, delta_ticks);
                        break;

                    default:
                        break;
                }
            }

            RenderFrame();
            return event.type != SDL_QUIT;
        }
    } // namespace pinhole
}
