#include "precompiled_engine.h"

#ifndef _ITF_TRAIL_H_
#include "engine/display/Trail.h"
#endif //_ITF_TRAIL_H_

#if !defined(ITF_WII)
#define _INVERT_2D_Y
#endif // !defined(ITF_WII)

namespace ITF
{
    BEGIN_SERIALIZATION(Trail)
    END_SERIALIZATION()


    Trail::Trail()
    : m_template(NULL)
    , m_hdiv(MESH_DEFAULT_HDIV)
    , m_vdiv(MESH_DEFAULT_VDIV)
    , m_color(Color::white())
    , m_thicknessScale(1)
    , m_is2D(bfalse)
    , m_alphaMultiplier(1)
    , m_trailFaidingValue(0.f)
    , m_currTrailLenght(0)
    , m_currTrailBegin(0)
    , m_useLife(bfalse)
    , m_useSetColor(bfalse)
    , m_setColor(Color::white())
    {
        ITF_UNUSED(m_hdiv);
        ITF_UNUSED(m_vdiv);
    }


    Trail::~Trail()
    {
#if defined(ANIMUSESHADER)  || defined(ANIMUSEGEOMETRYSHADER)
#else
        m_mesh.emptyIndexedMesh();
#endif
    }

    void Trail::initData()
    {
        const u32 nbFrames = getTemplate()->getNBFrames();

        m_color = getTemplate()->getInitColor();
        m_trailPointTab.resize(nbFrames);
        for (u32 trailCount=0; trailCount < nbFrames; ++trailCount)
            m_trailPointTab[trailCount].m_life = 0;
    }

    void Trail::setColor( const Color& _color )
    {
        m_useSetColor = btrue;
        m_setColor = _color;
        m_color = m_setColor;
    }

    void Trail::resetSetColor()
    {
        m_useSetColor = bfalse;
        m_color = getTemplate()->getInitColor();
    }

    void Trail::multiplyColor( const Color& _color )
    {
        m_color = ( m_useSetColor ? m_setColor : getTemplate()->getInitColor() ) * _color;
    }

    void Trail::startFade() 
    { 
        m_trailFaidingValue = getTemplate()->getTrailFadingTime();
    } 

    void Trail::clear()
    {
        m_currTrailLenght = 0;
        u32 count = m_trailPointTab.size();
        for (u32 trailCount=0; trailCount < count; ++trailCount)
            m_trailPointTab[trailCount].m_life = 0;
        m_color = getTemplate()->getInitColor();
    }


    void Trail::addPoint(Vec3d &_pos, f32 _angle)
    {
        const u32 nbFrames = getTemplate()->getNBFrames();
        if ( !nbFrames)
            return;

        
        f32 screenHeight = (f32)GFX_ADAPTER->getScreenHeight();
        f32 scale = m_is2D?screenHeight:1;
        scale *= m_thicknessScale;

        TrailPoint *pTrailPoint;
        Vec2d   dir;
        u32 prevTrail3DBegin;
        prevTrail3DBegin      = m_currTrailBegin;
        m_currTrailBegin    = (m_currTrailBegin + 1) % nbFrames;
        if (m_currTrailLenght < nbFrames)
            m_currTrailLenght++;

        pTrailPoint = &m_trailPointTab[m_currTrailBegin];
        pTrailPoint->m_pos         = _pos;
        pTrailPoint->m_life        = nbFrames;
        if (m_currTrailLenght == 1)
        {
            Vec2d::CosSin(&dir, _angle);
            pTrailPoint->m_norm = dir.getPerpendicular();
        } else
        {
            dir                 = (pTrailPoint->m_pos - m_trailPointTab[prevTrail3DBegin].m_pos).truncateTo2D();
            pTrailPoint->m_norm = dir.getPerpendicular();
            pTrailPoint->m_norm.normalize();
            pTrailPoint->m_norm    = pTrailPoint->m_norm * (1 - getTemplate()->getTrailBlending()) + m_trailPointTab[prevTrail3DBegin].m_norm*getTemplate()->getTrailBlending();
            if (pTrailPoint->m_norm.dot(m_trailPointTab[prevTrail3DBegin].m_norm) < 0)
                pTrailPoint->m_norm *= -1.f;
            if (pTrailPoint->m_norm.sqrnorm() < MTH_EPSILON)
                pTrailPoint->m_norm= m_trailPointTab[prevTrail3DBegin].m_norm;
        }
        pTrailPoint->m_norm.normalize();

        pTrailPoint->m_thickness   = scale * getTemplate()->getThicknessBegin();
        pTrailPoint->m_alpha       = getTemplate()->getAlphaBegin();

        pTrailPoint->m_uv1         = Vec2d::Zero;
        pTrailPoint->m_uv2         = Vec2d(0.f, 1.f);
    }

    bbool Trail::fade( f32 _deltaTime )
    {
        m_trailFaidingValue -= _deltaTime;
        f32 alphaFaid        = m_trailFaidingValue/getTemplate()->getTrailFadingTime();

        if (alphaFaid < 0.f)
            return bfalse;

        multiplyColor(Color(alphaFaid,1.f,1.f,1.f));
        return btrue;
    }

    void Trail::update()
    {
        const u32 nbFrames = getTemplate()->getNBFrames();
        TrailPoint *pTrailPoint;
        f32             ratio   = 1.f/(f32)m_currTrailLenght;
        f32 thicknessFrameRatio = (getTemplate()->getThicknessEnd() - getTemplate()->getThicknessBegin())*ratio;
        f32 alphaFrameRatio     = (getTemplate()->getAlphaEnd() - getTemplate()->getAlphaBegin())*ratio;

        f32 screenHeight = (f32)GFX_ADAPTER->getScreenHeight();
        f32 scale = m_is2D?screenHeight:1;
        scale *= m_thicknessScale;
        u32 trailPos        = (m_currTrailBegin + nbFrames - 1) % nbFrames;
        for (f32 trailCount=1.0f; trailCount < m_currTrailLenght; ++trailCount)
        {
            pTrailPoint = &m_trailPointTab[trailPos];
            if (pTrailPoint->m_life <= 0)
                break;

            if (m_useLife)
                pTrailPoint->m_life--;
            pTrailPoint->m_thickness = scale * getTemplate()->getThicknessBegin() + thicknessFrameRatio*trailCount;
            pTrailPoint->m_alpha     = getTemplate()->getAlphaBegin()     + alphaFrameRatio*trailCount;
            pTrailPoint->m_uv1.m_x   = ratio*trailCount;
            pTrailPoint->m_uv2.m_x   = ratio*trailCount;

            trailPos = (trailPos + nbFrames - 1) % nbFrames;
        }
    }

    void Trail::updateAABB(AABB &_aabb)
    {
        const u32 nbFrames = getTemplate()->getNBFrames();

        for (u32 trailCount=0; trailCount < nbFrames; ++trailCount)
        {
            TrailPoint *pTrailPoint = &m_trailPointTab[trailCount];
            if (pTrailPoint->m_life > 0)
            {
                Vec2d pos2D = pTrailPoint->m_pos.truncateTo2D();
                _aabb.grow(pos2D - pTrailPoint->m_norm*(pTrailPoint->m_thickness*0.5f));
                _aabb.grow(pos2D + pTrailPoint->m_norm*(pTrailPoint->m_thickness*0.5f));
            }
        }
    }

    bbool Trail::directDraw()
    {
        if (m_currTrailLenght < 2)
            return bfalse;

#ifdef _INVERT_2D_Y
        f32 screenHeight = (f32)GFX_ADAPTER->getScreenHeight();
#endif

#if defined(ANIMUSESHADER) || defined(ANIMUSEGEOMETRYSHADER)
        u32 trailCount = 0;
        m_shaderPointBuffer.reserve(m_currTrailLenght*2);
        m_shaderPointBuffer.resize(0);
        const u32 nbFrames = getTemplate()->getNBFrames();

        for (u32 trailPos = m_currTrailBegin;
            trailCount < m_currTrailLenght;
            trailPos = (trailPos + nbFrames - 1) % nbFrames, trailCount++)
        {
            if (m_trailPointTab[trailPos].m_life <= 0)
                break;

            TrailPoint &trail = m_trailPointTab[trailPos];
            Vec3d p1(trail.m_pos - trail.m_norm.to3d()*(trail.m_thickness*0.5f));
            Vec3d p2(trail.m_pos + trail.m_norm.to3d()*(trail.m_thickness*0.5f));
#ifdef _INVERT_2D_Y
            if (m_is2D)
            {
                p1.m_y = screenHeight-p1.m_y;
                p2.m_y = screenHeight-p2.m_y;
            }
#endif
            m_shaderPointBuffer.push_back(p1);
            m_shaderPointBuffer.push_back(p2);
        }

        GFX_ADAPTER->setAlphaBlend(getTemplate()->getBlendMode());
        Color c(m_color);
        c.m_a *= m_alphaMultiplier;
        GFX_ADAPTER->drawTrail3D((Texture *)m_texture.getResource(), c, m_shaderPointBuffer, getTemplate()->getAlphaBegin(), getTemplate()->getAlphaEnd(), getTemplate()->getFadeLength());
#else
        u32         trailCount = 0;
        Color       tmpColor(Color::white());

        #ifdef VBMANAGER_USE_DYNAMICRING_VB
            ITF_VertexBuffer * pVB = m_mesh.m_vtxBuffer;
            // Set real vtx number to avoid extra lock
            if(pVB->bIsDynamic())
                pVB->m_nVertex = min(m_currTrailLenght, getTemplate()->getNBFrames()) * (m_hdiv + 1);
            if(!pVB->m_nVertex)
                return bfalse;
            VertexPCT *pdata;
            GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(pVB,(void **)&pdata);
        #else // VBMANAGER_USE_DYNAMICRING_VB
            u32         posIdx = 0;
            u32         pointBufferSize = (m_hdiv+1)*m_currTrailLenght;
            VertexPCT   tmpVertex;
            m_meshPointBuffer.resize(pointBufferSize);
            memset(&m_meshPointBuffer[0], 0, pointBufferSize * sizeof(VertexPCT));
        #endif // VBMANAGER_USE_DYNAMICRING_VB
        
        u32 lastTrail = U32_INVALID;
        for (u32 trailPos = m_currTrailBegin;
            trailCount < m_currTrailLenght;
            trailPos = (trailPos + getTemplate()->getNBFrames() - 1) % getTemplate()->getNBFrames(), trailCount++) //CASA::TOCHECK
        {
            if (m_trailPointTab[trailPos].m_life <= 0)
                break;
            
            TrailPoint &trail   = m_trailPointTab[trailPos];
            tmpColor.m_a        = trail.m_alpha;
            
            if (getTemplate()->getFadeLength() > 0.f && trailCount > 0 &&
               (trail.m_pos - m_trailPointTab[lastTrail].m_pos).sqrnorm() < getTemplate()->getFadeLength()*getTemplate()->getFadeLength() )
            {
                tmpColor.m_a *= (trail.m_pos - m_trailPointTab[lastTrail].m_pos).norm() / getTemplate()->getFadeLength();
            }

            u32   tmpColorU32   = tmpColor.getAsU32();
            
            Vec3d normBase      = trail.m_norm.to3d()*trail.m_thickness;
            Vec3d pos           = trail.m_pos - normBase*0.5f;
            Vec3d normStep      = normBase / (f32)m_hdiv;

            Vec2d uv            = trail.m_uv1;
            Vec2d uvStep        = (trail.m_uv2 - trail.m_uv1) / (f32)m_hdiv;

            for (u32 i=0; i<=m_hdiv; i++, pos += normStep, uv += uvStep)
            {
#ifdef VBMANAGER_USE_DYNAMICRING_VB
                #ifdef USE_WGP
                    #ifdef _INVERT_2D_Y
                    if (m_is2D)
                    {
                        pVB->wgp_write(tmpColorU32, pos.m_z, Vec2d(pos.m_x, screenHeight - pos.m_y), uv);
                    }
                    else
                    #endif
                    {
                        pVB->wgp_write(tmpColorU32, pos.m_z, pos.truncateTo2D(), uv);
                    }
                #else // USE_WGP
                    pdata->setData(pos, uv, tmpColorU32);
                    #ifdef _INVERT_2D_Y
                    if (m_is2D)
                    {
                        pdata->m_pos.m_y = screenHeight - pdata->m_pos.m_y;
                    }
                    #endif
                    pdata++;
                #endif // USE_WGP
#else // VBMANAGER_USE_DYNAMICRING_VB
                tmpVertex.setData(pos, uv, tmpColorU32);
                #ifdef _INVERT_2D_Y
                if (m_is2D)
                {
                    tmpVertex.m_pos.m_y = screenHeight - tmpVertex.m_pos.m_y;
                }
                #endif
                m_meshPointBuffer[posIdx++] = tmpVertex;
#endif // VBMANAGER_USE_DYNAMICRING_VB
            }
            lastTrail = trailPos;
        }

#ifndef VBMANAGER_USE_DYNAMICRING_VB
        m_mesh.m_userBuffer = !m_mesh.m_userBuffer;
        u32 currentBuffer = m_mesh.m_userBuffer;
        ITF_VertexBuffer * pVB = m_mesh.m_vtxBufferBuffered[currentBuffer];

        void *pdata;
        GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(pVB,&pdata);
        ITF_MemcpyWriteCombined(pdata,&m_meshPointBuffer[0],pointBufferSize*sizeof(VertexPCT));
#endif // VBMANAGER_USE_DYNAMICRING_VB
        GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(pVB);

        m_mesh.m_globalColor = m_color;
        m_mesh.m_globalColor.m_a *= m_alphaMultiplier;
        m_mesh.m_ElementList[0].m_material.m_textureDiffuse = m_texture;  //CASA::TOCHECK
        m_mesh.m_ElementList[0].m_material.m_blendMode = getTemplate()->getBlendMode();
        m_mesh.m_ElementList[0].m_count = (m_currTrailLenght-1) * 6 * m_hdiv;
#endif
        return btrue;
    }

#if defined(ANIMUSESHADER) || defined(ANIMUSEGEOMETRYSHADER)
#else
    void Trail::initMesh( )
    {
        m_mesh.emptyIndexedMesh();
        ITF_IndexBuffer * indexBuffer = GFX_ADAPTER->getIndexBuffer(m_hdiv, m_vdiv);
        ITF_ASSERT(getTemplate()->getNBFrames() <= m_vdiv);
        u32 u32_meshMaxVertexBufferSize = getTemplate()->getNBFrames()*(m_hdiv + 1);
        m_mesh.initStaticIndexMesh(VertexFormat_PCT, indexBuffer, u32_meshMaxVertexBufferSize);
        #ifndef VBMANAGER_USE_DYNAMICRING_VB
        m_meshPointBuffer.reserve(u32_meshMaxVertexBufferSize);
        #endif // VBMANAGER_USE_DYNAMICRING_VB
    }
#endif


    BEGIN_SERIALIZATION(Trail_Template)

        SERIALIZE_MEMBER("texture", m_textureFile);
        SERIALIZE_MEMBER("nbFrames", m_nbFrames);
        SERIALIZE_MEMBER("trailFaidingTime", m_trailFadingTime);

        SERIALIZE_MEMBER("thicknessBegin",  m_thicknessBegin);
        SERIALIZE_MEMBER("thicknessEnd",    m_thicknessEnd);

        SERIALIZE_MEMBER("alphaBegin",  m_alphaBegin);
        SERIALIZE_MEMBER("alphaEnd",    m_alphaEnd);
        SERIALIZE_MEMBER("trailBlending", m_trailBlending);
        SERIALIZE_MEMBER("fadeLength", m_fadeLength);
        SERIALIZE_ENUM_BEGIN("blendmode",m_blendMode);
        SERIALIZE_ENUM_VAR(GFX_BLEND_UNKNOWN);
        SERIALIZE_ENUM_VAR(GFX_BLEND_COPY);
        SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHA);
        SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHAPREMULT);
        SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHADEST);
        SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHADESTPREMULT);
        SERIALIZE_ENUM_VAR(GFX_BLEND_ADD);
        SERIALIZE_ENUM_VAR(GFX_BLEND_ADDALPHA);
        SERIALIZE_ENUM_VAR(GFX_BLEND_SUBALPHA);
        SERIALIZE_ENUM_VAR(GFX_BLEND_SUB);
        SERIALIZE_ENUM_VAR(GFX_BLEND_MUL);
        SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHAMUL);
        SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHAMUL);
        SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHA);
        SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHAPREMULT);
        SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHADEST);
        SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHADESTPREMULT);
        SERIALIZE_ENUM_VAR(GFX_BLEND_MUL2X);
        SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHATOCOLOR);
        SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHATOCOLOR);
        SERIALIZE_ENUM_END();
        SERIALIZE_MEMBER("color", m_initColor);
        SERIALIZE_MEMBER("attachBone", m_attachBone);

    END_SERIALIZATION()

    Trail_Template::Trail_Template()
    : m_nbFrames(1)
    , m_trailFadingTime(1.f)
    , m_thicknessBegin(1.0f)
    , m_thicknessEnd(0.0f)
    , m_alphaBegin(1.0f)
    , m_alphaEnd(1.0f)
    , m_trailBlending(0.f)
    , m_fadeLength(0.f)
    , m_blendMode(GFX_BLEND_ALPHA)
    , m_initColor(Color::white())
    {

    }
}
