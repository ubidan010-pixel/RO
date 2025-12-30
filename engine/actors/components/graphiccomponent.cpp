#include "precompiled_engine.h"

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(GraphicComponent)

BEGIN_SERIALIZATION_CHILD(GraphicComponent)

    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)

        BEGIN_CONDITION_BLOCK(ESerialize_Deprecate)
        SERIALIZE_MEMBER("ColorFog_Red",m_colorFog_red);
        SERIALIZE_MEMBER("ColorFog_Green",m_colorFog_green);
        SERIALIZE_MEMBER("ColorFog_Blu",m_colorFog_blu);
        SERIALIZE_MEMBER("fogfactor",m_colorFog_alpha);
        m_colorFog = Color(m_colorFog_alpha/255.0f, m_colorFog_red/255.0f, m_colorFog_green/255.0f, m_colorFog_blu/255.0f);
        END_CONDITION_BLOCK()

        SERIALIZE_MEMBER("ColorFactor", m_colorFactor);
        SERIALIZE_MEMBER("ColorFog", m_colorFog);

        // WARNING don't>of serialize in FINAL.
        SERIALIZE_MEMBER("colorComputerTagId", m_colorComputerTagId);
        SERIALIZE_MEMBER("renderInTarget", m_renderInTarget);
		SERIALIZE_MEMBER("disableLight",m_disableLight);

    END_CONDITION_BLOCK()

END_SERIALIZATION()

GraphicComponent::GraphicComponent()
: Super()
, m_hDiv(2)
, m_vDiv(2)
, m_alpha(1.0f)
, m_alphaSrc(1.0f)
, m_alphaDst(1.0f)
, m_faidingTime(1.0f)
, m_faidingValue(0.0f)
, m_pauseOnFade(bfalse)
, m_destroyOnFade(bfalse)
, m_matFlags(0)
, m_renderInTarget(bfalse)

//deprecate (start)
, m_colorFog_red(255.f)
, m_colorFog_green(255.f)
, m_colorFog_blu(255.f)
, m_colorFog_alpha(0.f)
//deprecate (end)

, m_colorFog(0.f, Color::white().m_r, Color::white().m_g, Color::white().m_b)
, m_colorFactor(Color::white())
, m_colorComputerTagId(0)
, m_shadowObj(NULL)
, m_disableLight(UNINIT_BBOOL)
,m_onEndCallback(NULL)
{
}

GraphicComponent::~GraphicComponent()
{
	clear();
}

void GraphicComponent::clear()
{
	if (m_shadowObj)
	{
		if (m_shadowObj->m_shadowTexture.isValidResourceId() && m_actor)
		{
			m_actor->removeResource(m_shadowObj->m_shadowTexture);
			m_shadowObj->m_shadowTexture.invalidateResourceId();
		}

		destroyShadowMesh();

		SF_DEL(m_shadowObj)
	}
}

ITF_IndexBuffer* GraphicComponent::getStaticIndexBuffer(u32 _hdiv, u32 _vdiv)
{
    return GFX_ADAPTER->getIndexBuffer(_hdiv, _vdiv);
};


void GraphicComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    if (m_disableLight == UNINIT_BBOOL || getTemplate()->forceDisableLight())
    {
        m_disableLight = getTemplate()->getDisableLight();
    }

#ifdef ITF_CTR
	m_hDiv = 2;
	m_vDiv = 2;
#else
#ifdef ITF_WII
#define DOWNSHIFT 1
#else
#define DOWNSHIFT 0
#endif
    if (getTemplate()->getPatchLevel())
    {
        m_hDiv  = getTemplate()->getPatchLevel()>>DOWNSHIFT;
        m_vDiv  = getTemplate()->getPatchLevel()>>DOWNSHIFT;
    }
    else
    {
        m_hDiv  = getTemplate()->getHDiv()>>DOWNSHIFT;
        m_vDiv  = getTemplate()->getVDiv()>>DOWNSHIFT;
    }
#endif
    /*if (getTemplate()->getRenderInTarget())
        m_matFlags |= MAT_F_RENDERINTARGET;
    else
        m_matFlags &= ~MAT_F_RENDERINTARGET;*/

	if (getTemplate()->getIsUseShadow())
	{
		m_shadowObj = newAlloc(mId_Vertex, ShadowObj);
		// constructor ??.
		m_shadowObj->m_renderShadow = bfalse;
		m_shadowObj->m_shadowAlpha = 1.f;
		m_shadowObj->m_shadowMul = 1.f;

#ifndef VBMANAGER_USE_DYNAMICRING_VB
		if (!m_shadowObj->m_shadowMesh.m_vtxBufferBuffered[0])
#else
		if (!m_shadowObj->m_shadowMesh.m_vtxBuffer)
#endif // VBMANAGER_USE_DYNAMICRING_VB
		{
			createShadowMesh();
		}
	}

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventShow,111532112),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventReset,2551104629),this);
}

void GraphicComponent::onCheckpointLoaded()
{
    Super::onCheckpointLoaded();

    reset();
}

void GraphicComponent::reset()
{
    m_alpha = 1.f;
    m_faidingTime = 0.0f;
    m_faidingValue = 0.0f;
}

void GraphicComponent::processAlpha(f32 _deltaTime)
{
    if (m_faidingValue <= 0.f|| m_faidingTime <= 0.f)
        return;

    m_faidingValue -= _deltaTime;
    if (m_faidingValue <= 0.f)
    {
        m_alpha = m_alphaDst;
        if (m_pauseOnFade)
            m_actor->disable();
        else if (m_destroyOnFade)
            m_actor->requestDestruction();
        if (m_onEndCallback) {
            m_onEndCallback();
        }
    }
    else
    {
        f32 t = 1.f - ( m_faidingValue / m_faidingTime );
        m_alpha = Interpolate(m_alphaSrc,m_alphaDst,t);
    }
}


void GraphicComponent::processShadow(f32 _deltaTime, f32 _angle)
{
	//	shadow construction:
	//
	//	p0. --- p1. --- contactPos. --- p2. --- p3.
	//

	if (getTemplate()->getIsUseShadow())
	{
		// reinit States.
		m_shadowObj->m_renderShadow = bfalse;
		m_shadowObj->m_sS = 1;
		m_shadowObj->m_sE = 3;

		/// Find collisions polylines.
		PhysRayCastContactsContainer contacts;
		Vec3d offsetVector = getTemplate()->getShadowOffsetPos();

		if (getTemplate()->getUseRootBone())
			Vec3d_Rotate(&offsetVector, fabsf(_angle));

		if (m_actor->isFlipped())
			offsetVector.m_x *= -1.f;

		Vec2d rayCastDir = Vec2d(0, -getTemplate()->getShadowDist());
		const Vec2d raycastP0 = m_actor->getPos().truncateTo2D() + offsetVector.truncateTo2D();
		const Vec2d raycastP1 = raycastP0 + rayCastDir;

		if ( PHYSWORLD->rayCastEnvironment( raycastP0, raycastP1, ECOLLISIONFILTER_ENVIRONMENT, m_actor->getDepth(), contacts) )
		{
			/// get nearest collision.
			f32 coeff = F32_INFINITY;
			u32 minc = U32_INVALID;

			for ( u32 i = 0; i < contacts.size(); i++ )
			{
                SRayCastContact& c = contacts[i];

				if ( c.m_collidableUserData == m_actor->getRef() )
				{
					continue;
				}

				PolyLine* poly = AIUtils::getPolyLine(c.m_collidableUserData);

				if ( !poly || poly->getOwnerActor() == m_actor )
				{
					continue;
				}

                if ( c.m_edgeIndex0 < poly->getVectorsCount() )
                {
                    const PolyLineEdge& edge = poly->getEdgeAt(c.m_edgeIndex0);
                    const GameMaterial_Template* mat = World::getGameMaterial(edge.getGameMaterial());

                    if ( mat && mat->getNoShadow() )
                    {
                        continue;
                    }
                }

                if ( c.m_edgeIndex1 < poly->getVectorsCount() && c.m_edgeIndex1 != c.m_edgeIndex0 )
                {
                    const PolyLineEdge& edge = poly->getEdgeAt(c.m_edgeIndex1);
                    const GameMaterial_Template* mat = World::getGameMaterial(edge.getGameMaterial());

                    if ( mat && mat->getNoShadow() )
                    {
                        continue;
                    }
                }

				if ( contacts[i].m_t0 < coeff )
				{
					minc = i;
					coeff = contacts[i].m_t0;
				}
			}

			if ( minc == U32_INVALID || contacts[minc].m_edgeIndex0 == U32_INVALID )
			{
				return;
			}

			m_shadowObj->m_shadowAlpha = 1.f - coeff;

			PolyLine* polyLine;
			const PolyLineEdge* edge;
			AIUtils::getPolyLine(contacts[minc].m_collidableUserData,contacts[minc].m_edgeIndex0, polyLine, edge);

			f32 rejectAngle = fabsf(atan2f(edge->m_normalizedVector.m_y, edge->m_normalizedVector.m_x)) * MTH_RADTODEG;
			if (rejectAngle > 80)
				return;

#ifdef showallcontacts
			for ( u32 i = 0; i < contacts.size(); i++ )
			{
				Vec2d contactPos = p0 + ( p1 - p0) *contacts[i].m_t0;
				GFX_ADAPTER->drawDBGCircle(contactPos.m_x, contactPos.m_y, 0.2f, 1.f,0.f,0.f);
			}
#endif

			Vec2d contactPos = raycastP0 + ( raycastP1 - raycastP0) * contacts[minc].m_t0;

			Vec2d shadsize = getShadowSize();
			//subanimation can control sizex.
			shadsize.m_x *= m_shadowObj->m_shadowMul;
			f32 tsize = shadsize.m_x * m_shadowObj->m_shadowAlpha;
			f32 halfsize = tsize * 0.5f;

            Vec2d tmp = edge->m_normalizedVector * halfsize;
			Vec2d p1 = contactPos - tmp;
			Vec2d p2 = contactPos + tmp;

			const f32 mindist = 0.3f;
			const f32 offsetcut = 0.2f;

			PolyLine* nextPoly = NULL;
            u32 nextEdge = U32_INVALID;
			f32 leng = 0.f;

			Vec2d p1edg = edge->getPos();
			Vec2d p2edg = p1edg + (edge->m_normalizedVector * edge->m_length);

			/// p1 > end of edge ??.
			leng = (p2edg - p1).norm();
			if (leng > edge->m_length)
			{
				/// Find on the left.
				AIUtils::getAdjacentEdge(polyLine, contacts[minc].m_edgeIndex0, bfalse, nextPoly, nextEdge);
				if (nextPoly)
				{
					const PolyLineEdge* edgen = &nextPoly->getEdgeAt(nextEdge);
					if (edgen)
					{
						if (leng > (edge->m_length + mindist))
						{
							// Limit angle.
							f32 anglelimit = getTemplate()->getAngleLimit();
							f32 angleSegLimit = f32_ACos(edge->m_normalizedVector.dot(edgen->m_normalizedVector))* MTH_RADTODEG;
							f32 angleEdge = f32_Abs(atan2f(edgen->m_normalizedVector.m_y, edgen->m_normalizedVector.m_x)) * MTH_RADTODEG;
							if ((anglelimit > 0.f && angleEdge > anglelimit) || angleSegLimit > 88)
							{
								 p1 = p1edg - (edge->m_normalizedVector * offsetcut);
							}
							else
							{
#ifdef simpleShadow
								contactPos = edge->getPos();
								f32 dst = f32_Abs(halfsize - (contactPos - edge->getPos()).norm());
								p1 = contactPos - (edgen->m_normalizedVector * dst);
#else
								p1 = edge->getPos();
								f32 dst = halfsize - (contactPos - p1).norm();
								m_shadowObj->m_sS--;
								Vec2d p0 = p1 - (edgen->m_normalizedVector * dst);
								m_shadowObj->m_shadowPos[0] = Vec3d( p0.m_x, p0.m_y, m_actor->getDepth());
#endif
							}
						}
					}
				}
				else
				{
					p1 = p1edg - (edge->m_normalizedVector * offsetcut);
				}
			}

			/// p2 > start of edge ??.
			leng = (p2 - p1edg).norm();
			if (leng > edge->m_length)
			{
				nextPoly = NULL;
				nextEdge = U32_INVALID;

				/// Find on the right.
				AIUtils::getAdjacentEdge(polyLine, contacts[minc].m_edgeIndex0, btrue, nextPoly, nextEdge);
				if (nextPoly)
				{
					const PolyLineEdge* edgen = &nextPoly->getEdgeAt(nextEdge);
					if (edgen)
					{
						if (leng > (edge->m_length + mindist))
						{
							// Limit angle. World/local?
							f32 anglelimit = getTemplate()->getAngleLimit();
							f32 angleSegLimit = f32_ACos(edge->m_normalizedVector.dot(edgen->m_normalizedVector))* MTH_RADTODEG;
							f32 angleEdge = f32_Abs(atan2f(edgen->m_normalizedVector.m_y, edgen->m_normalizedVector.m_x)) * MTH_RADTODEG;
							if ((anglelimit > 0.f && angleEdge > anglelimit) || angleSegLimit > 88)
							{
                                Vec2d::ScaleAdd(&p2, &edge->m_normalizedVector, offsetcut, &p2edg);
							}
							else
							{
#ifdef simpleShadow
								contactPos = edgen->getPos();
								f32 dst = halfsize - (edgen->getPos() - contactPos).norm();
								p2 = contactPos + (edgen->m_normalizedVector * dst);
#else
								p2 = edgen->getPos();
								f32 dst = halfsize - (p2 - contactPos).norm();
								m_shadowObj->m_sE++;
                                Vec2d p3;
                                Vec2d::ScaleAdd(&p3, &edgen->m_normalizedVector, dst, &p2);
								m_shadowObj->m_shadowPos[4] = Vec3d( p3.m_x, p3.m_y, m_actor->getDepth());
#endif
							}
						}
					}
				}
				else
				{
                    Vec2d::ScaleAdd(&p2, &edge->m_normalizedVector, offsetcut, &p2edg);
				}
			}

			m_shadowObj->m_shadowPos[1] = Vec3d( p1.m_x, p1.m_y, m_actor->getDepth());
			m_shadowObj->m_shadowPos[2] = Vec3d( contactPos.m_x, contactPos.m_y, m_actor->getDepth());
			m_shadowObj->m_shadowPos[3] = Vec3d( p2.m_x, p2.m_y, m_actor->getDepth());

			m_shadowObj->m_shadowTexture = m_actor->addResource(Resource::ResourceType_Texture, getTemplate()->getShadowTextureFile());;
			m_shadowObj->m_renderShadow = btrue;

			computeVertexBufferCache();
		}
	}
}


void GraphicComponent::setColorFog(const Color& _color)
{
    m_colorFog = _color;
}

const Color& GraphicComponent::getColorFog()
{
    return m_colorFog;
}

void GraphicComponent::setColorFactor(const Color& _color)
{
    m_colorFactor = _color;
}

const Color& GraphicComponent::getColorFactor()
{
    return m_colorFactor;
}

void GraphicComponent::Update( f32 _deltaTime )
{
    ITF_ASSERT(!m_pause);
    // Compute AABB
    ITF_ASSERT(m_actor);

    AABB aabb;

    aabb.setMin(m_actor->getPos().truncateTo2D() + getTemplate()->getPosOffset() - Vec2d(f32_Abs(m_actor->getScale().m_x) * 0.5f, f32_Abs(m_actor->getScale().m_y) * 0.5f));
    aabb.setMax(m_actor->getPos().truncateTo2D() + getTemplate()->getPosOffset() + Vec2d(f32_Abs(m_actor->getScale().m_x) * 0.5f, f32_Abs(m_actor->getScale().m_y) * 0.5f));

    m_actor->growRelativeAABBFromAbsolute(aabb);

    processAlpha(_deltaTime);
}

void GraphicComponent::createShadowMesh()
{
	//if a previous mesh is created destroy it
	destroyShadowMesh();

#ifndef VBMANAGER_USE_DYNAMICRING_VB
	m_shadowObj->m_shadowMesh.m_vtxBufferBuffered[0] = GFX_ADAPTER->createVertexBuffer(10 , VertexFormat_PCT, sizeof(VertexPCT), 1, VB_T_MESH);
    ITF_ASSERT(m_shadowObj->m_shadowMesh.m_vtxBufferBuffered[0]);
    m_shadowObj->m_shadowMesh.m_vtxBufferBuffered[1] = GFX_ADAPTER->createVertexBuffer(10 , VertexFormat_PCT, sizeof(VertexPCT), 1, VB_T_MESH);
    ITF_ASSERT(m_shadowObj->m_shadowMesh.m_vtxBufferBuffered[1]);
#else
	m_shadowObj->m_shadowMesh.m_vtxBuffer = GFX_ADAPTER->createVertexBuffer(10 , VertexFormat_PCT, sizeof(VertexPCT), 1, VB_T_MESH);
#endif // VBMANAGER_USE_DYNAMICRING_VB

	//create mesh element
	if( m_shadowObj->m_shadowMesh.m_ElementList.size() == 0 )
	{
		ITF_MeshElement elem;
		elem.m_startIndex = 0;
		elem.m_count = 0;
		m_shadowObj->m_shadowMesh.m_ElementList.push_back(elem);
	}

	//create mesh element index buffer
	m_shadowObj->m_shadowMesh.m_ElementList[0].m_indexBuffer = GFX_ADAPTER->createIndexBuffer(24, bfalse);

	//fill mesh element index buffer
	u16* indexBuffer;
	m_shadowObj->m_shadowMesh.m_ElementList[0].m_indexBuffer->Lock((void**)&indexBuffer);

	u16 inc = 0;
    for (u32 i = 0; i < 4 ; i++ , inc += 2)
	{
		indexBuffer[i * 6 + 0] = inc + 0;
		indexBuffer[i * 6 + 1] = inc + 1;
		indexBuffer[i * 6 + 2] = inc + 3;
		indexBuffer[i * 6 + 3] = inc + 3;
		indexBuffer[i * 6 + 4] = inc + 2;
		indexBuffer[i * 6 + 5] = inc + 0;
	}

	m_shadowObj->m_shadowMesh.m_ElementList[0].m_indexBuffer->Unlock();

	//set up mesh element
	m_shadowObj->m_shadowMesh.m_ElementList[0].m_count = 24;
	m_shadowObj->m_shadowMesh.m_ElementList[0].m_material.m_blendMode = GFX_BLEND_ALPHA;

    if ( !getTemplate()->getShadowTextureFile().isEmpty() )
    {
		m_shadowObj->m_shadowTexture = m_actor->addResource(Resource::ResourceType_Texture, getTemplate()->getShadowTextureFile());
    }

	m_shadowObj->m_shadowMesh.m_ElementList[0].m_material.m_textureDiffuse = m_shadowObj->m_shadowTexture;

	//set mesh color
	m_shadowObj->m_shadowMesh.m_globalColor = Color::white();
	//set mesh matrix
	m_shadowObj->m_shadowMesh.setMatrix((GMatrix44&)GMatrixIdentity);
}

//------------------------------------------------------------------

void GraphicComponent::destroyShadowMesh()
{
	//free vertex buffers
#ifndef VBMANAGER_USE_DYNAMICRING_VB
	if( m_shadowObj->m_shadowMesh.m_vtxBufferBuffered[0] != NULL )
	{
		GFX_ADAPTER->removeVertexBuffer(m_shadowObj->m_shadowMesh.m_vtxBufferBuffered[0]);
		m_shadowObj->m_shadowMesh.m_vtxBufferBuffered[0] = NULL;
	}
	if( m_shadowObj->m_shadowMesh.m_vtxBufferBuffered[1] != NULL )
	{
		GFX_ADAPTER->removeVertexBuffer(m_shadowObj->m_shadowMesh.m_vtxBufferBuffered[1]);
		m_shadowObj->m_shadowMesh.m_vtxBufferBuffered[1] = NULL;
	}
#else
	if( m_shadowObj->m_shadowMesh.m_vtxBuffer != NULL )
	{
		GFX_ADAPTER->removeVertexBuffer(m_shadowObj->m_shadowMesh.m_vtxBuffer);
    	m_shadowObj->m_shadowMesh.m_vtxBuffer = NULL;
	}
#endif //VBMANAGER_USE_DYNAMICRING_VB

	//free index buffer
	if(m_shadowObj-> m_shadowMesh.m_ElementList.size() > 0 )
	{
		if(m_shadowObj->m_shadowMesh.m_ElementList[0].m_indexBuffer != NULL)
		{
			GFX_ADAPTER->removeIndexBuffer(m_shadowObj->m_shadowMesh.m_ElementList[0].m_indexBuffer);
		}
	}
}

void GraphicComponent::computeVertexBufferCache()
{
	Vec3d dir;
	Vec3d perp;
	Vec3d lastperp;

    Vec2d shadsize = getShadowSize();
	shadsize.m_x *= m_shadowObj->m_shadowMul;
	u32 nS = m_shadowObj->m_sE - m_shadowObj->m_sS;

	f32 hlfsizey = shadsize.m_y * m_shadowObj->m_shadowAlpha * 0.5f;

	f32 distU = 0.f;
	for (u32 i = 0; i < nS; i++ )
	{
		const Vec3d& p1 = m_shadowObj->m_shadowPos[i + m_shadowObj->m_sS];
		const Vec3d& p2 = m_shadowObj->m_shadowPos[i + m_shadowObj->m_sS + 1];
		f32 length = (p2 - p1).norm();

		dir = (p2 - p1).normalize();
        Vec3d::ZAxis.crossProduct(dir, perp);

		// Blend with last perpendicular.
		if (i>0)
		{
			perp.m_x = f32_Lerp(perp.m_x, lastperp.m_x, 0.5f);
			perp.m_y = f32_Lerp(perp.m_y, lastperp.m_y, 0.5f);
			perp.normalize();
		}

		lastperp = perp;
		perp *= hlfsizey;

		m_shadowObj->m_cacheVB[ i * 2].m_pos = p1 - perp;
		m_shadowObj->m_cacheVB[i * 2+1].m_pos = p1 + perp;

		m_shadowObj->m_cacheVB[i * 2].m_uv = Vec2d(distU, 0.f);
		m_shadowObj->m_cacheVB[i * 2+1].m_uv = Vec2d(distU, 1.f);

		distU += length/shadsize.m_x;

		if (i == nS - 1)
		{
			perp = dir.getPerpendicular();
			perp *= hlfsizey;

			m_shadowObj->m_cacheVB[i * 2 + 2].m_pos = p2 - perp;
			m_shadowObj->m_cacheVB[i * 2 + 3].m_pos = p2 + perp;

			m_shadowObj->m_cacheVB[i * 2 + 2].m_uv = Vec2d(1.f, 0.f);
			m_shadowObj->m_cacheVB[i * 2 + 3].m_uv = Vec2d(1.f, 1.f);
		}
	}

	//color and UVs.
	Color col = Color::white();
	col.m_a = m_shadowObj->m_shadowAlpha * getTemplate()->getShadowAttenuation();
	u32 colo = col.getAsU32();
	for (u32 i=0; i < 10; i++)
	{
		m_shadowObj->m_cacheVB[i].m_color = colo;
	}
}

void GraphicComponent::drawShadow()
{
	if (m_shadowObj && m_shadowObj->m_renderShadow)
	{
		//switch mesh user buffer
#ifndef VBMANAGER_USE_DYNAMICRING_VB
		m_shadowObj->m_shadowMesh.m_userBuffer = !m_shadowObj->m_shadowMesh.m_userBuffer;
		u32 currentBuffer = m_shadowObj->m_shadowMesh.m_userBuffer;
		//get vertex buffer ptr
		ITF_VertexBuffer * vertexBuffer = m_shadowObj->m_shadowMesh.m_vtxBufferBuffered[currentBuffer];
#else
		ITF_VertexBuffer * vertexBuffer = m_shadowObj->m_shadowMesh.m_vtxBuffer;
#endif //VBMANAGER_USE_DYNAMICRING_VB

		u32 nS = m_shadowObj->m_sE - m_shadowObj->m_sS;
		u32 nV = nS*2+2;

		m_shadowObj->m_shadowMesh.m_ElementList[0].m_startIndex = 0;
		m_shadowObj->m_shadowMesh.m_ElementList[0].m_count = nS*6;

		//lock vertex buffer data
		VertexPCT* data = NULL;
		GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(vertexBuffer, (void**)&data );
		ITF_ASSERT_MSG(data, "Unable to lock a vertex buffer");
		if(data != NULL)
		{
			//write in vertex buffer data
			ITF_MemcpyWriteCombined(data, &m_shadowObj->m_cacheVB, sizeof(VertexPCT)*nV);
			//unlock vertex buffer
			GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(vertexBuffer);
			//add primitive to be drawn
            GFX_ADAPTER->addPrimitiveInZList((void*)&m_shadowObj->m_shadowMesh, GFX_ITF_MESH, NULL, GetActor()->getDepth() - 0.01f + getTemplate()->getShadowOffsetPos().m_z, btrue, m_actor->getRef());
		}
	}
}

void GraphicComponent::onEvent( Event* _event)
{
    Super::onEvent(_event);

    if ( EventShow* eventShow = _event->DynamicCast<EventShow>(ITF_GET_STRINGID_CRC(EventShow,111532112)) )
    {
        m_alphaSrc      = m_alpha;
        m_alphaDst      = eventShow->getAlpha();
        m_faidingTime   = eventShow->getTransitionTime();
        if (m_faidingTime <= 0.f)
            m_alpha         = m_alphaDst;
        else
            m_faidingValue  = m_faidingTime;
        m_pauseOnFade = eventShow->getPauseOnEnd();
        m_destroyOnFade = eventShow->getDestroyOnEnd();
        m_onEndCallback = eventShow->getOnEndCallback();

    }
    else if ( _event->IsClassCRC(EventReset::GetClassCRCStatic()) )
    {
        reset();
    }
}

AABB GraphicComponent::getVisualAABB() const
{
    if ( !getTemplate() || ( getTemplate()->getVisualAABB().getMin() == Vec2d::Zero && getTemplate()->getVisualAABB().getMax() == Vec2d::Zero ) )
        return GetActor()->getAABB();
    else
    {
        AABB AABBtemp(getTemplate()->getVisualAABB());
        AABBtemp.Scale(GetActor()->getScale());
        AABBtemp.Rotate(GetActor()->getAngle(), bfalse);
        AABBtemp.Translate(GetActor()->getPos().truncateTo2D());
        return AABBtemp;
    }
}


ITF_IndexBuffer* GraphicComponent::getMyStaticIndexBuffer()
{
#if defined(ANIMUSEGEOMETRYSHADER)
    return NULL;
#else
    u32 prevHDiv = m_hDiv;
    u32 prevVDiv = m_vDiv;
    ITF_UNUSED(prevHDiv);
    ITF_UNUSED(prevVDiv);
    ITF_IndexBuffer* idxBuffer = GFX_ADAPTER->getIndexBuffer(m_hDiv, m_vDiv);

#ifndef ITF_WII
    ITF_WARNING_CATEGORY(GPP,
        m_actor, prevHDiv == m_hDiv && prevVDiv == m_vDiv,
        "Actor's component is using an invalid patch level %ix%i (only allowed: %s)",
        prevHDiv, prevVDiv, GFXAdapter::getAllowedBufferTypesStr().cStr() );
#endif

    return idxBuffer;

#endif
}

Vec2d GraphicComponent::getShadowSize()
{
    return getTemplate()->getShadowSize() * m_actor->getScale();
}



///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(GraphicComponent_Template)
BEGIN_SERIALIZATION(GraphicComponent_Template)
    SERIALIZE_MEMBER("patchLevel",m_patchLevel);
    SERIALIZE_MEMBER("patchHLevel",m_hDiv);
    SERIALIZE_MEMBER("patchVLevel",m_vDiv);
    SERIALIZE_OBJECT("visualAABB",m_visualAABB);
    SERIALIZE_MEMBER("renderintarget",m_renderInTarget);
    SERIALIZE_MEMBER("posOffset",m_posOffset);
    SERIALIZE_MEMBER("angleOffset",m_angleOffset);

    SERIALIZE_MEMBER("selfIllumColor",m_selfIllumColor);
    SERIALIZE_MEMBER("disableLight",m_disableLight);
    SERIALIZE_MEMBER("forceDisableLight",m_forceDisableLight);
    SERIALIZE_MEMBER("useShadow", m_useShadow);
	SERIALIZE_MEMBER("useRootBone", m_useRootBone);
	SERIALIZE_MEMBER("shadowTextureFile",m_shadowTextureFile);
	SERIALIZE_MEMBER("shadowSize",m_shadowSize);
	SERIALIZE_MEMBER("shadowAttenuation",m_shadowAttenuation);
	SERIALIZE_MEMBER("shadowDist",m_shadowDist);
	SERIALIZE_MEMBER("shadowOffsetPos",m_shadowOffsetPos);
	SERIALIZE_MEMBER("angleLimit",m_angleLimit);

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
    SERIALIZE_ENUM_BEGIN("materialtype",m_matType);
        SERIALIZE_ENUM_VAR(GFX_MAT_DEFAULT);
        SERIALIZE_ENUM_VAR(GFX_MAT_REFRACTION);
        SERIALIZE_ENUM_VAR(GFX_MAT_PATCH);
        SERIALIZE_ENUM_VAR(GFX_MAT_FRIEZEANIM);
        SERIALIZE_ENUM_VAR(GFX_MAT_GLOW);
        SERIALIZE_ENUM_VAR(GFX_MAT_ALPHAFADE);
        SERIALIZE_ENUM_VAR(GFX_MAT_FRIEZEOVERLAY);
    SERIALIZE_ENUM_END();
END_SERIALIZATION()

GraphicComponent_Template::GraphicComponent_Template()
: m_patchLevel(0)
, m_hDiv(2)
, m_vDiv(2)
, m_visualAABB(AABB(Vec2d::Zero))
, m_renderInTarget(0)
, m_posOffset(Vec2d::Zero)
, m_angleOffset(btrue,0.f)
, m_blendMode(GFX_BLEND_ALPHA)
, m_matType(GFX_MAT_DEFAULT)
, m_selfIllumColor(Color::zero())
, m_disableLight(bfalse)
, m_forceDisableLight(bfalse)
, m_useShadow(bfalse)
, m_useRootBone(bfalse)
, m_shadowSize(Vec2d(1.8f, 0.30f))
, m_shadowAttenuation(1.f)
, m_shadowDist(4.f)
, m_shadowOffsetPos(Vec3d::Zero)
, m_angleLimit(0.f)
{
}

GraphicComponent_Template::~GraphicComponent_Template()
{
}

}

