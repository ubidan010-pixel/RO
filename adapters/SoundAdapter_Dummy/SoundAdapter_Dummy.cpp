#include "precompiled_SoundAdapter_Dummy.h"

#include "adapters/SoundAdapter_Dummy/SoundAdapter_Dummy.h"

#include "engine/sound/Sound.h"
#include "engine/resources/ResourceManager.h"

namespace ITF
{

    SoundAdapter_Dummy::SoundAdapter_Dummy()
    {
        ITF_SOUND_NOT_IMPLEMENTED();
    }


    SoundAdapter_Dummy::~SoundAdapter_Dummy()
    {
        ITF_SOUND_NOT_IMPLEMENTED();
    }

    bbool SoundAdapter_Dummy::isRunning() const
    {
        ITF_SOUND_NOT_IMPLEMENTED();
        return btrue;
    }

    bbool SoundAdapter_Dummy::loadSound(Sound* _sound, const char* _path)
    {
        ITF_SOUND_NOT_IMPLEMENTED();

        RESOURCE_MANAGER->addResourceAsLoaded(_sound);
        return btrue;
    }

    void SoundAdapter_Dummy::unloadSound(Sound* _sound)
    {
        ITF_SOUND_NOT_IMPLEMENTED();
    }

    SourceHandle SoundAdapter_Dummy::createSource(SourceInfo* _params)
    {
        ITF_SOUND_NOT_IMPLEMENTED();
        return InvalidSourceHandle;
    }

    void SoundAdapter_Dummy::toggleDebug()
    {
        ITF_SOUND_NOT_IMPLEMENTED();
    }

    bbool SoundAdapter_Dummy::isDebug()
    {
        ITF_SOUND_NOT_IMPLEMENTED();
        return bfalse;
    }

    f32 SoundAdapter_Dummy::getSoundDuration(const Sound* _sound)
    {
        ITF_SOUND_NOT_IMPLEMENTED();
        return 0.f;
    }

#ifndef ITF_FINAL
    void SoundAdapter_Dummy::getPerformanceDebugInfo(String& debugInfo, char paramDelimiter/* = '\n' */, char valueDelimiter /* = ':' */)
    {
        ITF_SOUND_NOT_IMPLEMENTED();
        debugInfo = "SoundAdapter_Dummy";
    }
#endif

} // namespace ITF
