#include "precompiled_gameplay.h"

#ifndef _ITF_TRAILCOMPONENT_H_
#include "gameplay/components/misc/TrailComponent.h"
#endif //_ITF_TRAILCOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(TrailComponent)


    BEGIN_SERIALIZATION_CHILD(TrailComponent)
    END_SERIALIZATION()


    ITF_INLINE Vec2d GetInterPlus(const Vec2d & _pos, const Vec2d & _norm, f32 _lenght)
    {
        return _pos + Turn90(_norm) * _lenght;
    }
    
    ITF_INLINE Vec2d GetInterMinus(const Vec2d & _pos, const Vec2d & _norm, f32 _lenght)
    {
        return _pos - Turn90(_norm) * _lenght;
    }


    void drawPatchCurve(Patch32Data *_pData, Texture *texture, TrailPointInfo *_ppA, TrailPointInfo  *_ppB, u32 _hdiv, u32 _vdiv, f32 depth)
    {
        Vec2d           NSPoints[16];
        ColorInteger    color[4];
        f32             lenght;

        lenght = (_ppA->m_pos - _ppB->m_pos).norm();
        Vec2d n1 = _ppA->m_norm;
        Vec2d n2 = _ppB->m_norm;

        Vec2d pp1Pos = _ppA->m_pos - n1*(_ppA->m_thickness*0.5f);
        Vec2d pp2Pos = _ppB->m_pos - n2*(_ppB->m_thickness*0.5f);
        Vec2d pp3Pos = _ppA->m_pos + n1*(_ppA->m_thickness*0.5f);
        Vec2d pp4Pos = _ppB->m_pos + n2*(_ppB->m_thickness*0.5f);
        
        {
            lenght        = (pp1Pos - pp2Pos).norm()*.5f;
            NSPoints[0]   = pp1Pos;
            NSPoints[1]   = GetInterPlus(pp1Pos, n1*-1, lenght);
            NSPoints[2]   = GetInterMinus(pp2Pos, n2*-1, lenght);
            NSPoints[3]   = pp2Pos;
            lenght        = (pp3Pos - pp4Pos).norm()*.5f;
            NSPoints[4]   = pp3Pos;
            NSPoints[5]   = GetInterMinus(pp3Pos, n1, lenght);
            NSPoints[6]   = GetInterPlus(pp4Pos, n2, lenght);
            NSPoints[7]   = pp4Pos;
        } 

        
        color[0] = ColorInteger(0xFF, 0xFF, 0xFF, ITFCOLOR_F32_TO_U8(_ppA->m_alpha));
        color[2] = color[0];

        color[1] = ColorInteger(0xFF, 0xFF, 0xFF, ITFCOLOR_F32_TO_U8(_ppB->m_alpha));
        color[3] = color[2];

        //independent from Points[0-7] UV
        n1 = Vec2d(0,0);
        n2 = Vec2d(0,0);
        pp1Pos = _ppA->m_uv1;
        pp2Pos = _ppB->m_uv1;
        pp3Pos = _ppA->m_uv2;
        pp4Pos = _ppB->m_uv2;


        lenght        = (pp1Pos - pp2Pos).norm()*.5f;
        NSPoints[8]   = pp1Pos;
        NSPoints[9]   = GetInterPlus(pp1Pos, n1*-1, lenght);
        NSPoints[10]  = GetInterMinus(pp2Pos, n2*-1, lenght);
        NSPoints[11]  = pp2Pos;

        lenght        = (pp3Pos -  pp4Pos).norm()*.5f;
        NSPoints[12]  = pp3Pos;
        NSPoints[13]  = GetInterMinus(pp3Pos, n1, lenght);
        NSPoints[14]  = GetInterPlus(pp4Pos, n2, lenght);
        NSPoints[15]  = pp4Pos;

#ifdef _PATCH_USESHADER
        GFX_ADAPTER->shaderDrawPatch32(texture, NSPoints, color);
#else
        GFX_ADAPTER->drawPatch32PCT(_pData, texture, NSPoints, _hdiv, _vdiv, color, depth);
#endif
    }


    TrailComponent::TrailComponent()
    : m_trailPointTab(NULL)
    , m_trailFaiding(bfalse)
    , m_trailFaidingValue(0)
    , m_trailFaidingTime(0.0001f)
    , m_currTrailBegin(0)
    , m_currTrailLenght(0)
    , m_animComponent(NULL)
    , m_trailActive(true)
    {
        if (m_PDataCount == 0)
        {
            m_PData = newAlloc(mId_Animation, Patch32Data(VertexFormat_PCT));
        }
        m_PDataCount++;

        m_mesh.initStaticIndexMesh(VertexFormat_PCT, getMyStaticIndexBuffer());
    }


    TrailComponent::~TrailComponent()
    {
        clear();

        m_mesh.emptyIndexedMesh();
        if (--m_PDataCount == 0)
        {
            SF_DEL(m_PData);
        }
    }

    Patch32Data *TrailComponent::m_PData        = NULL;
    u32          TrailComponent::m_PDataCount   = 0;


    void TrailComponent::clear()
    {
        SF_DEL_ARRAY(m_trailPointTab);

        if (m_actor && m_texture.isValidResourceId())
        {
            m_actor->removeResource(m_texture);
            m_texture.invalidateResourceId();
        }
    }

    bbool TrailComponent::needsDraw() const 
    {
        return !getTemplate()->getDraw2D();
    }


    bbool TrailComponent::needsDraw2D() const 
    {
        return getTemplate()->getDraw2D(); 
    }

    void TrailComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        if(!getTemplate()->getTextureFile().isEmpty())
        {
            m_texture = m_actor->addResource(Resource::ResourceType_Texture, getTemplate()->getTextureFile());
        }

        m_attachBone = getTemplate()->getAttachBone();

        m_trailActive       = getTemplate()->getStartActive();

        m_trailPointTab     = new_array(TrailPointInfo,getTemplate()->getNBFrames(),mId_Gameplay);
        m_currTrailLenght   = 0;
        m_currTrailBegin    = 0;

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrail,2898640431),this);
    }

    void TrailComponent::Update( f32 _deltaTime )
    {
        if ( !getTemplate()->getNBFrames() || !m_trailPointTab)
        {
            return;
        }

        TrailPointInfo *pTrailPoint;
        f32 ratio = 1.f/(f32) getTemplate()->getNBFrames();;
        f32 alphaBegin =  getTemplate()->getAlphaBegin();
        f32 alphaEnd = getTemplate()->getAlphaEnd();


        f32 thicknessScale(1);
        if (getTemplate()->getDraw2D())
        {
            thicknessScale = (f32)GFX_ADAPTER->getScreenHeight();
        }

        if (m_trailFaiding)
        {
            f32 frameFade = _deltaTime/m_trailFaidingTime;
            m_trailFaidingValue += frameFade;
            alphaBegin  -= m_trailFaidingValue;
            if (alphaBegin < 0.f) alphaBegin = 0.f;
            alphaEnd    -= m_trailFaidingValue;
            if (alphaEnd < 0.f) alphaEnd = 0.f;
        }


        const u32 nbFrames = getTemplate()->getNBFrames();


        if (m_trailActive)
        {
            Vec2d   dir;
            Vec2d   pos;
            u32 prevTrailBegin;

            getAttachPos(pos);
            if (pos != Vec2d::Zero) // YOUS QUICK FIX !!
            {
                prevTrailBegin      = m_currTrailBegin;
                m_currTrailBegin    = (m_currTrailBegin + 1) % nbFrames;
                if (m_currTrailLenght < nbFrames)
                    m_currTrailLenght++;

                pTrailPoint = &m_trailPointTab[m_currTrailBegin];
                pTrailPoint->m_life       = nbFrames;
                pTrailPoint->m_pos        = pos;
                if (m_currTrailLenght == 1)
                {
                    Vec2d::CosSin(&dir, m_actor->getAngle());
                    pTrailPoint->m_norm    = dir.getPerpendicular();
                } else
                {
                    dir                    = pTrailPoint->m_pos - m_trailPointTab[prevTrailBegin].m_pos;
                    pTrailPoint->m_norm    = dir.getPerpendicular();
                    pTrailPoint->m_norm.normalize();
                    pTrailPoint->m_norm    = pTrailPoint->m_norm * (1 - getTemplate()->getTrailBlending()) + m_trailPointTab[prevTrailBegin].m_norm*getTemplate()->getTrailBlending();
                    if (pTrailPoint->m_norm.dot(m_trailPointTab[prevTrailBegin].m_norm) < 0)
                        pTrailPoint->m_norm *= -1.f;
                    if (pTrailPoint->m_norm.sqrnorm() < MTH_EPSILON)
                        pTrailPoint->m_norm= m_trailPointTab[prevTrailBegin].m_norm;
                }
                pTrailPoint->m_norm.normalize();

                pTrailPoint->m_thickness   = getTemplate()->getThicknessBegin()*thicknessScale;
                pTrailPoint->m_alpha       = alphaBegin;

                pTrailPoint->m_uv1         = Vec2d(0, 0);
                pTrailPoint->m_uv2         = Vec2d(0, 1);
            }
        }

        if (m_currTrailLenght)
        {
            ratio                   = 1.f/(f32)m_currTrailLenght;
            f32 thicknessFrameRatio = (getTemplate()->getThicknessBegin()*thicknessScale - getTemplate()->getThicknessBegin()*thicknessScale)*ratio;
            f32 alphaFrameRatio     = (alphaEnd - alphaBegin)*ratio;

            u32 trailPos        = (m_currTrailBegin + nbFrames - 1) % nbFrames;
            TrailPointInfo *pTrail;
            for (f32 trailCount=1.0f; trailCount < m_currTrailLenght; ++trailCount)
            {
                pTrail = &m_trailPointTab[trailPos];
                if (pTrail->m_life <= 0)
                    break;

                pTrail->m_life--;
                pTrail->m_thickness = getTemplate()->getThicknessBegin()*thicknessScale + thicknessFrameRatio*trailCount;
                pTrail->m_alpha     = alphaBegin     + alphaFrameRatio*trailCount;
                pTrail->m_uv1.m_x   = ratio*trailCount;
                pTrail->m_uv2.m_x   = ratio*trailCount;

                trailPos = (trailPos + nbFrames - 1) % nbFrames;
            }
        }

        if (m_trailFaiding && m_trailFaidingValue >= 1.0f)
        {
            m_trailActive       = bfalse;
            m_trailFaiding      = bfalse;
            m_currTrailLenght   = 0;
            for (u32 trailCount=0; trailCount < nbFrames; ++trailCount)
                m_trailPointTab[trailCount].m_life = 0;
        }
        updateAABB();
    }

    void TrailComponent::updateAABB()
    {
        bbool   aabbInit = bfalse;
        AABB    aabb;

        const u32 nbFrames = getTemplate()->getNBFrames();
        for (u32 trailCount=0; trailCount < nbFrames; ++trailCount)
        {
            TrailPointInfo *pTrail = &m_trailPointTab[trailCount];
            if (pTrail->m_life > 0)
            {
                if (!aabbInit)
                {
                    aabb = AABB(pTrail->m_pos - pTrail->m_norm*(pTrail->m_thickness*0.5f));
                    aabb.grow(pTrail->m_pos + pTrail->m_norm*(pTrail->m_thickness*0.5f));
                    aabbInit = btrue;
                } else
                {
                    aabb.grow(pTrail->m_pos - pTrail->m_norm*(pTrail->m_thickness*0.5f));
                    aabb.grow(pTrail->m_pos + pTrail->m_norm*(pTrail->m_thickness*0.5f));
                }
            }
        }
        if (aabbInit)
            GetActor()->growRelativeAABBFromAbsolute(aabb);
    }

    void TrailComponent::Draw()
    {
#ifdef _PATCH_USESHADER 
        Texture* defaultTexture = m_mesh.getDefaultTexture();
        if (defaultTexture && defaultTexture->isPhysicallyLoaded())
            GFX_ADAPTER->addPrimitive(GFX_ADAPTER->getPriorityFromRankAndDepth(m_actor->getRenderRank(), m_actor->getDepth()), defaultTexture , GFX_ITF_MESH, NULL, 0, 0, NULL, NULL, NULL, this);
#else
        DirectDraw();
#endif
    }

    void TrailComponent::Draw2D()
    {
        DirectDraw();
    }


    void TrailComponent::DirectDraw()
    {
        u32 trailPos        = m_currTrailBegin;
        u32 nextTrailPos;
        
        if (!m_texture.isValidResourceId())
            return;

        Texture *texture = (Texture *)m_texture.getResource();
        ITF_ASSERT(texture);
        if (!texture)
            return;

#ifdef _PATCH_USESHADER    
        GFX_ADAPTER->prepareShaderPatch(&m_mesh.getMatrix(), m_actor->getDepth(), m_hDiv, m_vDiv);
        u32 patchCount = 0;
        u32 renderpass = 0;
#endif

        m_PData->Init(&m_mesh, VertexFormat_PCT);
        m_PData->mesh->m_ElementList[0].m_material.m_textureDiffuse   = m_texture;
        m_PData->mesh->m_ElementList[0].m_material.m_blendMode = getTemplate()->getBlendMode();

#ifndef _PATCH_USESHADER 
#if PATCH32_WORKINGBUFF == 0
        m_PData->vertexListPT = (VertexPT*)m_PData->LockVBuff();
#endif // PATCH32_WORKINGBUFF
#endif // _PATCH_USESHADER 

        const u32 nbFrames = getTemplate()->getNBFrames();
        //f32 trailLenght = (f32)m_currTrailLenght;

        for (f32 trailCount=1.0f; trailCount < m_currTrailLenght; ++trailCount)
        {
            if (m_trailPointTab[trailPos].m_life <= 0)
                break;
            nextTrailPos = (trailPos + nbFrames - 1) % nbFrames;

            drawPatchCurve(m_PData,texture,
                &m_trailPointTab[trailPos], &m_trailPointTab[nextTrailPos],
                m_hDiv, m_vDiv, m_actor->getDepth());

            trailPos = nextTrailPos;

#ifdef _PATCH_USESHADER    
        patchCount++;
        if (patchCount >= 16)
        {
            GFX_ADAPTER->shaderPatchFlush(m_hDiv, m_vDiv, renderpass++);
            patchCount = 0;
        }
#endif
            //break;
        }

#ifndef _PATCH_USESHADER 
#if PATCH32_WORKINGBUFF == 0
        m_PData->UnlockVBuff();
#else // PATCH32_WORKINGBUFF
        m_PData->CopyToMesh();
#endif // PATCH32_WORKINGBUFF
        m_mesh.m_type =     ITF_Mesh::meshType_PTC;
        if (getTemplate()->getDraw2D())
            GFX_ADAPTER->addPrimitive2d(Vec2d(0,0),Vec2d(1.f,1.f),0, GFX_ITF_MESH,(void*)&m_mesh);
        else
            GFX_ADAPTER->addPrimitiveInZList((void*)&m_mesh, GFX_ITF_MESH, NULL, m_actor->getDepth(),0,m_actor->getRef());
#else
        GFX_ADAPTER->shaderPatchFlush(m_hDiv, m_vDiv, renderpass);
        GFX_ADAPTER->endShaderPatch();
#endif

    }

    void TrailComponent::onEvent( Event * _event)
    {
        Super::onEvent(_event);

        if ( EventTrail* trailing = _event->DynamicCast<EventTrail>(ITF_GET_STRINGID_CRC(EventTrail,2898640431)) )
        {
            if (trailing->getLaunch())
            {
                m_trailActive   = btrue;
                m_trailFaiding  = bfalse;
                m_currTrailLenght = 0;

                const u32 nbFrames = getTemplate()->getNBFrames();
                for (u32 trailCount=0; trailCount < nbFrames; ++trailCount)
                    m_trailPointTab[trailCount].m_life = 0;
            } else
            {
                m_trailFaiding      = btrue;
                m_trailFaidingValue = 0.f;
            }
        }
    }

    void TrailComponent::onResourceReady()
    {
        Super::onResourceReady();

        m_animComponent = m_actor->GetComponent<AnimLightComponent>();
        if (m_animComponent)
        {
            m_attachBone.init(m_animComponent);
        }
    }

    void TrailComponent::getAttachPos( Vec2d& _pos )
    {
        if (m_animComponent && m_attachBone.isValid())
        {
            m_animComponent->getBonePos(m_attachBone.getIndex(), _pos);
        }
        else
        {
            _pos = m_actor->get2DPos();
        }
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(TrailComponent_Template)
    BEGIN_SERIALIZATION_CHILD(TrailComponent_Template)
        SERIALIZE_MEMBER("texture", m_textureFile);
        SERIALIZE_MEMBER("nbFrames", m_nbFrames);

        SERIALIZE_MEMBER("thicknessBegin", m_thicknessBegin);
        SERIALIZE_MEMBER("thicknessEnd", m_thicknessEnd);

        SERIALIZE_MEMBER("alphaBegin", m_alphaBegin);
        SERIALIZE_MEMBER("alphaEnd", m_alphaEnd);
        SERIALIZE_MEMBER("startActive", m_startActive);
        SERIALIZE_MEMBER("trailFaidingTime", m_trailFadingTime);
        SERIALIZE_MEMBER("trailBlending", m_trailBlending);

        SERIALIZE_MEMBER("attachBone", m_attachBone);
        SERIALIZE_MEMBER("draw2D", m_draw2D);
    END_SERIALIZATION()

    TrailComponent_Template::TrailComponent_Template()
    : m_nbFrames(1)
    , m_thicknessBegin(1.0f)
    , m_thicknessEnd(0.0f)
    , m_alphaBegin(1.0f)
    , m_alphaEnd(1.0f)
    , m_startActive(btrue)
    , m_trailFadingTime(0.0001f)
    , m_trailBlending(0.f)
    , m_draw2D(bfalse)
    {
    }
}
