#ifndef _ITF_MUSICPARTTEMPLATE_H_
#define _ITF_MUSICPARTTEMPLATE_H_

#ifndef _ITF_SOUND_ADAPTER_
#include "engine/AdaptersInterfaces/SoundAdapter.h"
#endif //_ITF_SOUND_ADAPTER_

#ifndef SERIALIZEROBJECT_H
#include "core/serializer/ZSerializerObject.h"
#endif // SERIALIZEROBJECT_H



namespace ITF
{

    class MusicPart_Template
    {
    public:

        DECLARE_SERIALIZE()

        MusicPart_Template();
        ~MusicPart_Template();

        ITF_INLINE const StringID&      getName() const                             { return m_name; }
        ITF_INLINE const Path&          getPath() const                             { return m_path; }
        ITF_INLINE const u32            getNbMeasures() const                       { return m_nbMeasures; }
        ITF_INLINE const u32            getBeatsPerBar() const                      { return m_beatsPerBar; }
        ITF_INLINE const bbool          getPrefetch() const                         { return m_prefetch; }
        const StringID& getWwiseEventGuid() const { return m_wwiseEventGuid; }


    private:

        StringID                        m_name;
        Path                            m_path;
        u32                             m_nbMeasures;
        u32                             m_beatsPerBar;
        bbool                           m_prefetch;
        StringID m_wwiseEventGuid;
    };



}

#endif //_ITF_MUSICPARTTEMPLATE_H_
