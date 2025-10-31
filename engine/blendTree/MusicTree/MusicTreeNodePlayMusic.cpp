#include "precompiled_engine.h"


#ifndef _ITF_MUSICTREENODEPLAYMUSIC_H_
#include "engine/blendTree/MusicTree/MusicTreeNodePlayMusic.h"
#endif //_ITF_MUSICTREENODEPLAYMUSIC_H_



#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_



namespace ITF
{

//======================================================================================================
//======================================================================================================

IMPLEMENT_OBJECT_RTTI(MusicTreeNodePlayMusic)

void MusicTreeNodePlayMusic::onLoadResource( BlendTreeLoadResourceBase* _resData )
{
    Super::onLoadResource(_resData);

}

void MusicTreeNodePlayMusic::updateResult( f32 _deltaTime, const InputContainer& _inputs, BlendTreeResultBase* _result )
{
    Super::updateResult(_deltaTime,_inputs,_result);

    MusicTreeResultLeaf result;

    result.m_partName = getTemplate()->m_musicName;


    MusicTreeResult* musicTreeResult = static_cast<MusicTreeResult*>(_result);
    musicTreeResult->m_musicLeaf = result;
    musicTreeResult->m_isfinished = btrue;


}

void MusicTreeNodePlayMusic::onBecomeActive( const InputContainer& _inputs )
{
    Super::onBecomeActive(_inputs);
}


bbool MusicTreeNodePlayMusic::validate() const
{
    return btrue;
}

//======================================================================================================
//======================================================================================================

IMPLEMENT_OBJECT_RTTI(MusicTreeNodePlayMusic_Template)

BEGIN_SERIALIZATION_CHILD(MusicTreeNodePlayMusic_Template)

    SERIALIZE_MEMBER("musicName",m_musicName);

END_SERIALIZATION()

}



