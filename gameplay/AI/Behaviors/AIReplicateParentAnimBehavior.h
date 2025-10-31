#ifndef _ITF_AIREPLICATEPARENTANIMBEHAVIOR_H_
#define _ITF_AIREPLICATEPARENTANIMBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/AI/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    //***************************************************************************
    // behavior which check parent (bind/link) animation and play the animation 
    // with the same name on me
    //***************************************************************************
    class AIReplicateParentAnimBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(AIReplicateParentAnimBehavior, AIBehavior,137688680);
        //DECLARE_VALIDATE_BEHAVIOR()

    public:

        AIReplicateParentAnimBehavior();
        virtual ~AIReplicateParentAnimBehavior();

        virtual void        update( f32 _dt  );
        virtual void        onActivate();
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

    protected:

        StringID            getParentAnimation( Actor * & _res_parent ) const;
        void                replicateParentAnimation();
        
        StringID            m_currentAnim;
        ObjectRef           m_parentLink;
        
    private:
        ITF_INLINE const class AIReplicateParentAnimBehavior_Template * getTemplate() const;
    };

    //***************************************************************************
    class AIReplicateParentAnimBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(AIReplicateParentAnimBehavior_Template,TemplateAIBehavior,1147033395);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(AIReplicateParentAnimBehavior);

    public:

        AIReplicateParentAnimBehavior_Template();
        ~AIReplicateParentAnimBehavior_Template();

        ITF_INLINE bbool                        useParentBind()     const { return m_useParentBind; }
        ITF_INLINE const StringID &             getDefaultAnim()    const { return m_defaultAnim; }
        bbool                                   isAnimReplicable( const StringID & _anim ) const;
        
    private:
        bbool                   m_useParentBind;
        StringID                m_defaultAnim;
        SafeArray<StringID>     m_animsToAvoid;
    };

    ITF_INLINE const class AIReplicateParentAnimBehavior_Template * AIReplicateParentAnimBehavior::getTemplate() const
    {
        return static_cast<const class AIReplicateParentAnimBehavior_Template *>(m_template);
    }

}

#endif // _ITF_AIREPLICATEPARENTANIMBEHAVIOR_H_