
#ifndef _ITF_ANIMTREENODEPLAYANIMINTERPOLATECURSOR_H_
#define _ITF_ANIMTREENODEPLAYANIMINTERPOLATECURSOR_H_

#ifndef _ITF_ANIMTREENODEPLAYANIM_H_
#include "engine/blendTree/animTree/AnimTreeNodePlayAnim.h"
#endif //_ITF_ANIMTREENODEPLAYANIM_H_

namespace ITF
{

class AnimTreeNodePlayAnimInterpolateCursor : public AnimTreeNodePlayAnim
{
    DECLARE_OBJECT_CHILD_RTTI(AnimTreeNodePlayAnimInterpolateCursor,AnimTreeNodePlayAnim,521776981);

public:

    AnimTreeNodePlayAnimInterpolateCursor() : m_interpolateCounter(0.f) {}
    virtual ~AnimTreeNodePlayAnimInterpolateCursor() {}

    virtual void            init( BlendTreeInitDataBase* _initData, const BlendTreeNodeTemplate <AnimTreeResult>* _template );
    virtual void            onBecomeActive( const InputContainer& _inputs );

private:

    ITF_INLINE const class AnimTreeNodePlayAnimInterpolateCursor_Template* getTemplate() const;

    virtual void            setCursorPos( f32 _deltaTime, const InputContainer& _inputs );

    f32                     m_interpolateCounter;
};

/////////////////////////////////////////////////////////////////////////////////

class AnimTreeNodePlayAnimInterpolateCursor_Template : public AnimTreeNodePlayAnim_Template
{
    DECLARE_OBJECT_CHILD_RTTI(AnimTreeNodePlayAnimInterpolateCursor_Template,AnimTreeNodePlayAnim_Template,2482758920);
    DECLARE_SERIALIZE_VIRTUAL()
    DECLARE_ANIMTREENODE_TEMPLATE(AnimTreeNodePlayAnimInterpolateCursor)

public:

    AnimTreeNodePlayAnimInterpolateCursor_Template() : m_interpolateTime(0.2f), m_startCursor(0.5f)  {}
    ~AnimTreeNodePlayAnimInterpolateCursor_Template() {}

    f32                     getInterpolateTime() const { return m_interpolateTime; }
    f32                     getStartCursor() const { return m_startCursor; }

private:

    f32                     m_interpolateTime;
    f32                     m_startCursor;
};

ITF_INLINE const AnimTreeNodePlayAnimInterpolateCursor_Template* AnimTreeNodePlayAnimInterpolateCursor::getTemplate() const { return static_cast<const AnimTreeNodePlayAnimInterpolateCursor_Template*>(m_template); }

}

#endif // _ITF_ANIMTREENODEPLAYANIMINTERPOLATECURSOR_H_

