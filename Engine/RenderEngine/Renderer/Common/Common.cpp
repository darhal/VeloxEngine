#include "Common.hpp"
#include <RenderEngine/Renderer/Backend/StateGroups/StateGroup.hpp>

TRE_NS_START

namespace RenderSettings
{
    StateGroup DEFAULT_STATE       = StateGroup();
    StateHash  DEFAULT_STATE_HASH  = 0;
    uint8      BLEND_DISTANCE_BITS = 23;
    FboID      DEFAULT_FRAMEBUFFER = 0;
}

TRE_NS_END