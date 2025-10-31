#include "precompiled_engine.h"


#ifndef _ITF_MUSICPARTSET_H_
#include "engine/sound/Music/MusicPartSet.h"
#endif // _ITF_MUSICPARTSET_H_


namespace ITF
{

BEGIN_SERIALIZATION(MusicPartSet_Template)
    SERIALIZE_CONTAINER_OBJECT("parts",m_musicPartList);
END_SERIALIZATION()


MusicPartSet_Template::MusicPartSet_Template()
{
}

MusicPartSet_Template::~MusicPartSet_Template()
{
}



} // namespace ITF
