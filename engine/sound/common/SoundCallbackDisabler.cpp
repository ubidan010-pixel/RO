#include "precompiled_engine.h"
#include "engine/sound/common/SoundCallbackDisabler.h"
#include "engine/AdaptersInterfaces/SoundAdapter.h"

namespace ITF
{
	namespace sound
	{
		CallbackDisabler::CallbackDisabler()
		{
#if defined(ITF_SNOWDROP)
			SOUND_ADAPTER->disableAudioCallback();
#endif
		}

		CallbackDisabler::~CallbackDisabler()
		{
#if defined(ITF_SNOWDROP)
			SOUND_ADAPTER->enableAudioCallback();
#endif
		}
	} // namespace sound
} // namespace ITF