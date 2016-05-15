#pragma once

#include <stdint.h>
enum ResourceType : uint16_t {
    Mesh_T,
    Texture_T,
    Shader_T,
    DrawState_T,
    InvalidResourceType = 0xFFFF,
};

