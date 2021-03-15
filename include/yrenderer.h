#pragma once
#include <string>
#include "glm/glm.hpp"

namespace yapre
{
    namespace renderer 
    {
        bool Init();
        void Deinit();
        void Update();
        void DrawSprite(
                std::string texture_filename,
                glm::vec3 position = glm::vec3(0.0f), 
                glm::vec2 size = glm::vec2(-1.0f),
                float rotate = 0.0f, 
                glm::vec3 color = glm::vec3(1.0f)
                ); 
    };
};

