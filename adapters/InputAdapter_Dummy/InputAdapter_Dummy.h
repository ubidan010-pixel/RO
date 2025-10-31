#pragma once

#ifdef ITF_DUMMY_INPUTADAPTER

#include "engine/AdaptersInterfaces/InputAdapter.h"

#ifdef ITF_DUMMY_ADAPTERS_ASSERT_ON_NOT_IMPLEMENTED
    #define ITF_INPUT_NOT_IMPLEMENTED() ITF_ASSERT_MSG(0, "Input not implemented")
#else
    #define ITF_INPUT_NOT_IMPLEMENTED() do {} while(0)
#endif

namespace ITF
{

    class InputAdapter_Dummy : public InputAdapter
    {
    public:
        InputAdapter_Dummy();
        ~InputAdapter_Dummy();
    };

} // namespace ITF

#endif // ITF_DUMMY
