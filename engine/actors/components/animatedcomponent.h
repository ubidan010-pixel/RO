
#ifndef _ITF_ANIMATEDCOMPONENT_H_
#define _ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_STRINGID_H_
#include "core/StringID.h"
#endif //_ITF_STRINGID_H_

#ifndef _ITF_MATHTOOLS_H_
#include "core/math/MathTools.h"
#endif //_ITF_MATHTOOLS_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_ANIMTREE_H_
#include "engine/blendTree/animTree/AnimTree.h"
#endif //_ITF_ANIMTREE_H_

namespace ITF
{
    class AnimatedComponent : public AnimLightComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(AnimatedComponent,AnimLightComponent,1654726928);

    public:
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

        AnimatedComponent();
        ~AnimatedComponent();

        //brut de brut pour l'instant
        typedef ITF_VECTOR<Vertex> VertexContainer;


        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                onResourceLoaded();
        //virtual void                Draw();
        virtual void                onEvent( Event* _event );

        virtual bbool               isSynchronous() const { return Super::isSynchronous() || isLockMagicBox(); }
        virtual bool                updateData();
        virtual bool                postUpdateData();

        virtual void                setAnim(const StringID & _anim, u32 _blendFrames = U32_INVALID) { ITF_ASSERT(_blendFrames == U32_INVALID); m_action = m_lastAction = _anim; }
        ITF_INLINE bbool            isInTransition() const { return m_animTree.isInTransition(); }
        bbool                       isAnimNodeFinished() const { return ( m_animTree.isValid() ? m_animTree.isAnimFinished() : isSubAnimFinished() ); }
        bbool                       isAnimNodeLooped() const { return ( m_animTree.isValid() ? m_animTree.isAnimLooped() : isAnimLooped() ); }
        bbool                       isCurrentAnimNodePlaying() const { return !m_action.isValid(); }
        bbool                       isMainNodeFinished() const { return !isInTransition() && isAnimNodeFinished() && isCurrentAnimNodePlaying(); }
        u32                         getNumAnimTreeNodes() const { return m_animTree.getNumNodes(); }
        virtual bbool               isAnimToPlayFinished() const { return isMainNodeFinished(); }
#ifdef ITF_DEBUG_STRINGID
        const char*                 getAnimTreeNodeName( u32 _index) { return m_animTree.getNodeID(_index).getDebugString(); }
#endif
        const AnimTree*             getAnimTree() { return &m_animTree; }

        template <typename T>
        void                        setInput(StringID _inputName, T _value );

        template <typename T>
        void                        setInputFromIndex(u32 _index, T _value );

        bool                        getMagicBox(Vec3d & _pos, bbool _local = btrue);
        bbool                       getRoot( Vec3d& _pos ) const;

        void                        lockMagicBox( const Vec3d& _pos, f32 _interpolationTime = s_MagicBoxLockTimer );
        void                        resetLockMagicBoxFrom() { m_lockMagicBoxResetFrom = btrue; }
        const Vec3d&                getLockedMagicBoxPos() const { return m_lockMagicBoxPosTo; }
        void                        resetLockMagicBox() { m_lockMagicBox = bfalse; }
        bbool                       isLockMagicBox() const { return m_lockMagicBox; }
        bbool                       isLockMagicBoxInterpolating() const { return isLockMagicBox() && m_lockMagicBoxTimer != 0.f; }

        void                        setUpdateAnimInput( IUpdateAnimInputCriteria* _update ) { m_animInputUpdate = _update; }

        void                        dbgGetCurRule(String8 & _text);

        u32                         getNumInputs() const { return m_inputList.size(); }
        u32                         findInputIndex( StringID _id ) const;
#ifdef ITF_DEBUG_STRINGID
        const char*                 getInputName( u32 _index ) const { return m_inputList[_index].getName(); }
#endif
        InputType                   getInputType( u32 _index ) const { return m_inputList[_index].getType(); }

        template <typename T>
        void                        getInputValue( u32 _index, T& _val ) const
        {
            const Input& input = m_inputList[_index];
            input.getValue(_val);
        }

        virtual void                updateAnimTime( f32 _dt );
        virtual void                resetCurTime();
        void                        resetTree();
        void                        resetTransition();

        f32                         getPrevBlending() const;

        void                        setDisableInputUpdate( bbool _val ) { m_disableInputUpdate = _val; }

    protected:

        ITF_INLINE const class AnimatedComponent_Template* getTemplate() const;

        void                        clear();
        virtual void                setFirstAnim();
        virtual void                processAnim();

        void                        updateLockMagicBox();

        typedef SafeArray<StringID> AnimsPlayingContainer;
        void                        getResultSubAnimNames(const AnimTreeResult & _result, AnimsPlayingContainer& _anims);
  
        IUpdateAnimInputCriteria*   m_animInputUpdate;
        VertexContainer             m_vertexs;

        AnimTree                    m_animTree;

        StringID                    m_action;
        StringID                    m_lastAction;
        
        u32                         m_magicBoxIndex;
        Vec3d                       m_base;

        Vec3d                       m_prevMagicBox;
        bbool                       m_lockMagicBox;
        Vec3d                       m_lockMagicBoxPosFrom;
        Vec3d                       m_lockMagicBoxPosTo;
        f32                         m_lockMagicBoxTimer;
        f32                         m_lockMagicBoxTotalTime;
        bbool                       m_lockMagicBoxResetFrom;
        static const f32            s_MagicBoxLockTimer;

        SafeArray <Input>           m_inputList;
        bbool                       m_disableInputUpdate;
    };

    template <typename T>
    ITF_INLINE void AnimatedComponent::setInput(StringID _inputName, T _value )
    {
        u32 numInputs = m_inputList.size();

        for ( u32 i = 0; i < numInputs; i++ )
        {
            Input& input = m_inputList[i];

            if ( input.getId() == _inputName )
            {
                input.setValue(_value);
                break;
            }
        }
    }

    template <typename T>
    ITF_INLINE void AnimatedComponent::setInputFromIndex( u32 _index, T _value )
    {
        m_inputList[_index].setValue(_value);
    }

    class AnimatedComponent_Template : public AnimLightComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AnimatedComponent_Template,AnimLightComponent_Template,2655002388);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(AnimatedComponent);

    public:

        AnimatedComponent_Template();
        ~AnimatedComponent_Template();

        const SafeArray <InputDesc>&    getInputList() const { return m_inputList; }
        const AnimTree_Template&        getAnimTreeTemplate() const { return m_animTree; }

    private:

        SafeArray <InputDesc>       m_inputList;
        AnimTree_Template           m_animTree;
    };

    ITF_INLINE const class AnimatedComponent_Template* AnimatedComponent::getTemplate() const { return static_cast<const class AnimatedComponent_Template*>(m_template); }
}

#endif // _ITF_ANIMATEDCOMPONENT_H_

