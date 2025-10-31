#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_WAVEBALLCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_WaveBallComponent.h"
#endif //_ITF_RAY_WAVEBALLCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_RAY_STIMCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_StimComponent.h"
#endif //_ITF_RAY_STIMCOMPONENT_H_

#ifndef _ITF_SHAPEDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/ShapeDetectorComponent.h"
#endif //_ITF_SHAPEDETECTORCOMPONENT_H_

//if _DRAW_TEXTURED_LINES is undefined, we draw pie parts
//#define _DRAW_TEXTURED_LINES

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_WaveBallComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_WaveBallComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_WaveBallComponent)
    END_VALIDATE_COMPONENT()

    static f32 virtualWaveRadius = 3.f;

    static const f32 spikeHeight = 0.5f;

    static const f32 spikeMultiplier_Preparing = 1.0f;

    ///////////////////////////////////////////////////////////////////////////////////////////
    u32 Ray_WaveBallComponent::m_lastTimeComputed = U32_INVALID;
    bbool Ray_WaveBallComponent::m_wasInitialized = bfalse;
    Vec2d Ray_WaveBallComponent::m_precomputedWaveCosSin[Ray_WaveBallComponent::sampleCount+1];
    f32 Ray_WaveBallComponent::m_computedWave[Ray_WaveBallComponent::sampleCount] = {0};
    Ray_FluidShaper_Spikes Ray_WaveBallComponent::m_shaper;

    ///////////////////////////////////////////////////////////////////////////////////////////

    Ray_WaveBallComponent::Ray_WaveBallComponent()
        : Super()
        , m_scaleCausedByState(0.3f)
        , m_state(State_Off)
        , m_timeInCurrentState(0)
        , m_color(Color::white())
        , m_spikeMultiplier(0)
        , m_isPreparing(bfalse)
        , m_indexBuffer(NULL)
    {
    }

    Ray_WaveBallComponent::~Ray_WaveBallComponent()
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
    void Ray_WaveBallComponent::setScaleCausedByState(f32 _scaleFactor)
    {
        m_scaleCausedByState = _scaleFactor;
        ShapeDetectorComponent *shapeDetector = m_actor->GetComponent<ShapeDetectorComponent>();
        if (shapeDetector)
        {
            f32 factor = _scaleFactor/getTemplate()->getScaleWhenOn();
            shapeDetector->setScaleMultiplier(factor);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_WaveBallComponent::computeWave()
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
    void Ray_WaveBallComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventWaveGeneratorNotification,1312585498), this);

        m_shaper.setRadius(virtualWaveRadius);
        m_shaper.setRampLength(0);
        m_shaper.setLinearOffset(0);
        m_isPreparing = bfalse;

        u32 lineCount = sampleCount;
        void *indexBufferData;
        ITF_ASSERT_CRASH(!m_indexBuffer, "leaked index buffer");

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
    void Ray_WaveBallComponent::computeAABB()
    {
        f32 maxScaleCausedByState = 1;//std::max(getTemplate()->getScaleWhenOn(),getTemplate()->getScaleWhenOff());
        f32 maxRadius = (1+spikeHeight)*maxScaleCausedByState*m_actor->getScale().m_x;
        AABB aabb(Vec2d(-maxRadius, -maxRadius), Vec2d(maxRadius, maxRadius));
        m_actor->growRelativeAABB(aabb);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_WaveBallComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( Ray_EventWaveGeneratorNotification* notification = _event->DynamicCast<Ray_EventWaveGeneratorNotification>(ITF_GET_STRINGID_CRC(Ray_EventWaveGeneratorNotification,1312585498)) )
        {
            
            if (notification->getIsColored())
            {
                if (m_state==State_Off)
                {
                    setState(State_OffToOn);
                }
            }
            else
            {
                m_isPreparing = notification->getIsPreparing();
                if (m_state==State_On)
                {
                    setState(State_OnToOff);
                }
            }
            
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_WaveBallComponent::setState(State_t _newState)
    {
        m_state = _newState;
        m_timeInCurrentState = 0;
        bbool isDangerousState = bfalse;

        switch(m_state)
        {
        case State_OffToOn:
            m_color = getTemplate()->getColorOn();
            break;
        case State_On: 
            m_color = getTemplate()->getColorOn();
            isDangerousState = btrue;
            setScaleCausedByState(getTemplate()->getScaleWhenOn());
            m_spikeMultiplier = 1;
            break;
        case State_OnToOff:
            m_color = getTemplate()->getColorOff();
            break;
        case State_Off: 
            m_color = getTemplate()->getColorOff();
            setScaleCausedByState(getTemplate()->getScaleWhenOff()); 
            m_spikeMultiplier = 0;
            break;
        }
        Ray_StimComponent *stimComponent = m_actor->GetComponent<Ray_StimComponent>();
        if (stimComponent)
        {
            if (isDangerousState)
                stimComponent->unpause();
            else
                stimComponent->pause();
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    f32 Ray_WaveBallComponent::getCurrentPrepareScale() const
    {
        if (m_isPreparing)
        {
            return getTemplate()->getSpikeMultiplier_Preparing();
        }
        else
        {

            return getTemplate()->getSpikeMultiplier_Idle();;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_WaveBallComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        m_timeInCurrentState += _dt;

        switch(m_state)
        {
            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_Off:
            {
                m_spikeMultiplier = getCurrentPrepareScale();
            } break;
            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_OffToOn:
            {
                 //update Transition
                f32 delay = getTemplate()->getOffToOnDelay();
                f32 lambda = std::min(m_timeInCurrentState/delay,1.f);
                m_spikeMultiplier = getCurrentPrepareScale()*(1-lambda)+lambda;
                setScaleCausedByState(getTemplate()->getScaleWhenOn()*lambda+(1-lambda)*getTemplate()->getScaleWhenOff());
                    
                if (m_timeInCurrentState>delay)
                {
                    setState(State_On);
                }
            } break;
             ///////////////////////////////////////////////////////////////////////////////////////////
            case State_OnToOff:
                {
                    //update Transition
                    f32 delay = getTemplate()->getOnToOffDelay();
                    f32 lambda = std::min(m_timeInCurrentState/delay,1.f);
                    m_spikeMultiplier = getCurrentPrepareScale()*lambda+(1-lambda);
                    setScaleCausedByState(getTemplate()->getScaleWhenOff()*lambda+(1-lambda)*getTemplate()->getScaleWhenOn());
                    if (m_timeInCurrentState>delay)
                    {
                        setState(State_Off);
                    }
                } break;
        }
        computeWave();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_WaveBallComponent::fillMesh()
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
    void Ray_WaveBallComponent::Draw()
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

            f32 scale = basicScale*(1+spikeHeight*m_spikeMultiplier*m_computedWave[i]);

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
    IMPLEMENT_OBJECT_RTTI(Ray_WaveBallComponent_Template)
    BEGIN_SERIALIZATION(Ray_WaveBallComponent_Template)
    SERIALIZE_MEMBER("scaleWhenOff", m_scaleWhenOff);
    SERIALIZE_MEMBER("scaleWhenOn", m_scaleWhenOn);
    SERIALIZE_MEMBER("offToOnDelay", m_offToOnDelay);
    SERIALIZE_MEMBER("onToOffDelay", m_onToOffDelay);
    SERIALIZE_MEMBER("texturePath", m_texturePath);
    SERIALIZE_MEMBER("spikeMultiplier_Preparing", m_spikeMultiplier_Preparing);
    SERIALIZE_MEMBER("spikeMultiplier_Idle", m_spikeMultiplier_Idle);
    SERIALIZE_MEMBER("colorOn", m_colorOn);
    SERIALIZE_MEMBER("colorOff", m_colorOff);
    END_SERIALIZATION()

    Ray_WaveBallComponent_Template::Ray_WaveBallComponent_Template()
    : m_scaleWhenOff(0.0666666f)
    , m_scaleWhenOn(0.15f)
    , m_offToOnDelay(0.1f)
    , m_onToOffDelay(0.1f)
    , m_spikeMultiplier_Preparing(1.0f)
    , m_spikeMultiplier_Idle(0.f)
    , m_colorOn(Color::red())
    , m_colorOff(Color::white())
    {
    }


}
