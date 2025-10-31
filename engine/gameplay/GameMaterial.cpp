
#include "precompiled_engine.h"

#ifndef _ITF_GAMEMATERIAL_H_
#include "engine/gameplay/GameMaterial.h"
#endif //_ITF_GAMEMATERIAL_H_

#ifndef _ITF_FXKIT_H_
#include "engine/gameplay/FxKit.h"
#endif //_ITF_FXKIT_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_RESOURCEGROUP_H_
#include "engine/resources/ResourceGroup.h"
#endif //_ITF_RESOURCEGROUP_H_

namespace ITF
{
	Path GameMaterial_Template::nullMatName;
	StringID GameMaterial_Template::s_nullMatID;

	///////////////////////////////////////////////////////////////////////
	// GameMaterial_Template
	///////////////////////////////////////////////////////////////////////
	IMPLEMENT_OBJECT_RTTI(GameMaterial_Template)
	BEGIN_SERIALIZATION(GameMaterial_Template)
        SERIALIZE_MEMBER("type",m_type);
        SERIALIZE_MEMBER("archetype",m_archetype);
        SERIALIZE_MEMBER("solid",m_solid);
		SERIALIZE_MEMBER("nostim",m_nostim);
        SERIALIZE_MEMBER("noStick",m_noStick);
        SERIALIZE_MEMBER("noWalkOnWall",m_noWalkOnWall);
        SERIALIZE_MEMBER("noShadow",m_noShadow);
		SERIALIZE_MEMBER("moveSpeed",m_moveSpeed);
		SERIALIZE_MEMBER("moveForce",m_moveForce);
		SERIALIZE_MEMBER("friction",m_friction);
        SERIALIZE_MEMBER("speedMultiplier",m_speedMultiplier);
		SERIALIZE_MEMBER("regionFriction",m_regionFriction);
		SERIALIZE_MEMBER("regionFrictionAir",m_regionFrictionAir);
        SERIALIZE_MEMBER("WaterResistance",m_waterResistance);
		SERIALIZE_MEMBER("amount",m_amount);
		SERIALIZE_OBJECT("fxkit",m_fxKit);
	END_SERIALIZATION()

    GameMaterial_Template::GameMaterial_Template(const Path& _path)
    : Super(_path)
    , m_solid(btrue)
    , m_nostim(bfalse)
    , m_noStick(bfalse)
    , m_noWalkOnWall(bfalse)
    , m_noShadow(bfalse)
    , m_moveSpeed(0.0f)
    , m_moveForce(0.0f)
    , m_friction(1.0f)
    , m_speedMultiplier(1.f)
    , m_regionFriction(0.0f)
    , m_regionFrictionAir(0.0f)
    , m_waterResistance(0.0f)
    , m_amount(0)
    , m_type(StringID::Invalid)
    , m_archetype(StringID::Invalid)
    , m_fxKit(NULL)
    {
        m_resourceGroup.invalidateResourceId();
    }

	GameMaterial_Template::~GameMaterial_Template()
	{
        if (m_resourceGroup.isValidResourceId())
        {
            ResourceGroup* grp = (ResourceGroup*)m_resourceGroup.getResource();
            u32 PhysicalCount = grp ? grp->getPhysicalUsersCount() : 0;
            while (PhysicalCount)
            {
                RESOURCE_MANAGER->removeUserToResourcePhysicalData(m_resourceGroup);
                PhysicalCount--;
            }

            RESOURCE_MANAGER->removeUserToResourceLogicalData(m_resourceGroup);
        }

        SF_DEL(m_fxKit);
	}

    bbool GameMaterial_Template::onTemplateLoaded()
    {
        Super::onTemplateLoaded();

        m_resourceGroup = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_ResourceGroup, Path::EmptyPath);

        ResourceGroup* pGroup = (ResourceGroup*)m_resourceGroup.getResource();

        // Tell FeedbackFXManager that there is a new potential data owner
        FEEDBACKFX_MANAGER->addOwner( m_archetype, m_type, pGroup );

        return btrue;
    }
}
