
#include "precompiled_engine.h"

#ifndef _ITF_MUSICTREE_H_
#include "engine/blendTree/MusicTree/MusicTree.h"
#endif //_ITF_MUSICTREE_H_


#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_




namespace ITF
{


/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_OBJECT_RTTI(MusicTree_Template)
BEGIN_SERIALIZATION_CHILD(MusicTree_Template)
SERIALIZE_MEMBER("metronomeType", m_metronomeType);
END_SERIALIZATION()




MusicTree_Template::MusicTree_Template()
: m_metronomeType( 0 )
{
    m_factory = ACTORSMANAGER->getMusicTreeNodeFactory();
}

}
