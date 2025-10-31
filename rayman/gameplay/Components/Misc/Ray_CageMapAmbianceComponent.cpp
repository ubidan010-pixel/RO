#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_CAGMAPAMBIANCECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_CageMapAmbianceComponent.h"
#endif //_ITF_RAY_CAGMAPAMBIANCECOMPONENT_H_
#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_
#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_


namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_CageMapAmbianceComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_CageMapAmbianceComponent)
    END_SERIALIZATION()


    BEGIN_VALIDATE_COMPONENT(Ray_CageMapAmbianceComponent)
    END_VALIDATE_COMPONENT()

        Ray_CageMapAmbianceComponent::Ray_CageMapAmbianceComponent()
        : m_forceFieldOpen(bfalse)
        , m_cageOpen(bfalse)
        , m_timer(0.0f)
        , m_init(bfalse)
        , m_darkAlphaTimer(0.0f)
        , m_lightAlphaTimer(0.0f)
        , m_lightFadeDone(bfalse)
        , m_darkFadeDone(bfalse)
        , m_state(Dark)
    {
    }

    void Ray_CageMapAmbianceComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        m_state = Dark;
        m_colorRTarget = getTemplate()->getDarkColor();
        m_afterFx.setType(AFX_Fade);
		m_afterFx.setCustomVal(17);

        m_linkComponent = m_actor->GetComponent<LinkComponent>();

        m_init = bfalse;

        m_darkAlphaTimer = getTemplate()->getDarkAlphaFadeTime();
        m_lightAlphaTimer = getTemplate()->getLightAlphaFadeTime();
        m_lightFadeDone = btrue;
        m_darkFadeDone = btrue;

    }

    Ray_CageMapAmbianceComponent::~Ray_CageMapAmbianceComponent()
    {
    }


	//----------------------------------------------------------------------------------------------------------------------------
    void Ray_CageMapAmbianceComponent::Update( f32 _deltaTime )
    {
        Super::Update(_deltaTime);

        const StringID light = ITF_GET_STRINGID_CRC(light, 1838533242);
        const StringID dark = ITF_GET_STRINGID_CRC(dark,4000110176);
        if (!m_init)
        {
            m_init = btrue;
            const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();
            const u32 size = children.size();


            for(u32 i = 0; i < size; ++i)
            {
                const ChildEntry & child = children[i];
                if (child.hasTag(dark))
                {
                    Pickable * p = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, child.getPath());
                    if (p)
                    {
                        AIUtils::setAlpha(p,1.0f);    
                    }
                }
                else if (child.hasTag(light))
                {
                    Pickable * p = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, child.getPath());
                    if (p)
                    {
                        AIUtils::setAlpha(p,0.0f);    
                    }
                }
            }
        }


        Ray_EventQueryOpen query;

        const StringID forcefield = ITF_GET_STRINGID_CRC(forcefield, 1883637587);
        m_linkComponent->sendEventToChildren(&query, forcefield);
        m_forceFieldOpen = query.isOpen();

        const StringID cage = ITF_GET_STRINGID_CRC(cage, 2003579885);
        m_linkComponent->sendEventToChildren(&query, cage);
        m_cageOpen = query.isOpen();

        switch (m_state)
        {
        case Dark:
            if (m_forceFieldOpen)
            {
                m_timer = getTemplate()->getDarkToNeutralTime();
                m_state = DarkToNeutral;
                m_darkAlphaTimer = 0.0f;
                m_darkFadeDone = bfalse;
            }
            break;
        case DarkToNeutral:
            if (m_timer == 0.0f)
            {
                m_state = Neutral;
            }
            else
            {
                f32 value = 1.0f - (m_timer/ getTemplate()->getDarkToNeutralTime());
                m_colorRTarget = Color::Interpolate(getTemplate()->getDarkColor(),getTemplate()->getNeutralColor(), value);

            }
            break;
        case  Neutral:
            if (m_cageOpen)
            {
                m_timer = getTemplate()->getNeutralToLightTime();
                m_state = NeutralToLight;
                m_lightAlphaTimer = 0.0f;
                m_lightFadeDone = bfalse;
            }
            break;
        case  NeutralToLight:
            if(m_timer == 0.0f)
            {
                m_state = Light;
            }
            else
            {
                f32 value = 1.0f - (m_timer/ getTemplate()->getDarkToNeutralTime());
                m_colorRTarget = Color::Interpolate(getTemplate()->getNeutralColor(),getTemplate()->getLightColor(), value);
            }
            break;
        case Light:
            break;
        }

        const Vec2d pos = m_actor->get2DPos();
        const f32 darkAlphFadeTime = getTemplate()->getDarkAlphaFadeTime();
        
        if (!m_darkFadeDone)
        {
            const f32 darkAlphaProgressionSpeed = getTemplate()->getDarkAlphaRadialProgressionSpeed();
            const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();
            const u32 size = children.size();

            for(u32 i = 0; i < size; ++i)
            {
                const ChildEntry & child = children[i];
                if (child.hasTag(dark))
                {
                    Pickable * p = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, child.getPath());
                    if (p)
                    {
                        m_darkFadeDone = AIUtils::setRadialAlpha(p,pos,darkAlphaProgressionSpeed,m_darkAlphaTimer,darkAlphFadeTime,bfalse);    
                    }
                }
            }

        }

        const f32 lightAlphFadeTime = getTemplate()->getLightAlphaFadeTime();

        if (!m_lightFadeDone)
        {
            const f32 lightAlphaProgressionSpeed = getTemplate()->getLightAlphaRadialProgressionSpeed();
            const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();
            const u32 size = children.size();

            for(u32 i = 0; i < size; ++i)
            {
                const ChildEntry & child = children[i];
                if (child.hasTag(light))
                {
                    Pickable * p = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, child.getPath());
                    if (p)
                    {
                        m_lightFadeDone = AIUtils::setRadialAlpha(p,pos,lightAlphaProgressionSpeed,m_lightAlphaTimer,lightAlphFadeTime,btrue);    
                    }
                }
            }
        }


        m_darkAlphaTimer = m_darkAlphaTimer+_deltaTime;
        m_lightAlphaTimer = m_lightAlphaTimer+_deltaTime;
        m_timer = Max(0.0f,m_timer - _deltaTime);

        updateAABB();

    }

    void Ray_CageMapAmbianceComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event);
    }

    void Ray_CageMapAmbianceComponent::Draw()
    {
        Super::Draw();
        
		m_afterFx.setParamcAt( 0, m_colorRTarget );

		f32 zs = 0.f;
        f32 zr = (f32)getTemplate()->getRenderRank() + m_actor->getDepth() * 5.f;
        f32 zp = zr;
        zs = (f32)getTemplate()->getStartRank() + m_actor->getDepth() * 5.f;
        zp = zs;

        m_afterFx.setZStart(zs);
        m_afterFx.setZRender(zr);
        GFX_ADAPTER->addPrimitiveInZList((void*)&m_afterFx, GFX_ITF_AFTERFX, NULL, zp ,0,m_actor->getRef());
    }

    void Ray_CageMapAmbianceComponent::updateAABB()
    {
        AABB forcedaabb = getTemplate()->getForcedAABB();
        forcedaabb.Scale(m_actor->getScale());
        forcedaabb.setMin(forcedaabb.getMin() + m_actor->get2DPos());
        forcedaabb.setMax(forcedaabb.getMax() + m_actor->get2DPos());

        AABB aabb(m_actor->getPos());

        aabb.grow(forcedaabb);

        m_actor->growRelativeAABBFromAbsolute(aabb);

    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_CageMapAmbianceComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface,_flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;
    }
#endif // ITF_SUPPORT_EDITOR

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_CageMapAmbianceComponent_Template)
    BEGIN_SERIALIZATION(Ray_CageMapAmbianceComponent_Template)
    SERIALIZE_MEMBER("darkColor",m_darkColor);
    SERIALIZE_MEMBER("neutralColor",m_neutralColor);
    SERIALIZE_MEMBER("lightColor",m_lightColor);
    SERIALIZE_MEMBER("darkToNeutralTime",m_darkToNeutralTime);
    SERIALIZE_MEMBER("neutralToLightTime",m_neutralToLightTime);
    SERIALIZE_MEMBER("startRank",m_startRank);
    SERIALIZE_MEMBER("renderRank",m_renderRank);
    SERIALIZE_MEMBER("darkAlphaFadeTime",m_darkAlphaFadeTime);
    SERIALIZE_MEMBER("darkAlphaRadialProgressionSpeed",m_darkAlphaRadialProgressionSpeed);
    SERIALIZE_MEMBER("lightAlphaFadeTime",m_lightAlphaFadeTime);
    SERIALIZE_MEMBER("lightAlphaRadialProgressionSpeed",m_lightAlphaRadialProgressionSpeed);
    SERIALIZE_OBJECT("forcedAABB",m_forcedAABB);
    END_SERIALIZATION()


    Ray_CageMapAmbianceComponent_Template::Ray_CageMapAmbianceComponent_Template()
    : m_startRank(-1000)
    , m_renderRank(1000)
    , m_darkToNeutralTime(1.0f)
    , m_neutralToLightTime(1.0f)
    , m_darkAlphaFadeTime(0.5f)
    , m_darkAlphaRadialProgressionSpeed(4.0f)
    , m_lightAlphaFadeTime(0.5f)
    , m_lightAlphaRadialProgressionSpeed(4.0f)
    , m_forcedAABB(Vec2d(-1.0f,-1.0f),Vec2d(1.0f,1.0f))
    {
    }

    Ray_CageMapAmbianceComponent_Template::~Ray_CageMapAmbianceComponent_Template()
    {
    }

}
