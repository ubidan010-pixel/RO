#ifndef _ITF_MUSICPARTSET_H_
#define _ITF_MUSICPARTSET_H_

#ifndef SERIALIZEROBJECT_H
#include "core/serializer/ZSerializerObject.h"
#endif // SERIALIZEROBJECT_H

#ifndef _ITF_MUSICPARTTEMPLATE_H_
#include "engine/sound/Music/MusicPartTemplate.h"
#endif // _ITF_MUSICPARTTEMPLATE_H_

namespace ITF
{

    class MusicPartSet_Template
    {
    public:

        DECLARE_SERIALIZE()

    public:

        MusicPartSet_Template();
        ~MusicPartSet_Template();

        const ITF_VECTOR<MusicPart_Template>&   getMusicPartList() const { return m_musicPartList; }

    private:

        ITF_VECTOR<MusicPart_Template>  m_musicPartList;
    };


}

#endif //_ITF_MUSICPARTSET_H_
