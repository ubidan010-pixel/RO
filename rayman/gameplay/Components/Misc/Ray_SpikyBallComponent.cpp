#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SPIKYBALLCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SpikyBallComponent.h"
#endif //_ITF_RAY_SPIKYBALLCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_RAY_STIMCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_StimComponent.h"
#endif //_ITF_RAY_STIMCOMPONENT_H_

#ifndef _ITF_SHAPEDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/ShapeDetectorComponent.h"
#endif //_ITF_SHAPEDETECTORCOMPONENT_H_

#ifndef _ITF_RAY_TRIGGERBOUNCECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_TriggerBounceComponent.h"
#endif //_ITF_RAY_TRIGGERBOUNCECOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

//if _DRAW_TEXTURED_LINES is undefined, we draw pie parts
//#define _DRAW_TEXTURED_LINES

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_SpikyBallComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_SpikyBallComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_SpikyBallComponent)
    VALIDATE_COMPONENT_PARAM("circles", getTemplate()->getCircles().size()>0, "no circle in actor definition");
    END_VALIDATE_COMPONENT()

    static f32 virtualWaveRadius = 3.f;

    const f32 maxSpikeHeight = 1.f;


    ///////////////////////////////////////////////////////////////////////////////////////////
    u32 Ray_SpikyBallComponent::m_lastTimeComputed = U32_INVALID;
    bbool Ray_SpikyBallComponent::m_wasInitialized = bfalse;
    Vec2d Ray_SpikyBallComponent::m_precomputedWaveCosSin[Ray_SpikyBallComponent::sampleCount+1];
    f32 Ray_SpikyBallComponent::m_computedWave[Ray_SpikyBallComponent::sampleCount] = {0};
    Ray_FluidShaper_Spikes Ray_SpikyBallComponent::m_shaper;

    ///////////////////////////////////////////////////////////////////////////////////////////

    Ray_SpikyBallComponent::Ray_SpikyBallComponent()
        : Super()
        , m_state(State_Wave)
        , m_timeInCurrentState(0)
        , m_indexBuffer(NULL)
        , m_color(Color::red())
        , m_circleIndex(0)
        , m_toCircleIndex(0)
        , m_currentTransition(NULL)
        , m_maxRadiusFactor(0)
        , m_spikeHeight(0)
        , m_scaleCausedByState(1)
        , m_fxComponent(NULL)
    {
    }

    Ray_SpikyBallComponent::~Ray_SpikyBallComponent()
    {
        if (m_indexBuffer)
        {
            GFX_ADAPTER->removeIndexBuffer(m_indexBuffer);
            m_indexBuffer = NULL;
        }
        if (m_actor && m_textureID.isValidResourceId())
        {
            m_actor->removeResource(m_textureID);            
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyBallComponent::setScaleCausedByState(f32 _scaleFactor, const Ray_SpikyBall_Circle &_circle)
    {
        m_scaleCausedByState = _scaleFactor;
        m_color = _circle.m_color;
        m_spikeHeight = _circle.m_spikeHeight;

        ShapeDetectorComponent *shapeDetector = m_actor->GetComponent<ShapeDetectorComponent>();
        if (shapeDetector)
        {
            f32 factor = _circle.m_hurtRadiusFactor;
            shapeDetector->setScaleMultiplier(factor);
        }

        AIUtils::disableCollision(m_actor, !_circle.m_hurts);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyBallComponent::computeWave()
    {
        if (m_lastTimeComputed==CURRENTFRAME)
        {
            return;
        }

        //may init sincos
        if (!m_wasInitialized)
        {
            f32 angle=0;
            f32 angleStep = MTH_2PI/sampleCount;

            for (u32 i=0; i<sampleCount; i++, angle+= angleStep)
            {
                Vec2d cossin;
                f32_CosSin(angle, &cossin.m_x, &cossin.m_y);
                m_precomputedWaveCosSin[i] = cossin;
            }
            m_precomputedWaveCosSin[sampleCount] = m_precomputedWaveCosSin[0];
            m_wasInitialized = btrue;
        }
        //init wave itself (unscaled, just linear shape)
        m_lastTimeComputed = CURRENTFRAME;

        ITF_ASSERT((sampleCount&1)==0);
        Vec2d pos;

        bbool isFound, hasColor, hasGameMaterial;
        f32 waveWeight;
        for (u32 i=0; i<sampleCount; i++)
        {
            f32 x = (i/(f32)sampleCount-0.5f)*2.f*virtualWaveRadius;
            m_computedWave[i] = (m_shaper.getHeight(x, isFound, waveWeight, hasColor, hasGameMaterial))*0.5f;
        }
        m_shaper.addTimeSlice(LOGICDT);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyBallComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventWaveGeneratorNotification,1312585498), this);

        m_fxComponent = m_actor->GetComponent<FXControllerComponent>();

        m_shaper.setRadius(virtualWaveRadius);
        m_shaper.setRampLength(0);
        m_shaper.setLinearOffset(0);
        u32 lineCount = sampleCount;
        void *indexBufferData;
        m_fxToPlay.invalidate();
        ITF_ASSERT_CRASH(!m_indexBuffer, "leaked index buffer");

        m_maxRadiusFactor = 0;
        for (u32 i=0; i<getTemplate()->getCircles().size(); i++)
        {
            m_maxRadiusFactor = std::max(m_maxRadiusFactor, getTemplate()->getCircles()[i].m_radiusFactor);
        }

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventSetSpikyFlower,4136114510), this);
        setState(State_Wave, m_circleIndex);

#ifdef _DRAW_TEXTURED_LINES
        const u32 indexCount = sampleCount*6;
        u16 indexBufferSrc[indexCount];
        u16 *ptrIndexBuffer = indexBufferSrc;
        for (u16 i=0; i<sampleCount; i++) //draw series of quads (each is 2 triangles
        {
            *(ptrIndexBuffer++) = i*2;
            *(ptrIndexBuffer++) = i*2+3;
            *(ptrIndexBuffer++) = i*2+2;

            *(ptrIndexBuffer++) = i*2;
            *(ptrIndexBuffer++) = i*2+1;
            *(ptrIndexBuffer++) = i*2+3;
        }
#else
        const u32 indexCount=sampleCount*3;
        u16  indexBufferSrc[indexCount];
        u16 *ptrIndexBuffer = indexBufferSrc;
        for (u16 i=0; i<sampleCount; i++) //draw series of quads (each is 2 triangles
        {
            *(ptrIndexBuffer++) = 0;
            *(ptrIndexBuffer++) = 1+i;
            *(ptrIndexBuffer++) = 1+i+1;
        }
#endif
        ITF_ASSERT(ptrIndexBuffer==indexBufferSrc+indexCount);
        m_indexBuffer = GFX_ADAPTER->createIndexBuffer( indexCount, bfalse);
        m_indexBuffer->Lock((void **) &indexBufferData);
        ITF_MemcpyWriteCombined(indexBufferData, indexBufferSrc, sizeof(indexBufferSrc));
        m_indexBuffer->Unlock();
        m_mesh.initDynamicIndexedMesh(VertexFormat_PCT, m_indexBuffer, lineCount*2+2);

        m_textureID = m_actor->addResource(Resource::ResourceType_Texture, getTemplate()->getTexturePath());

        computeAABB();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyBallComponent::computeAABB()
    {

        f32 maxRadius = (1+maxSpikeHeight)*m_maxRadiusFactor*m_actor->getScale().m_x;
        AABB aabb(Vec2d(-maxRadius, -maxRadius), Vec2d(maxRadius, maxRadius));
        m_actor->growRelativeAABB(aabb);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyBallComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if (Ray_EventSetSpikyFlower *setSpiky = _event->DynamicCast<Ray_EventSetSpikyFlower>(ITF_GET_STRINGID_CRC(Ray_EventSetSpikyFlower,4136114510)))
        {
            u32 newCircle = setSpiky->getCircleIndex();
            switch(m_state)
            {
            case State_Wave:
                setState(State_Transition, newCircle);
                break;
            case State_Transition:
                setState(State_Wave, m_toCircleIndex); //finish transition
                setState(State_Transition, newCircle); //start another
                break;
            }
        }

    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyBallComponent::setState(State_t _newState, u32 _circleIndex)
    {
        m_state = _newState;
        m_timeInCurrentState = 0;

        switch(m_state)
        {
        case State_Wave:
            {
                m_circleIndex = _circleIndex;
                if (m_circleIndex<getTemplate()->getCircles().size())
                {
                    const Ray_SpikyBall_Circle &circle = getTemplate()->getCircles()[m_circleIndex];
                    setScaleCausedByState(circle.m_radiusFactor, circle);
                    prepareFX(circle.m_fx);
                }
            } break;
        case State_Transition:
            {
                if (_circleIndex==m_circleIndex)
                {
                    setState(State_Wave, _circleIndex);
                }
                else
                {
                    m_toCircleIndex = _circleIndex;
                    m_currentTransition = findTransition(m_circleIndex, m_toCircleIndex);
                    if (!m_currentTransition)
                    {
                        setState(State_Wave, _circleIndex);
                    }
                    else
                    {
                        prepareFX(m_currentTransition->m_fx);
                    }
                }
            } break;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyBallComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        m_timeInCurrentState += _dt;

        if (m_fxToPlay.isValid())
        {
            if (m_fxComponent)
            {
                m_fxComponent->stopAllFX();
                m_fxComponent->playFX(m_fxToPlay);
            }
            m_fxToPlay.invalidate();
        }

        switch(m_state)
        {
            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_Wave:
            {
                if (m_circleIndex<getTemplate()->getCircles().size())
                {
                    const Ray_SpikyBall_Circle &circle = getTemplate()->getCircles()[m_circleIndex];
                    setScaleCausedByState(circle.m_radiusFactor, circle);
                }
            } break;
            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_Transition:
            {
                f32 transitionDuration=0.1f;
                if (m_currentTransition)
                {
                    transitionDuration = m_currentTransition->m_duration;
                }
                f32 lambda = 1;
                if (transitionDuration>0)
                {
                    lambda = std::min(m_timeInCurrentState/transitionDuration,1.f);                    
                }
                if (m_circleIndex<getTemplate()->getCircles().size()
                    && m_toCircleIndex<getTemplate()->getCircles().size())
                {
                    const Ray_SpikyBall_Circle &circle = getTemplate()->getCircles()[m_circleIndex];
                    const Ray_SpikyBall_Circle &toCircle = getTemplate()->getCircles()[m_toCircleIndex];
                    setScaleCausedByState(circle.m_radiusFactor*(1-lambda)+toCircle.m_radiusFactor*lambda, circle);
                }
                if (lambda>=1)
                {
                    setState(State_Wave, m_toCircleIndex);
                }

            } break;
        }
        computeWave();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyBallComponent::fillMesh()
    {
        void *pData;
        static const u32 lineCount = sampleCount;

        const Vec2d *current = m_computedPoints;
        const Vec2d *last = m_computedPoints+(sampleCount+1);
        f32 uvX = 0;

        u32 color = m_color.getAsU32();
        f32 zero=0;
        f32 uvStep = 1.f/(sampleCount);

#ifdef _DRAW_TEXTURED_LINES
        const Vec2d *currentCosSin = m_precomputedWaveCosSin;
        f32 lineWidth = 0.1f;
        static const u32 meshBufferVtxCount = lineCount*2+2;
        static VertexPCT meshBuffer[meshBufferVtxCount];        
        VertexPCT *currentVertex = meshBuffer;
        u32 indexCount = sampleCount*6;
        for (;current!=last; current++, currentCosSin++)
        {
            Vec2d widthVec(currentCosSin->m_x*lineWidth, currentCosSin->m_y*lineWidth);

            currentVertex->m_pos.setFromVec2d(*current+widthVec, &zero);
            currentVertex->m_uv = Vec2d(uvX, 1);
            currentVertex->m_color = color;            
            currentVertex++;

            currentVertex->m_pos.setFromVec2d(*current-widthVec, &zero);
            currentVertex->m_uv = Vec2d(uvX, 0);
            currentVertex->m_color = color;
            currentVertex++;
            uvX += uvStep;
        }
#else
        static const u32 meshBufferVtxCount = sampleCount+1+1; /*we add center and duplicate start point*/
        static VertexPCT meshBuffer[meshBufferVtxCount];
        u32 indexCount = sampleCount*3;
        VertexPCT *currentVertex = meshBuffer;
        currentVertex->m_pos = m_actor->getPos();
        currentVertex->m_color = color;
        currentVertex->m_uv = Vec2d(0,1);
        currentVertex++;
        for (;current!=last; current++)
        {
            currentVertex->m_pos.setFromVec2d(*current, &zero);
            currentVertex->m_uv = Vec2d(uvX, 0);
            currentVertex->m_color = color;            
            currentVertex++;
            uvX += uvStep;
        }

#endif

        ITF_ASSERT(currentVertex == meshBuffer+meshBufferVtxCount);

        m_mesh.swapVBForDynamicMesh();
        m_mesh.m_ElementList[0].m_material.m_textureDiffuse = m_textureID;
        m_mesh.m_ElementList[0].m_count = indexCount;
        ITF_VertexBuffer *vertexBuffer = m_mesh.getCurrentVB();
        GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(vertexBuffer,&pData);
        ITF_MemcpyWriteCombined(pData,meshBuffer,meshBufferVtxCount*sizeof(VertexPCT));
        GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(vertexBuffer);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    const Ray_SpikyBall_Transition *Ray_SpikyBallComponent::findTransition(u32 _i1, u32 _i2)
    {
        if (_i1>_i2)
        {
            std::swap(_i1,_i2);
        }
        u32 size = getTemplate()->getTransitions().size();
        for (u32 i=0; i<size; i++)
        {
            const Ray_SpikyBall_Transition *transition = &getTemplate()->getTransitions()[i];
            if (transition->m_firstIndex==(_i1+1) && transition->m_secondIndex==(_i2+1))
            {
                return transition;
            }
        }
        ITF_WARNING(m_actor, bfalse, "No transition between %d and %d", _i1+1, _i2+1);
        return NULL;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyBallComponent::Draw()
    {
        f32 angle=0;
        f32 angleStep = MTH_2PI/sampleCount;
        f32 basicScale = m_actor->getScale().m_x * m_scaleCausedByState;
    
        const Vec3d &center = m_actor->getPos();
        f32 zero=0;
        Vec3d projPos;

        for (u32 i=0; i<sampleCount; i++, angle+= angleStep)
        {
            Vec3d cossin;
            
            cossin.setFromVec2d(m_precomputedWaveCosSin[i], &zero);

            f32 scale = basicScale*(1+m_spikeHeight*m_computedWave[i]);

            cossin *= scale;

            //x = i*0.1f;
            //y = scale;

            cossin += center;

            //GFX_ADAPTER->compute3DTo2D(cossin, projPos);
            m_computedPoints[i] = (Vec2d&)cossin;
        }

        m_computedPoints[sampleCount] = m_computedPoints[0];
        fillMesh();


        Texture* tex = static_cast<Texture*>(m_textureID.getResource());
        if (tex)
        {
            GFX_ADAPTER->addPrimitiveInZList(&m_mesh, GFX_ITF_MESH, tex, m_actor->getDepth(),0,m_actor->getRef());
        }
        /*
        const f32 size=1.f;
        u32 color = m_color.getAsU32();
        for (u32 i=0; i<sampleCount; i++)
        {
            GFX_ADAPTER->draw2dLine(m_computedPoints[i], m_computedPoints[i+1], size, color);
        }*/
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_SpikyBall_Circle)
    BEGIN_SERIALIZATION(Ray_SpikyBall_Circle)
    SERIALIZE_MEMBER("radiusFactor", m_radiusFactor);
    SERIALIZE_MEMBER("hurtRadiusFactor", m_hurtRadiusFactor);
    SERIALIZE_MEMBER("color", m_color);
    SERIALIZE_MEMBER("hurts", m_hurts);
    SERIALIZE_MEMBER("spikeHeight", m_spikeHeight);
    SERIALIZE_MEMBER("fx", m_fx);
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(Ray_SpikyBall_Transition)
    BEGIN_SERIALIZATION(Ray_SpikyBall_Transition)
    SERIALIZE_MEMBER("firstIndex", m_firstIndex);
    SERIALIZE_MEMBER("secondIndex", m_secondIndex);
    SERIALIZE_MEMBER("duration", m_duration);
    SERIALIZE_MEMBER("fx", m_fx);
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_SpikyBallComponent_Template)
    BEGIN_SERIALIZATION(Ray_SpikyBallComponent_Template)
    SERIALIZE_CONTAINER_OBJECT("circles", m_circles);
    SERIALIZE_CONTAINER_OBJECT("transitions", m_transitions);
    SERIALIZE_MEMBER("texturePath", m_texturePath);
    END_SERIALIZATION()

    Ray_SpikyBallComponent_Template::Ray_SpikyBallComponent_Template()
    /*
    : m_scaleWhenOff(0.0666666f)
    , m_scaleWhenOn(0.15f)
    , m_offToOnDelay(0.1f)
    , m_onToOffDelay(0.1f)
    , m_spikeMultiplier_Preparing(1.0f)
    , m_spikeMultiplier_Idle(0.f)
    , m_colorOn(Color::red())
    , m_colorOff(Color::white())
    */
    {
    }


}
