#ifndef _ITF_GAMEMATERIAL_H_
#define _ITF_GAMEMATERIAL_H_

#ifndef _ITF_RESOURCE_H_   
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_ 

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

#ifndef _ITF_FEEDBACKFXMANAGER_H_
#include "engine/actors/managers/FeedbackFXManager.h"
#endif //_ITF_FEEDBACKFXMANAGER_H_

namespace ITF
{

	class ResourceGroup;
	class ITF_ParticleGenerator;
	class StringID;
	class FxKit;

	///////////////////////////////////////////////////////////////////////
	// GameMaterial_Template
	///////////////////////////////////////////////////////////////////////
	class GameMaterial_Template : public TemplateObj
	{
		DECLARE_OBJECT_CHILD_RTTI(GameMaterial_Template, TemplateObj,3805522588);
		DECLARE_SERIALIZE()

	public:
        GameMaterial_Template()
            : m_solid(btrue)
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
            , m_archetype(StringID::Invalid)
            , m_type(StringID::Invalid)
            , m_fxKit(NULL)
        {}
		GameMaterial_Template(const Path& _path);

		virtual ~GameMaterial_Template();

        virtual bbool               onTemplateLoaded();

		ITF_INLINE FxKit*			getFxKit()				const { return m_fxKit; }
		ITF_INLINE bbool			isSolid()				const { return m_solid; }
		ITF_INLINE bbool			getNoStim()				const { return m_nostim; }
        ITF_INLINE bbool            getNoStick()            const { return m_noStick; }
        ITF_INLINE bbool            getNoWalkOnWall()       const { return m_noWalkOnWall; }
        ITF_INLINE bbool            getNoShadow()           const { return m_noShadow; }
		ITF_INLINE f32				getMoveSpeed()			const { return m_moveSpeed; }
		ITF_INLINE f32				getMoveForce()			const { return m_moveForce; }
		ITF_INLINE f32				getFriction()			const { return m_friction; }
        ITF_INLINE f32              getSpeedMultiplier()    const { return m_speedMultiplier; }
		ITF_INLINE f32				getRegionFriction()		const { return m_regionFriction; }
		ITF_INLINE f32				getRegionFrictionAir()	const { return m_regionFrictionAir; }
		ITF_INLINE const u32		getAmount()				const { return m_amount; }
        ITF_INLINE f32		        getWaterResistance()	const { return m_waterResistance; }
        ITF_INLINE const StringID&  getArchetype()          const { return m_archetype; }
        ITF_INLINE const StringID&  getType()               const { return m_type; }

        const ResourceID&                               getResourceGroup() const { return m_resourceGroup; }

        static Path                 nullMatName;
        static StringID             s_nullMatID;

	protected:

		bbool		m_solid;
		bbool		m_nostim;
        bbool       m_noStick;
        bbool       m_noWalkOnWall;
        bbool       m_noShadow;
		f32			m_moveSpeed;
		f32			m_moveForce;
		f32			m_friction;
        f32         m_speedMultiplier;
		f32			m_regionFriction;
		f32			m_regionFrictionAir;
        f32			m_waterResistance;
		u32			m_amount;
        StringID    m_archetype;
        StringID    m_type;

        ResourceID                      m_resourceGroup;

		// TODO : delete this parameter and the associated class when all game material are changed to
		// the new feedback system (then this will be obsolete)
		FxKit*      m_fxKit;
	};
}

#endif // _ITF_GAMEMATERIAL_H_

