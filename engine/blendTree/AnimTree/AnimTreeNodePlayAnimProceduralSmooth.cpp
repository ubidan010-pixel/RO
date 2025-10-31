
#include "precompiled_engine.h"

#ifndef _ITF_ANIMTREENODEPLAYANIMPROCEDURALSMOOTH_H_
#include "engine/blendTree/animTree/AnimTreeNodePlayAnimProceduralSmooth.h"
#endif //_ITF_ANIMTREENODEPLAYANIMPROCEDURALSMOOTH_H_

namespace ITF
{

//======================================================================================================
//======================================================================================================

IMPLEMENT_OBJECT_RTTI(AnimTreeNodePlayAnimProceduralSmooth)

void AnimTreeNodePlayAnimProceduralSmooth::init( BlendTreeInitDataBase* _initData, const BlendTreeNodeTemplate <AnimTreeResult>* _template )
{
    Super::init(_initData,_template);

    ITF_ASSERT(getTemplate()->getProceduralCursor().isValid());
}

void AnimTreeNodePlayAnimProceduralSmooth::setCursorPos( f32 _deltaTime, const InputContainer& _inputs )
{
    f32 targetCursor = getTemplate()->getProceduralCursor().getValue(m_anim.calculateT(),_deltaTime,_inputs);

    if ( m_d < 0.f )
    {
        m_d = targetCursor;
    }
    else
    {
        f32 dif = targetCursor - m_d;
        f32 f = ( dif * getTemplate()->getK() ) - ( m_v * getTemplate()->getD() );
        
        m_v += f * _deltaTime;
        m_d += m_v * _deltaTime;

        m_d = Clamp(m_d,0.f,1.f);
    }

    m_anim.setCursor(m_d);
}

void AnimTreeNodePlayAnimProceduralSmooth::onBecomeActive( const InputContainer& _inputs )
{
    Super::onBecomeActive(_inputs);

    m_v = 0.f;
    m_d = getTemplate()->getStartCursor();
}

//======================================================================================================
//======================================================================================================

IMPLEMENT_OBJECT_RTTI(AnimTreeNodePlayAnimProceduralSmooth_Template)

BEGIN_SERIALIZATION_CHILD(AnimTreeNodePlayAnimProceduralSmooth_Template)

    SERIALIZE_MEMBER("startCursor",m_startCursor);
    SERIALIZE_MEMBER("stiffConstant",m_k);
    SERIALIZE_MEMBER("dampConstant",m_d);

END_SERIALIZATION()

}

