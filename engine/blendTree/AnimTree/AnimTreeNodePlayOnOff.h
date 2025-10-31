
#ifndef _ITF_ANIMTREENODEPLAYONOFF_H_
#define _ITF_ANIMTREENODEPLAYONOFF_H_

#ifndef _ITF_ANIMTREENODEPLAYANIM_H_
#include "engine/blendTree/animTree/AnimTreeNodePlayAnim.h"
#endif //_ITF_ANIMTREENODEPLAYANIM_H_

namespace ITF
{

class AnimTreeNodePlayOnOff : public BlendTreeNode <AnimTreeResult>
{
    DECLARE_OBJECT_CHILD_RTTI(AnimTreeNodePlayOnOff,BlendTreeNode<AnimTreeResult>,795711399);

    enum EStateOnOff : i32
    {
        STATEONOFF_ON,
        STATEONOFF_OFF,
        STATEONOFF_TOON,
        STATEONOFF_TOOFF,
    };

public:

    AnimTreeNodePlayOnOff();
    virtual ~AnimTreeNodePlayOnOff() { clear(); }

    virtual void                    init( BlendTreeInitDataBase* _initData, const BlendTreeNodeTemplate <AnimTreeResult>* _template );
    virtual void                    onBecomeActive( const InputContainer& _inputs );
    virtual void                    onLoadResource( BlendTreeLoadResourceBase* _resData );
    virtual void                    getPlayingNodes( const InputContainer & _inputs, NodeList& _playingNodes );
    virtual void                    updateResult( f32 _deltaTime, const InputContainer & _inputs, BlendTreeResultBase* _result );

private:

    ITF_INLINE const class AnimTreeNodePlayOnOff_Template* getTemplate() const;

    void                            clear();
    EStateOnOff                     getTargetState() const;
    void                            setActiveAnim( const InputContainer& _inputs );
    void                            updateLocalResult( f32 _deltaTime, const InputContainer & _inputs, AnimTreeResult& localResult );

    BlendTreeNode<AnimTreeResult>*  m_animOn;
    BlendTreeNode<AnimTreeResult>*  m_animOff;
    AnimTreeNodePlayAnim            m_transOn;
    AnimTreeNodePlayAnim            m_transOff;
    CriteriaArray                   m_criteriaOn;
    EStateOnOff                     m_state;
};

/////////////////////////////////////////////////////////////////////////////////

class AnimTreeNodePlayOnOff_Template : public BlendTreeNodeTemplate <AnimTreeResult>
{
    DECLARE_OBJECT_CHILD_RTTI(AnimTreeNodePlayOnOff_Template,BlendTreeNodeTemplate<AnimTreeResult>,984447049);
    DECLARE_SERIALIZE_VIRTUAL()
    DECLARE_ANIMTREENODE_TEMPLATE(AnimTreeNodePlayOnOff)

public:

    AnimTreeNodePlayOnOff_Template();
    ~AnimTreeNodePlayOnOff_Template();

    const BlendTreeNodeTemplate<AnimTreeResult>*    getAnimOn() const { return m_animOn; }
    const BlendTreeNodeTemplate<AnimTreeResult>*    getAnimOff() const { return m_animOff; }
    const AnimTreeNodePlayAnim_Template&            getTransOn() const { return m_transOn; }
    const AnimTreeNodePlayAnim_Template&            getTransOff() const { return m_transOff; }
    const CriteriaDescArray&                        getCriteriaDesc() const { return m_criteriaOnDesc; }

private:

    BlendTreeNodeTemplate<AnimTreeResult>*      m_animOn;
    BlendTreeNodeTemplate<AnimTreeResult>*      m_animOff;
    AnimTreeNodePlayAnim_Template               m_transOn;
    AnimTreeNodePlayAnim_Template               m_transOff;
    CriteriaDescArray                           m_criteriaOnDesc;
};

ITF_INLINE const AnimTreeNodePlayOnOff_Template* AnimTreeNodePlayOnOff::getTemplate() const { return static_cast<const AnimTreeNodePlayOnOff_Template*>(m_template); }

}

#endif // _ITF_ANIMTREENODEPLAYONOFF_H_

