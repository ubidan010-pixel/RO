
#include "precompiled_engine.h"

#ifndef _ITF_ANIMTREENODEPLAYANIMINTERPOLATECURSOR_H_
#include "engine/blendTree/animTree/AnimTreeNodePlayAnimInterpolateCursor.h"
#endif //_ITF_ANIMTREENODEPLAYANIMINTERPOLATECURSOR_H_

namespace ITF
{

//======================================================================================================
//======================================================================================================

IMPLEMENT_OBJECT_RTTI(AnimTreeNodePlayAnimInterpolateCursor)

void AnimTreeNodePlayAnimInterpolateCursor::init( BlendTreeInitDataBase* _initData, const BlendTreeNodeTemplate <AnimTreeResult>* _template )
{
    Super::init(_initData,_template);

    ITF_ASSERT(getTemplate()->getInterpolateTime());
    ITF_ASSERT(getTemplate()->getProceduralCursor().isValid());
}

void AnimTreeNodePlayAnimInterpolateCursor::setCursorPos( f32 _deltaTime, const InputContainer& _inputs )
{
    f32 targetCursor = getTemplate()->getProceduralCursor().getValue(m_anim.calculateT(),_deltaTime,_inputs);
    f32 startCursor = getTemplate()->getStartCursor();
    f32 interpolateTime = getTemplate()->getInterpolateTime();
    f32 cursor;

    if ( interpolateTime )
    {
        m_interpolateCounter = Min(m_interpolateCounter+_deltaTime,interpolateTime);

        cursor = Interpolate(startCursor,targetCursor,m_interpolateCounter/interpolateTime);
    }
    else
    {
        cursor = targetCursor;
    }

    if ( m_anim.getIsLooped() )
    {
        cursor = f32_Modulo(cursor,1.f);
    }
    else
    {
        cursor = Clamp(cursor,0.f,1.f);
    }

    m_anim.setCursor(cursor);
}

void AnimTreeNodePlayAnimInterpolateCursor::onBecomeActive( const InputContainer& _inputs )
{
    Super::onBecomeActive(_inputs);

    m_interpolateCounter = 0.f;
}

//======================================================================================================
//======================================================================================================

IMPLEMENT_OBJECT_RTTI(AnimTreeNodePlayAnimInterpolateCursor_Template)

BEGIN_SERIALIZATION_CHILD(AnimTreeNodePlayAnimInterpolateCursor_Template)

    SERIALIZE_MEMBER("interpolationTime",m_interpolateTime);
    SERIALIZE_MEMBER("startCursor",m_startCursor);

END_SERIALIZATION()

}



