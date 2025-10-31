#include "precompiled_GFXAdapter_Dummy.h"

#include "GFXAdapter_Dummy.h"

namespace ITF
{

    GFXAdapter_Dummy::GFXAdapter_Dummy()
    {
        ITF_GFX_NOT_IMPLEMENTED();

        // Set a valid resolution to avoid 
        m_screenWidth = 1920;
        m_screenHeight = 1080;

        m_waitVBL = btrue;
    }

    GFXAdapter_Dummy::~GFXAdapter_Dummy()
    {
        ITF_GFX_NOT_IMPLEMENTED();
    }

} // namespace ITF
