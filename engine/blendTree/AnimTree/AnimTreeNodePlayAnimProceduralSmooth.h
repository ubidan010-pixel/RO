
#ifndef _ITF_ANIMTREENODEPLAYANIMPROCEDURALSMOOTH_H_
#define _ITF_ANIMTREENODEPLAYANIMPROCEDURALSMOOTH_H_

#ifndef _ITF_ANIMTREENODEPLAYANIM_H_
#include "engine/blendTree/animTree/AnimTreeNodePlayAnim.h"
#endif //_ITF_ANIMTREENODEPLAYANIM_H_

namespace ITF
{

class AnimTreeNodePlayAnimProceduralSmooth : public AnimTreeNodePlayAnim
{
    DECLARE_OBJECT_CHILD_RTTI(AnimTreeNodePlayAnimProceduralSmooth,AnimTreeNodePlayAnim,1871561061);

public:

    AnimTreeNodePlayAnimProceduralSmooth() : m_v(0.f), m_d(0.f) {}
    virtual ~AnimTreeNodePlayAnimProceduralSmooth() {}

    virtual void            init( BlendTreeInitDataBase* _initData, const BlendTreeNodeTemplate <AnimTreeResult>* _template );
    virtual void            onBecomeActive( const InputContainer& _inputs );

private:

    ITF_INLINE const class AnimTreeNodePlayAnimProceduralSmooth_Template* getTemplate() const;

    virtual void            setCursorPos( f32 _deltaTime, const InputContainer& _inputs );

    f32                     m_v;
    f32                     m_d;
};

/////////////////////////////////////////////////////////////////////////////////

class AnimTreeNodePlayAnimProceduralSmooth_Template : public AnimTreeNodePlayAnim_Template
{
    DECLARE_OBJECT_CHILD_RTTI(AnimTreeNodePlayAnimProceduralSmooth_Template,AnimTreeNodePlayAnim_Template,4008896927);
    DECLARE_SERIALIZE_VIRTUAL()
    DECLARE_ANIMTREENODE_TEMPLATE(AnimTreeNodePlayAnimProceduralSmooth)

public:

    AnimTreeNodePlayAnimProceduralSmooth_Template() : m_startCursor(-1.f), m_k(120.f), m_d(10.f)  {}
    ~AnimTreeNodePlayAnimProceduralSmooth_Template() {}

    f32                     getStartCursor() const { return m_startCursor; }
    f32                     getK() const { return m_k; }
    f32                     getD() const { return m_d; }

private:

    f32                     m_startCursor;
    f32                     m_k;
    f32                     m_d;
};

ITF_INLINE const AnimTreeNodePlayAnimProceduralSmooth_Template* AnimTreeNodePlayAnimProceduralSmooth::getTemplate() const { return static_cast<const AnimTreeNodePlayAnimProceduralSmooth_Template*>(m_template); }

}

#endif // _ITF_ANIMTREENODEPLAYANIMPROCEDURALSMOOTH_H_

