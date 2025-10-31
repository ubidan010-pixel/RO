#ifndef _ITF_DEFORMONTRAJECTORYCOMPONENT_H_
#define _ITF_DEFORMONTRAJECTORYCOMPONENT_H_

#ifndef _ITF_TRAJECTORYPROVIDER_H_
#include "gameplay/AI/Utils/TrajectoryProvider.h"
#endif //_ITF_TRAJECTORYPROVIDER_H_

namespace ITF
{
    class DeformOnTrajectoryComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(DeformOnTrajectoryComponent, ActorComponent,315423501);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        DeformOnTrajectoryComponent();
        virtual ~DeformOnTrajectoryComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        onResourceReady();
        virtual void        onBecomeActive();
        virtual void        Update( f32 _dt );
#ifdef ITF_SUPPORT_EDITOR
        virtual void        onEditorMove(bbool _modifyInitialPos = btrue);
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
        void                drawDebugBones() const;
        void                drawDebugBonesLocal() const;
#endif // ITF_SUPPORT_EDITOR

    private:
        ITF_INLINE const class DeformOnTrajectoryComponent_Template* getTemplate() const;

        struct BoneData 
        {
            DECLARE_SERIALIZE()

            BoneData()
                : m_boneIndex(U32_INVALID)
                , m_length(0.f)
            {}

            u32     m_boneIndex;
            f32     m_length;
        };

        typedef SafeArray<BoneData> BoneList;


        void                initBones();
        void                initTrajectory();
        void                placeBonesOnTrajectory();

        AnimLightComponent* m_animComponent;

        BoneList            m_bones;
        TrajectoryProvider  m_trajectory;
    };


    //---------------------------------------------------------------------------------------------------

    class DeformOnTrajectoryComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(DeformOnTrajectoryComponent_Template, TemplateActorComponent,3452429417);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(DeformOnTrajectoryComponent);

    public:

        DeformOnTrajectoryComponent_Template()
            : m_sampleLength(0.1f)
        {
        }
        virtual ~DeformOnTrajectoryComponent_Template() {}

        typedef SafeArray<StringID> BoneList;

        const BoneList& getBones() const { return m_bones; }
        f32             getSampleLength() const { return m_sampleLength; }

    private:

        BoneList    m_bones;
        f32         m_sampleLength;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const DeformOnTrajectoryComponent_Template* DeformOnTrajectoryComponent::getTemplate() const
    {
        return static_cast<const DeformOnTrajectoryComponent_Template*>(m_template);
    }

};

#endif //_ITF_DEFORMONTRAJECTORYCOMPONENT_H_