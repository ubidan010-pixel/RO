#pragma once

#ifdef ITF_DUMMY_SOUND_ADAPTER

#include "engine/AdaptersInterfaces/SoundAdapter.h"

#ifdef ITF_DUMMY_ADAPTERS_ASSERT_ON_NOT_IMPLEMENTED
    #define ITF_SOUND_NOT_IMPLEMENTED() ITF_ASSERT_MSG(0, "Sound not implemented")
#else
    #define ITF_SOUND_NOT_IMPLEMENTED() do {} while(0)
#endif

namespace ITF
{
    class SoundAdapter_Dummy : public SoundAdapter
    {
    public:
        SoundAdapter_Dummy();
        ~SoundAdapter_Dummy() override;

        virtual bbool isRunning() const override;
        virtual bbool loadSound(Sound * _sound, const char * _path) override;
        virtual void unloadSound(Sound * _sound) override;
        virtual SourceHandle createSource(SourceInfo * _params) override;
        virtual void toggleDebug() override;
        virtual bbool isDebug() override;
        virtual f32 getSoundDuration(const Sound * _sound) override;
#ifndef ITF_FINAL
        virtual void getPerformanceDebugInfo(String& debugInfo, char paramDelimiter = '\n', char valueDelimiter = ':') override;
#endif
    };

} // namespace ITF

#endif // ITF_DUMMY_SOUND_ADAPTER
