#include "precompiled_inputadapter_Nintendo.h"

#include "adapters/InputAdapter_Nintendo/InputAdapter_Nintendo.h"

#ifdef INPUT_USE_VKEYBOARD

#include "adapters/InputAdapter_Nintendo/Task_VKeyboard_Nintendo.h"
#include "core/math/MathTools.h"

#include <nn/swkbd/swkbd_Api.h>
#include <nn/swkbd/swkbd_Result.h>

namespace ITF
{
    
    Task_VKeyboard_NX::Task_VKeyboard_NX(const InputAdapter::VKeyboardOpenParams& _params, const String8& guideText)
	// ITF_PASTA TODO RO2 Task class doesn't exist
    //: Task("VirtualKeyboard", Task::ERunThread_Dedicated), m_params(_params), m_text(guideText)
    {
    }

    Task_VKeyboard_NX::~Task_VKeyboard_NX()
    {
    }

	// ITF_PASTA TODO : in RO2 tasks are no objects
	void Task_VKeyboard_NX::run()
//	Task::EResult Task_VKeyboard_NX::run()
	{
        // initialize system virtual keyboard

        nn::swkbd::ShowKeyboardArg showKeyboardArg;

        if (m_params.m_pincode)
        {
            nn::swkbd::MakePreset(&showKeyboardArg.keyboardConfig, nn::swkbd::Preset_Password);
            showKeyboardArg.keyboardConfig.keyboardMode = nn::swkbd::KeyboardMode_Numeric;
        }
        else
        {
            nn::swkbd::MakePreset(&showKeyboardArg.keyboardConfig, nn::swkbd::Preset_Default);

            showKeyboardArg.keyboardConfig.isUseNewLine = m_params.m_multiline;

            showKeyboardArg.keyboardConfig.inputFormMode = m_params.m_multiline ? nn::swkbd::InputFormMode::InputFormMode_MultiLine : nn::swkbd::InputFormMode::InputFormMode_OneLine;

            showKeyboardArg.keyboardConfig.passwordMode = m_params.m_password ? nn::swkbd::PasswordMode::PasswordMode_Hide : nn::swkbd::PasswordMode::PasswordMode_Show;

            switch (m_params.m_style)
            {
            case InputAdapter::VK_style_num:
                showKeyboardArg.keyboardConfig.keyboardMode = nn::swkbd::KeyboardMode_Numeric;
                break;

            default:
                break;
            }
        }

        showKeyboardArg.keyboardConfig.textMaxLength = Min(m_params.m_textMaxSize, u32(nn::swkbd::TextMaxLength));

        const size_t workBufferSize = nn::swkbd::GetRequiredWorkBufferSize(false);
        void* workBuffer = Memory::alignedMallocCategory(workBufferSize, nn::os::MemoryPageSize, MemoryId::mId_System);

        showKeyboardArg.workBuf = workBuffer;
        showKeyboardArg.workBufSize = workBufferSize;

        const size_t resultBufferSize = nn::swkbd::GetRequiredStringBufferSize();
        nn::swkbd::String resultBuffer;
        resultBuffer.ptr = reinterpret_cast<char16_t*>(Memory::alignedMallocCategory(resultBufferSize, nn::os::MemoryPageSize, MemoryId::mId_System));
        resultBuffer.bufSize = resultBufferSize;

        if (!m_text.isEmpty())
            nn::swkbd::SetGuideTextUtf8(&showKeyboardArg.keyboardConfig, m_text.cStr());

        // run system UI 
        nn::Result result = nn::swkbd::ShowKeyboard(&resultBuffer, showKeyboardArg);

        // set output string to obtained value, if it exists
        const bbool cancelled = nn::swkbd::ResultCanceled::Includes(result);
        if (!cancelled)
        {
            m_text = reinterpret_cast<wchar_t*>(resultBuffer.ptr);
        }

        // free temporary buffers

        Memory::alignedFree(resultBuffer.ptr);
        Memory::alignedFree(workBuffer);

		// ITF_PASTA TODO : in RO2 tasks are no objects
		//return cancelled ? EResult_Failure : EResult_Success;
    }

}
#endif // INPUT_USE_VKEYBOARD
