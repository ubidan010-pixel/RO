#include "precompiled_gameplay.h"

#ifndef _ITF_DEFORMONTRAJECTORYCOMPONENT_H_
#include "gameplay/Components/Common/DeformOnTrajectoryComponent.h"
#endif //_ITF_DEFORMONTRAJECTORYCOMPONENT_H_

#ifndef _ITF_RENDERSIMPLEANIMCOMPONENT_H_
#include "gameplay/Components/Display/RenderSimpleAnimComponent.h"
#endif //_ITF_RENDERSIMPLEANIMCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(DeformOnTrajectoryComponent_Template)

    BEGIN_SERIALIZATION(DeformOnTrajectoryComponent_Template)
        SERIALIZE_CONTAINER("bones", m_bones);
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(DeformOnTrajectoryComponent)

    BEGIN_SERIALIZATION_CHILD(DeformOnTrajectoryComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(DeformOnTrajectoryComponent)
        VALIDATE_COMPONENT_PARAM("", m_animComponent, "DeformOnTrajectoryComponent requires an AnimLightComponent/AnimatedComponent");
        VALIDATE_COMPONENT_PARAM("", m_actor->GetComponent<RenderSimpleAnimComponent>(), "DeformOnTrajectoryComponent requires a RenderSimpleAnimComponent");
        for (u32 i = 0; i < m_bones.size(); i++)
        {
            VALIDATE_COMPONENT_PARAM("bones", m_bones[i].m_boneIndex != U32_INVALID, "bone %s is invalid", getTemplate()->getBones()[i].getDebugString());
        }
    END_VALIDATE_COMPONENT()

    DeformOnTrajectoryComponent::DeformOnTrajectoryComponent()
        : m_animComponent(NULL)
    {
    }

    DeformOnTrajectoryComponent::~DeformOnTrajectoryComponent()
    {
    }


    void DeformOnTrajectoryComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        m_animComponent = m_actor->GetComponent<AnimLightComponent>();  // mandatory, validated
    }

    void DeformOnTrajectoryComponent::onResourceReady()
    {
        Super::onResourceReady();

        if (!m_animComponent)
        {
            return;
        }

        initBones();
    }

    void DeformOnTrajectoryComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        initTrajectory();
    }

    void DeformOnTrajectoryComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        m_trajectory.update(m_actor->get2DPos());
        placeBonesOnTrajectory();
    }

    void DeformOnTrajectoryComponent::initBones()
    {
        m_bones.resize(getTemplate()->getBones().size());

        for (u32 i = 0; i < m_bones.size(); i++)
        {
            BoneData& boneData = m_bones[i];
            boneData.m_boneIndex = m_animComponent->getBoneIndex(getTemplate()->getBones()[i]);
            if (boneData.m_boneIndex != U32_INVALID)    // VALIDATED
            {
                AnimBoneDyn* bone = m_animComponent->getBone(boneData.m_boneIndex);
                boneData.m_length = (bone->m_PosEnd - bone->m_Pos).norm();
                ITF_ASSERT(!ITF_isnan(boneData.m_length));
            }
        }
    }

    void DeformOnTrajectoryComponent::initTrajectory()
    {
        f32 totalLength = 0;
        for (u32 i = 0; i < m_bones.size(); i++)
        {
            totalLength += m_bones[i].m_length;
        }

        m_trajectory.init(totalLength, getTemplate()->getSampleLength(),
            m_actor->get2DPos(), Vec2d::Up.Rotate(m_actor->getAngle()));
    }

    void DeformOnTrajectoryComponent::placeBonesOnTrajectory()
    {
        const Transform2d transform = getWorldTransform2d(m_actor);

        Vec2d p0(0,0);
        f32 a0 = 0;
        f32 dist = 0;

        for (u32 i = 0; i < m_bones.size(); i++)
        {
            BoneData& boneData = m_bones[i];
            AnimBoneDyn* bone = m_animComponent->getBone(boneData.m_boneIndex);

            dist += boneData.m_length;

            Vec2d p1 = m_trajectory.getPos(dist);   // world-space
#ifndef ITF_FINAL
            //DebugDraw::circle(p1, m_actor->getDepth(), 0.1f, Color::orange());
#endif // ITF_FINAL
            p1 = transform.inverseTransformPos(p1); // actor-space

            Vec2d boneVector = p1 - p0;
            const f32 boneLength = boneVector.norm();
            boneVector /= boneLength;

            const f32 a1 = boneVector.getAngle();   // actor-space
            bone->m_AngleLocal = a1 - a0;           // parent bone-space

            if (i > 0)
            {
                bone->m_PosLocal = Vec2d::Zero;
            }

            // the point on the trajectory may be closer than the length of the bone;
            // compute the real bone end here (we could also scale the bone down instead...)
            p1 = p0 + boneVector * boneData.m_length;


            p0 = p1;
            a0 = a1;
        }
    }



#ifdef ITF_SUPPORT_EDITOR
    void DeformOnTrajectoryComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);

        if (m_bones.size() == 0 ||
            !m_trajectory.isInitialized())
        {
            return;
        }

        m_trajectory.update(m_actor->get2DPos());
        placeBonesOnTrajectory();
        //m_animComponent->getAnimMeshScene()->m_AnimInfo.SetFrame();
    }

    void DeformOnTrajectoryComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
        {
            return;
        }

        //drawDebugBonesLocal();
        //drawDebugBones();
    }

    void DeformOnTrajectoryComponent::drawDebugBones() const
    {
        for (u32 i = 0; i < m_bones.size(); i++)
        {
            const BoneData& boneData = m_bones[i];
            AnimBoneDyn* bone = m_animComponent->getBone(boneData.m_boneIndex);

            String s;s.setTextFormat("%d", i);
            f32 duration = 0.f;
            DebugDraw::circle(bone->m_Pos, m_actor->getDepth(), 0.1f, (i%2) ? Color::green() : Color::white(), duration, s);
            DebugDraw::circle(bone->m_PosEnd, m_actor->getDepth(), 0.1f, (i%2) ? Color::red() : Color::black(), duration, s);
        }
    }

    void DeformOnTrajectoryComponent::drawDebugBonesLocal() const
    {
        const Transform2d transform = getWorldTransform2d(m_actor);

        Vec2d p0;
        f32 a0 = 0;

        for (u32 i = 0; i < m_bones.size(); i++)
        {
            const BoneData& boneData = m_bones[i];
            AnimBoneDyn* bone = m_animComponent->getBone(boneData.m_boneIndex);

            //Vec2d p1 = bone->m_PosLocal;
            Vec2d p1;
            p1 = transformPos(p0, a0, p1);
            p1 = transform.transformPos(p1);

            Vec2d p2 = bone->m_PosLocal + Vec2d(bone->GetScaledLength(), 0).Rotate(bone->m_AngleLocal);
            p2 = transformPos(p0, a0, p2);
            p2 = transform.transformPos(p2);

            String s;s.setTextFormat("%d", i);
            f32 duration = 0.f;
            DebugDraw::circle(p1, m_actor->getDepth(), 0.2f, (i%2) ? Color::green() : Color::white(), duration, s);
            DebugDraw::circle(p2, m_actor->getDepth(), 0.2f, (i%2) ? Color::red() : Color::black(), duration, s);

            p0 = p2;
            a0 = (p2 - p1).normalize().getAngle();
        }
    }

#endif // ITF_SUPPORT_EDITOR
};
