#include "precompiled_gameplay.h"

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_CHARACTERDBUGGERCOMPONENT_H_
#include "gameplay/components/Common/CharacterDebuggerComponent.h"
#endif //_ITF_CHARACTERDBUGGERCOMPONENT_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_


#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(CharacterDebuggerComponent)

BEGIN_SERIALIZATION_CHILD(CharacterDebuggerComponent)
END_SERIALIZATION()

const f32 CharacterDebuggerComponent::s_trajectoryPointSize = 0.05f;
const f32 CharacterDebuggerComponent::s_contactsPointSize = 0.03f;
const f32 CharacterDebuggerComponent::s_minForceSize = 0.f;
const f32 CharacterDebuggerComponent::s_maxForceSize = 0.5f;
const f32 CharacterDebuggerComponent::s_maxForce = 50.f;

CharacterDebuggerComponent::CharacterDebuggerComponent()
: Super()
, m_stickedPolyline(ITF_INVALID_OBJREF)
, m_stickedEdge(U32_INVALID)
, m_currentForce(Vec2d::Zero)
, m_currentGroundDir(Vec2d::Zero)
, m_currentSpeed(Vec2d::Zero)
, m_physPos(Vec3d::Zero)
, m_physPrevPos(Vec3d::Zero)
, m_speedAverage(Vec2d::Zero)
, m_torqueAverage(0.f)
, m_currentFrictionCoef(0.f)
, m_radius(0.f)
, m_active(bfalse)
{
}

CharacterDebuggerComponent::~CharacterDebuggerComponent()
{
}

void CharacterDebuggerComponent::Update( f32 _deltaTime )
{
    StickToPolylinePhysComponent* physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();

    if ( physComponent )
    {
        m_radius = physComponent->getRadius();
        m_currentForce = physComponent->getForce();
        m_currentSpeed = physComponent->getSpeed();
        m_speedAverage = physComponent->getWorldSpeedAverage();
        m_torqueAverage = physComponent->getTorqueAverage();
        m_currentGravityDir = physComponent->getCurrentGravityDir();
        m_currentFrictionCoef = physComponent->getFrictionCoef();
        m_stickedPolyline = physComponent->getStickedPolylineRef();
        m_stickedEdge = physComponent->getStickedEdgeIndex();

        if ( physComponent->getStickedEdge() )
        {
            m_currentGroundDir = physComponent->getCurrentGroundDir();
        }
        else
        {
            m_currentGroundDir = Vec2d::Zero;
        }

        m_physPos = m_actor->getPos();
        m_physPrevPos = physComponent->getPreviousPos().to3d(m_physPos.m_z);

        m_contacts.clear();

        const PhysContactsContainer& bodyContacts = physComponent->getContacts();

        for ( u32 i = 0; i < bodyContacts.size(); i++ )
        {
            m_contacts.push_back(bodyContacts[i].m_contactPoint);
        }

        m_stickyRanges.clear();

        if ( CHEATMANAGER->isResetCharacterDebugTrajectories() )
        {
            m_trajectory.clear();
        }

        const Vec3d& currentPos = m_actor->getPos();
        bbool samePos;

        if ( m_trajectory.empty() )
        {
            samePos = bfalse;
        }
        else
        {
            const Vec3d& lastP = m_trajectory.back().m_pos;

            samePos = currentPos.IsEqual(lastP,MTH_EPSILON);
        }

        if ( !samePos )
        {
            STrajectoryPoint newPoint;

            newPoint.m_sticked = physComponent->getStickedEdge() != NULL;
            newPoint.m_axis = Vec2d::Right.Rotate(m_actor->getAngle()+MTH_PIBY2);
            newPoint.m_pos = currentPos;

            m_trajectory.push_back(newPoint);

            if ( m_trajectory.size() >getTemplate()->getMaxTrajectory() )
            {
                m_trajectory.erase(m_trajectory.begin());
            }
        }
    }

    if ( m_active )
    {
        AABB aabb(m_actor->getPos());

        for ( TrajectoryList::const_iterator it = m_trajectory.begin(); it != m_trajectory.end(); ++it )
        {
            const STrajectoryPoint& traj = *it;

            aabb.grow(traj.m_pos);
        }

        m_actor->growRelativeAABBFromAbsolute(aabb);
    }

    m_active = CHEATMANAGER->getDebugCharacters();
}

#ifdef ITF_SUPPORT_EDITOR
void CharacterDebuggerComponent::editor_draw()
{
    if ( !m_active )
    {
        return;
    }


    if (getTemplate()->getShowCollider())
    {
        GFX_ADAPTER->drawDBGCircle(m_physPos, m_radius, 0.f, 1.f, 0.f);
        GFX_ADAPTER->drawDBGCircle(m_physPrevPos, m_radius, 1.f,0.f,0.f);
    }


    f32 stickyColors[2][3];
    u32 stickyIndex = 0;

    stickyColors[0][0] = 0.f;
    stickyColors[0][1] = 1.f;
    stickyColors[0][2] = 0.f;
    stickyColors[1][0] = 1.f;
    stickyColors[1][1] = 0.f;
    stickyColors[1][2] = 0.f;

    for ( u32 i = 0; i < m_stickyRanges.size(); i++ )
    {
        Vec2d pos = m_stickyRanges[i].ToVector();

        GFX_ADAPTER->drawDBG3DLine(m_physPos, m_physPos + (pos.to3d()*m_radius),
            stickyColors[stickyIndex][0],
            stickyColors[stickyIndex][1],
            stickyColors[stickyIndex][2],
            0.f);

        stickyIndex = !stickyIndex;
    }

    static const f32 trajectoryStickedColor[3] = { 0.f, 1.f, 0.f };
    static const f32 trajectoryUnstickedColor[3] = { 1.f, 0.f, 0.f };

    if (getTemplate()->getShowTrajectory())
    {
        Vec3d feet;

        for ( TrajectoryList::const_iterator it = m_trajectory.begin(); it != m_trajectory.end(); ++it )
        {
            const STrajectoryPoint& point = *it;
            const f32* color = point.m_sticked ? trajectoryStickedColor : trajectoryUnstickedColor;
            GFX_ADAPTER->drawDBGCircle(point.m_pos,s_trajectoryPointSize,color[0],color[1],color[2]);
            feet = point.m_pos - (point.m_axis*m_radius).to3d();
            GFX_ADAPTER->drawDBGCircle(feet,s_trajectoryPointSize,color[0],color[1],color[2]);

            GFX_ADAPTER->drawDBG3DLine(point.m_pos, point.m_pos + (point.m_axis.to3d()*m_radius),
                color[0],color[1],color[2],0.f);
        }

        for ( PointsList::const_iterator it = m_contacts.begin(); it != m_contacts.end(); ++it )
        {
            GFX_ADAPTER->drawDBGCircle(it->m_x,it->m_y,s_contactsPointSize,1.f,1.f,0.f);
        }

        if ( m_currentForce != Vec2d::Zero )
        {
            Vec2d forceDir = m_currentForce;
            f32 forceValue = forceDir.norm();

            forceDir.normalize();
            forceValue = Clamp(forceValue,0.f,s_maxForce);

            f32 forceSize = Interpolate(s_minForceSize,s_maxForceSize,forceValue/s_maxForce);

            GFX_ADAPTER->drawDBG3DLine(m_actor->getPos(), m_actor->getPos() + (forceDir.to3d()*forceSize),
                1.f,1.f,1.f,0.f);
        }

        if ( m_currentGroundDir != Vec2d::Zero )
        {
            Vec2d HorizAxis;
            f32_CosSin(m_actor->getAngle(), &HorizAxis.m_x, &HorizAxis.m_y);
            Vec2d vertAxis = HorizAxis.getPerpendicular();
            Vec2d contact = Vec2d(m_actor->getPos().m_x, m_actor->getPos().m_y);

            contact -= vertAxis * m_radius;

            Vec2d pA = contact - (m_currentGroundDir*0.5f);
            Vec2d pB = pA + m_currentGroundDir;

            GFX_ADAPTER->drawDBGLine(pA, pB, 1.f,1.f,1.f,0.f);
        }
    }

    Vec2d actorDir = Vec2d::Right.Rotate(m_actor->getAngle());
    Vec2d feetPos = m_actor->get2DPos() - ( actorDir.getPerpendicular() * m_radius );
    GFX_ADAPTER->drawDBGCircle(feetPos.m_x,feetPos.m_y,s_contactsPointSize,1.f,1.f,1.f);


#define MAX_BUFF 256
    char buf[MAX_BUFF]; 
#ifndef ITF_CTR //CASA::TODO:: error de compmilation
    if (getTemplate()->getDebugController())
    {
        SPRINTF_S_TRUNC(buf,MAX_BUFF,"Speed: %f %f",m_currentSpeed.m_x,m_currentSpeed.m_y);
        GFX_ADAPTER->drawDBGText(buf);

        SPRINTF_S_TRUNC(buf,MAX_BUFF,"Speed Value: %f",m_currentSpeed.norm());
        GFX_ADAPTER->drawDBGText(buf);

        SPRINTF_S_TRUNC(buf,MAX_BUFF,"Friction Coef.: %f",m_currentFrictionCoef);
        GFX_ADAPTER->drawDBGText(buf);

        SPRINTF_S_TRUNC(buf,MAX_BUFF,"Speed Average: %f %f",m_speedAverage.m_x,m_speedAverage.m_y);
        GFX_ADAPTER->drawDBGText(buf);

        SPRINTF_S_TRUNC(buf,MAX_BUFF,"Torque Average: %f",m_torqueAverage);
        GFX_ADAPTER->drawDBGText(buf);
    }
#endif

#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID)
    if (getTemplate()->getDebugAnim())
    {
        AnimLightComponent* animLightComponent = m_actor->GetComponent<AnimLightComponent>();

        if ( animLightComponent )
        {
            String8 spaces = "               ";

            u32 numAnims = animLightComponent->getNumAnimsPlaying();
            if ( numAnims )
            {
                for ( u32 i = 0; i < numAnims; i++ )
                {
                    String8 ruleText;
                    animLightComponent->dbgGetCurAnims(i,ruleText);
                    GFX_ADAPTER->drawDBGText(spaces + ruleText);
                }
                GFX_ADAPTER->drawDBGText("Playing anims: ");
            }

            AnimatedComponent* animComponent = animLightComponent->DynamicCast<AnimatedComponent>(ITF_GET_STRINGID_CRC(AnimatedComponent,1654726928));
            if ( animComponent )
            {
                if (getTemplate()->getDebugAnimInputs())
                {
                    u32 numInputs = animComponent->getNumInputs();
                    for ( u32 i = 0; i < numInputs; i++ )
                    {
                        String8 inputText;
                        if (animComponent->getInputType(i) == InputType_F32)
                        {
                            f32 inputValue;
                            animComponent->getInputValue(i, inputValue);
                            inputText.setTextFormat("%s : %.2f", animComponent->getInputName(i), inputValue);
                        }
                        else // InputType_U32
                        {
                            u32 inputValue;
                            animComponent->getInputValue(i, inputValue);
                            inputText.setTextFormat("%s : %d", animComponent->getInputName(i), inputValue);
                        }
                        GFX_ADAPTER->drawDBGText(spaces + inputText);
                    }
                    GFX_ADAPTER->drawDBGText("Inputs: ");
                }

                String8 nodeText;
                animComponent->dbgGetCurRule(nodeText);
                SPRINTF_S_TRUNC(buf,MAX_BUFF,"Playing nodes: %s",nodeText.cStr());
                GFX_ADAPTER->drawDBGText(buf);

                f32 blending = animComponent->getPrevBlending();

                if ( blending )
                {
                    SPRINTF_S_TRUNC(buf,MAX_BUFF,"Blending previous anim: %f%%",blending);
                    GFX_ADAPTER->drawDBGText(buf);
                }
            }
        }
    }

    if ( getTemplate()->getDebugAI() )
    {
        AIComponent* ai = m_actor->GetComponent<AIComponent>();
        if (ai)
        {
            String8 msg = "AI : ";
            ai->getDebugText(msg);
            GFX_ADAPTER->drawDBGText(msg.cStr());
        }
    }
    SoundComponent* soundComponent = m_actor->GetComponent<SoundComponent>();
    if ( soundComponent )
    {
        if (getTemplate()->getDebugSoundInputs())
        {
            String8 spaces = "               ";
            u32 numInputs = soundComponent->getNumInputs();
            for ( u32 i = 0; i < numInputs; i++ )
            {
                String8 inputText;
                if (soundComponent->getInputType(i) == InputType_F32)
                {
                    f32 inputValue;
                    soundComponent->getInputValue(i, inputValue);
                    inputText.setTextFormat("%s : %.2f", soundComponent->getInputName(i), inputValue);
                }
                else // InputType_U32
                {
                    u32 inputValue;
                    soundComponent->getInputValue(i, inputValue);
                    inputText.setTextFormat("%s : %d", soundComponent->getInputName(i), inputValue);
                }
                GFX_ADAPTER->drawDBGText(spaces + inputText);
            }
            GFX_ADAPTER->drawDBGText("Sound Inputs: ");
        }
    }

    String8 title = "    --  ";
    title = title + m_actor->getUserFriendly();
    title = title + "  --";
    GFX_ADAPTER->drawDBGText(title.cStr());
    GFX_ADAPTER->drawDBGText("");

#endif //!defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID)
}
#endif // ITF_SUPPORT_EDITOR

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(CharacterDebuggerComponent_Template)
BEGIN_SERIALIZATION(CharacterDebuggerComponent_Template)

    SERIALIZE_MEMBER("showCollider", m_showCollider);
    SERIALIZE_MEMBER("showTrajectory", m_showTrajectory);
    SERIALIZE_MEMBER("debugController", m_debugController);
    SERIALIZE_MEMBER("debugAnim", m_debugAnim);
    SERIALIZE_MEMBER("debugAnimInputs", m_debugAnimInputs);
    SERIALIZE_MEMBER("debugAI", m_debugAI);
    SERIALIZE_MEMBER("debugMaxTrajectory", m_maxTrajectory);
    SERIALIZE_MEMBER("debugSoundInputs", m_debugSoundInputs);

END_SERIALIZATION()

CharacterDebuggerComponent_Template::CharacterDebuggerComponent_Template()
: m_showCollider(btrue)
, m_showTrajectory(btrue)
, m_debugController(btrue)
, m_debugAnim(btrue)
, m_debugAnimInputs(bfalse)
, m_debugAI(btrue)
, m_maxTrajectory(64)
, m_debugSoundInputs(bfalse)
{

}

}

