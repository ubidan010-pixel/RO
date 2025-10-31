#include "precompiled_engine.h"


#ifndef _ITF_MUSICPARTTEMPLATE_H_
#include "engine/sound/Music/MusicPartTemplate.h"
#endif // _ITF_MUSICPARTTEMPLATE_H_


namespace ITF
{

BEGIN_SERIALIZATION(MusicPart_Template)
    SERIALIZE_MEMBER("name",m_name);
    SERIALIZE_MEMBER("path",m_path); 
    SERIALIZE_MEMBER("nbMeasures",m_nbMeasures);
    SERIALIZE_MEMBER("beatsPerBar",m_beatsPerBar);
    SERIALIZE_MEMBER("prefetch",m_prefetch);
    SERIALIZE_MEMBER("WwiseEventGuid", m_wwiseEventGuid);
END_SERIALIZATION()


MusicPart_Template::MusicPart_Template()
    : m_nbMeasures( 0 )
    , m_beatsPerBar( 0 )
    , m_prefetch( bfalse )
{
}


MusicPart_Template::~MusicPart_Template()
{
}


} // namespace ITF
