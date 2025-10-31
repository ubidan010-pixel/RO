#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SPIKYSHELLTRAPCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SpikyShellTrapComponent.h"
#endif

#ifndef _ITF_PHYSCOLLISIONSOLVER_H
#include "engine/physics/PhysCollisionSolver.h"
#endif //_ITF_PHYSCOLLISIONSOLVER_H

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_SpikyShellTrapComponent_Template)
    BEGIN_SERIALIZATION(Ray_SpikyShellTrapComponent_Template)
        SERIALIZE_MEMBER("texturePath", m_texturePath);
        SERIALIZE_MEMBER("baseWidth", m_baseWidth);
        SERIALIZE_MEMBER("minScaleFactor", m_minScaleFactor);
        SERIALIZE_MEMBER("minSpacing", m_minSpacing);
        SERIALIZE_MEMBER("maxSpacing", m_maxSpacing);
        SERIALIZE_MEMBER("baseHeight", m_baseHeight);
        SERIALIZE_MEMBER("restHeightPercent", m_restHeightPercent);
        SERIALIZE_MEMBER("risenHeightPercent", m_risenHeightPercent);
        SERIALIZE_MEMBER("maxHeight", m_maxHeight);

        SERIALIZE_MEMBER("frontBaseTexIndex_Idle", m_frontBaseTexIndex_Idle);
        SERIALIZE_MEMBER("frontBaseFirstTexIndex_Shaking", m_frontBaseFirstTexIndex_Shaking);
        SERIALIZE_MEMBER("frontBaseLastTexIndex_Shaking", m_frontBaseLastTexIndex_Shaking);

        SERIALIZE_MEMBER("backBaseTexIndex_Idle", m_backBaseTexIndex_Idle);
        SERIALIZE_MEMBER("backBaseFirstTexIndex_Shaking", m_backBaseFirstTexIndex_Shaking);

        SERIALIZE_MEMBER("spikeFirstTexIndex", m_spikeFirstTexIndex);
        SERIALIZE_MEMBER("spikeLastTexIndex", m_spikeLastTexIndex);
        SERIALIZE_MEMBER("spikeWidth", m_spikeWidth);
        SERIALIZE_MEMBER("shakeDetectionRadius", m_shakeDetectionRadius);
        SERIALIZE_MEMBER("spikeDetectionRadius", m_spikeDetectionRadius);
        SERIALIZE_MEMBER("endOfSpikeDetectionRadius", m_endOfSpikeDetectionRadius);
        SERIALIZE_MEMBER("faction", m_faction);
        SERIALIZE_MEMBER("minAlertDuration", m_minAlertDuration);
        SERIALIZE_MEMBER("minSpikeDuration", m_minSpikeDuration);
        SERIALIZE_MEMBER("shakeAmplitude", m_shakeAmplitude);
        SERIALIZE_MEMBER("spikeVibration_UpDuration", m_spikeVibration_UpDuration);
        SERIALIZE_MEMBER("spikeVibration_DownDuration", m_spikeVibration_DownDuration);
        SERIALIZE_MEMBER("spikeAmplitude", m_spikeAmplitude);
        SERIALIZE_MEMBER("spikeYOffset", m_spikeYOffset);
        SERIALIZE_MEMBER("hitMarginPercent", m_hitMarginPercent);
        SERIALIZE_MEMBER("hitLevel", m_hitLevel);
        SERIALIZE_MEMBER("spikeAnimationFirstFrameDuration", m_spikeAnimationFirstFrameDuration);
        SERIALIZE_MEMBER("spikeAnimationFrameDuration", m_spikeAnimationFrameDuration);
        SERIALIZE_MEMBER("baseAnimationFrameDuration", m_baseAnimationFrameDuration);
        SERIALIZE_MEMBER("spikeInertia_Out", m_spikeInertia_Out);
        SERIALIZE_MEMBER("spikeInertia_EmergencyOut", m_spikeInertia_EmergencyOut);
        SERIALIZE_MEMBER("spikeInertia_Holster", m_spikeInertia_Holster);
        SERIALIZE_MEMBER("spikeDetectionRadius_Emergency", m_spikeDetectionRadius_Emergency);
        SERIALIZE_MEMBER("spikeBounciness", m_spikeBounciness);
        SERIALIZE_MEMBER("hitWidthScale", m_hitWidthScale);
        SERIALIZE_MEMBER("rotationMargin", m_rotationMargin);
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_SpikyShellTrapComponent)
        VALIDATE_COMPONENT_PARAM("spikeDetectionRadius_Emergency", getTemplate()->getSpikeDetectionRadius()>=getTemplate()->getSpikeDetectionRadius_Emergency(), "spikeDetectionRadius_Emergency must be less or equal than spikeDetectionRadius");
    END_VALIDATE_COMPONENT()

    static const Vec2d cosSin_2ndOrder(1.f/24.f,1.f/120.f);
    static const Vec2d cosSin_1stOrder(-1.f/6,-1.f/2.f);

    static ITF_INLINE void getApproxCosSin(f32 _x, Vec2d &_cosSin)
    {
        f32 x2 = _x*_x;
        Vec2d x2_vec(x2,x2);

        _cosSin = cosSin_2ndOrder*x2_vec;
        _cosSin = (_cosSin + cosSin_1stOrder)*x2_vec;
        _cosSin += Vec2d(1,1);
        _cosSin *= Vec2d(1,_x);
    }

    static ITF_INLINE void rotateVector(Vec3d &_rotatedVec, const Vec3d &_vec, const Vec2d &_cosSin)
    {
        (Vec2d&)_rotatedVec = _cosSin*_vec.m_x + Vec2d(-_cosSin.m_y, _cosSin.m_x)*_vec.m_y;
        _rotatedVec.m_z = _vec.m_z;
    }

    static ITF_INLINE f32 getAsymetricOscillation(f32 _t, f32 _upDuration, f32 _downDuration)
    {        
        _t = fmodf(_t, _upDuration+_downDuration);
        if (_t<=_upDuration)
        {
            return _t/_upDuration;
        }

        return 1.f-(_t-_upDuration)/_downDuration;
    }

    Ray_SpikyShellTrapComponent_Template::Ray_SpikyShellTrapComponent_Template() 
    : m_baseWidth(1.f)
    , m_minScaleFactor(0.9f)
    , m_minSpacing(0.4f)
    , m_maxSpacing(0.6f)
    , m_baseHeight(0.5f)
    , m_restHeightPercent(0.1f)
    , m_maxHeight(1.f)
    , m_frontBaseTexIndex_Idle(4)
    , m_frontBaseFirstTexIndex_Shaking(5)
    , m_frontBaseLastTexIndex_Shaking(6)
    , m_backBaseTexIndex_Idle(5)
    , m_backBaseFirstTexIndex_Shaking(6)
    , m_spikeFirstTexIndex(0)
    , m_spikeLastTexIndex(3)
    , m_spikeWidth(0.2f)
    , m_faction(RAY_FACTION_ENEMY)
    , m_shakeDetectionRadius(4.f)
    , m_spikeDetectionRadius(2.f)
    , m_endOfSpikeDetectionRadius(4.f)
    , m_minAlertDuration(1.f)
    , m_minSpikeDuration(0.5f)
    , m_shakeAmplitude(0.1f)
    , m_shakePulsation(6.f)
    , m_spikeYOffset(0.2f)
    , m_hitMarginPercent(0.1f)
    , m_hitLevel(0)
    , m_baseAnimationFrameDuration(0.1f)
    , m_spikeAnimationFirstFrameDuration(0.5f)
    , m_spikeAnimationFrameDuration(0.05f)
    , m_risenHeightPercent(0.9f)
    , m_spikeInertia_Out(0.1f)
    , m_spikeInertia_EmergencyOut(0.1f)
    , m_spikeInertia_Holster(0.5f)
    , m_spikeDetectionRadius_Emergency(3.f)
    , m_spikeBounciness(4.f)
    , m_hitWidthScale(0.5f)
    , m_rotationMargin(btrue, 5.f)
    , m_spikeAmplitude(0.05f)
    , m_spikeVibration_UpDuration(0.1f)
    , m_spikeVibration_DownDuration(0.5f)
    {}

    IMPLEMENT_OBJECT_RTTI(Ray_SpikyShellTrapComponent);
    BEGIN_SERIALIZATION_CHILD(Ray_SpikyShellTrapComponent);
    END_SERIALIZATION()

    ///////////////////////////////////////////////////////////////////////////////////////////
    Ray_SpikyShellTrapComponent::Ray_SpikyShellTrapComponent()
    : m_currentIndexCount(0)
    , m_currentVertexCount(0)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    Ray_SpikyShellTrapComponent::~Ray_SpikyShellTrapComponent()
    {
        if (m_texture.isValidResourceId()&& m_actor)
        {
            m_actor->removeResource(m_texture);
            m_texture.invalidateResourceId();
        }
        destroyIndexVertexBuffers();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyShellTrapComponent::destroyIndexVertexBuffers()
    {
        m_mesh.emptyIndexedMesh();
        if (m_mesh.m_ElementList.size() && (m_mesh.m_ElementList[0].m_indexBuffer!=NULL))
        {
            GFX_ADAPTER->removeIndexBuffer(m_mesh.m_ElementList[0].m_indexBuffer);
            m_mesh.m_ElementList[0].m_indexBuffer = NULL;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyShellTrapComponent::mayCreateVertexIndexBuffers()
    {
        //indices
        ITF_IndexBuffer *indexBuff = NULL;
        u32 indexCount = std::max((u32)m_elements.size(),(u32)1)*Ray_SpikyShellElement::m_indexCount;
        if (m_currentIndexCount!=indexCount)
        {
            if (m_mesh.m_ElementList.size() && m_mesh.m_ElementList[0].m_indexBuffer!=NULL)
            {
                GFX_ADAPTER->removeIndexBuffer(m_mesh.m_ElementList[0].m_indexBuffer);
            }
            m_currentIndexCount = indexCount;
            indexBuff = GFX_ADAPTER->createIndexBuffer(m_currentIndexCount, btrue);
        }
        else
        {
            indexBuff = m_mesh.m_ElementList[0].m_indexBuffer;
        }
        
        //vertices
        u32 vertexCount = std::max((u32)m_elements.size(),(u32)1)*Ray_SpikyShellElement::m_vertexCount;
        if (m_currentVertexCount!=vertexCount)
        {
            m_mesh.emptyIndexedMesh();
            m_currentVertexCount = vertexCount;
            m_mesh.initDynamicIndexedMesh(VertexFormat_PCT, indexBuff, m_currentVertexCount);
        }
        //Now, mesh is guarantied to be initialized
        m_mesh.m_ElementList[0].m_indexBuffer = indexBuff;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyShellTrapComponent::computeGround(Vec3d &_p1, Vec3d &_p2, Vec3d &_normal)
    {
        float width = m_actor->getScale().m_x;
        (Vec2d&)_p1 = Vec2d(-width,0.f).Rotate(m_actor->getAngle());
        (Vec2d&)_p2 = Vec2d(width,0.f).Rotate(m_actor->getAngle());
        _p1.m_z = 0;
        _p2.m_z = 0;

        _p1 += m_actor->getPos();
        _p2 += m_actor->getPos();

        (Vec2d&)_normal = Vec2d::Up.Rotate(m_actor->getAngle());
        _normal.m_z = 0;
    }

    //////////////////////////////////////////////////////////////////////////
    void Ray_SpikyShellTrapComponent::updateAABB()
    {
        Vec3d p1,p2, normal;

        ITF_ASSERT_MSG(m_actor, "Invalid actor");
        if (!m_actor)
            return;

        computeGround(p1,p2,normal);
        Vec3d p3,p4;
        Vec3d HeightVect = normal*getTemplate()->getMaxHeight();
        p3 = p1 + HeightVect + m_spikeOffset;
        p4 = p2 + HeightVect + m_spikeOffset;

        AABB aabb(p1);
        aabb.grow(p2);
        aabb.grow(p3);
        aabb.grow(p4);

        m_actor->growRelativeAABBFromAbsolute(aabb);
    }

    //////////////////////////////////////////////////////////////////////////
    void Ray_SpikyShellTrapComponent::Update( f32 _dt )
    {
        if ((m_lastPos-m_actor->getPos()).sqrnorm()>0.0001f)
        {
            populate(bfalse);
            m_lastPos = m_actor->getPos();
        }

        updateAABB();

        //1st compute rough detection AABB for filtering
        AABB detectionAABB = m_actor->getAABB();
        f32 detectionMargin = std::max(std::max(getTemplate()->getShakeDetectionRadius(), getTemplate()->getSpikeDetectionRadius()),
            getTemplate()->getEndOfSpikeDetectionRadius());

        if (!detectionAABB.isValid())
        {
            detectionAABB = AABB(m_actor->getPos());
        }
        Vec2d detectionMarginVect(detectionMargin,detectionMargin);

        detectionAABB.setMin(detectionAABB.getMin()-detectionMarginVect);
        detectionAABB.setMax(detectionAABB.getMax()+detectionMarginVect);

        //enumerate enemies
        static ActorList enemies_unfiltered;
        enemies_unfiltered.clear();
        AIUtils::getEnemies(getTemplate()->getFaction(),m_actor->getDepth(),enemies_unfiltered);
        static ActorList enemies;
        enemies.clear();
        for (u32 index=0; index<enemies_unfiltered.size(); index++)
        {
            Actor *enemy = enemies_unfiltered[index];
            if (enemy)
            {
                const AABB &enemyAABB = enemy->getAABB();
                if (enemyAABB.checkOverlap(detectionAABB))
                {
                    enemies.push_back(enemy);
                }            
            }
        }

        //
        f32 sqDetectionMargin = detectionMargin*detectionMargin;
        for (u32 elemIndex=0; elemIndex<m_elements.size(); elemIndex++)
        {
            f32 minSqDist = MTH_HUGE;
            const Vec2d &elemPos = (const Vec2d&) m_elements[elemIndex].m_pos;
            for (u32 enemyIndex=0; enemyIndex<enemies.size(); enemyIndex++)
            {
                Actor *enemy = enemies[enemyIndex];
                f32 sqDist = (enemy->get2DPos()-elemPos).sqrnorm();
                if (sqDist<minSqDist)
                    minSqDist = sqDist;
            }
            if (minSqDist<=sqDetectionMargin)
            {
                m_elements[elemIndex].signalNearestEnemy(f32_Sqrt(minSqDist));
            }
            else
            {
                m_elements[elemIndex].signalNearestEnemy(MTH_HUGE);
            }
        }                        

        //update
        static ActorList hitEnemies;
        hitEnemies.clear();
        for (u32 i=0; i<m_elements.size(); i++)
        {
            m_elements[i].updateCurrentState(_dt, enemies, hitEnemies);
        }
        //hit enemies
        for (u32 i=0; i<hitEnemies.size(); i++)
        {
            Actor *actor = hitEnemies[i];
            sendStim(actor, m_elements[0].m_normal);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyShellTrapComponent::sendStim(Actor *_victim, const Vec3d &_normal)
    {

        PhysShapeCircle circle(1.f); //fake shape : we already know our target

        PunchStim stim;

        stim.setReceivedHitType(RECEIVEDHITTYPE_HURTBOUNCE);
        stim.setFxPos(_victim->getPos());
        stim.setShape(&circle);
        stim.setDepth(m_actor->getDepth());
        stim.setSender(m_actor->getRef());
        stim.setLevel(getTemplate()->getHitLevel());


        //f32 angle = atan2f(_normal.m_y, _normal.m_x);
        stim.setDirection((const Vec2d&)_normal);
        stim.setAngle(m_actor->getAngle());
        Vec2d pos2d = _victim->get2DPos(); 
        stim.setPos(pos2d);
        stim.setPrevPos(pos2d);
        stim.setIsRadial(bfalse);
        stim.setFaction(getTemplate()->getFaction());
        stim.setSender(m_actor->getRef());
        _victim->onEvent(&stim);        
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyShellTrapComponent::Draw()
    {
        Texture *tex = (Texture*)m_texture.getResource();
        if (!tex)
            return;
        if (m_mesh.m_ElementList.size()==0)
            return;

        ITF_WARNING_CATEGORY(Graph, m_actor, tex->getUVAtlas()!=NULL, "Ray_SpikyShellElement : texture has no atlas");

        m_mesh.swapVBForDynamicMesh();
        ITF_VertexBuffer *vb = m_mesh.getCurrentVB();
        ITF_IndexBuffer *ib = m_mesh.m_ElementList[0].m_indexBuffer;

        VertexPCT *vertices = NULL;
        GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(vb, (void **) &vertices);

        u16 *indices = NULL;
        ib->Lock((void**)&indices);

        VertexPCT *endVertices = vertices + m_currentVertexCount;
        u16 *endIndices = indices+m_currentIndexCount;

        if (indices && vertices)
        {
            u16 indexOffset = 0;
            for (u32 i=0; i<m_elements.size(); i++)
            {
                m_elements[i].fillBuffers(tex,vertices, endVertices, indices, endIndices, indexOffset);
            }
        }

        GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(vb);
        ib->Unlock();

        m_mesh.m_ElementList[0].m_material.m_textureDiffuse = m_texture;
        m_mesh.m_ElementList[0].m_count = m_currentIndexCount;

        GFX_ADAPTER->addPrimitiveInZList(&m_mesh, GFX_ITF_MESH, NULL, m_actor->getDepth(), bfalse,m_actor->getRef());
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyShellTrapComponent::populate(bbool _initialize)
    {
        Vec3d p1,p2;
        Vec3d normal;

        u32 seed = ((u32)m_actor->getObjectId().m_details.m_second)<<8;
        m_seeder.reset(seed);
        if (_initialize)
        {
            m_elements.clear();
        }
        computeGround(p1,p2,normal);

        f32 baseWidth = getTemplate()->getBaseWidth();

        f32 groundSize = (p2-p1).norm();
        Vec3d dir = p2-p1;
        dir.normalize();

        Vec2d cosSin;
        Vec3d rotatedDir, rotatedNormal;

        u32 itemIndex = 0;
        for (f32 x = 0; x<(groundSize-baseWidth); )
        {
            if (_initialize)
            {
                Ray_SpikyShellElement tmp;            
                m_elements.push_back(tmp);               
            }

            f32 baseScale = m_seeder.GetFloat(getTemplate()->getMinScaleFactor(),1.f);

            if (itemIndex>=m_elements.size())
                break;

            Ray_SpikyShellElement &newElem = m_elements[itemIndex];

            f32 rotationMargin = getTemplate()->getRotationMargin();
            getApproxCosSin(m_seeder.GetFloat(-rotationMargin, rotationMargin),cosSin);
            rotateVector(rotatedDir, dir, cosSin);
            rotatedNormal = rotatedDir.getPerpendicular();

            Vec3d baseWidthVector(rotatedDir*baseWidth);
            Vec3d baseHeightVector(rotatedNormal*getTemplate()->getBaseHeight());
            Vec3d spikeWidthVector(rotatedDir*getTemplate()->getSpikeWidth());
            m_spikeOffset = rotatedNormal*getTemplate()->getSpikeYOffset();

            if (_initialize)
            {
                newElem.setup(
                    m_actor,
                    getTemplate(),
                    getTemplate()->getBackBaseTexIndex_Idle(),
                    getTemplate()->getFrontBaseTexIndex_Idle(),
                    getTemplate()->getSpikeFirstTexIndex(),
                    p1+dir*x, rotatedNormal, 
                    baseWidthVector*baseScale, baseHeightVector*baseScale, spikeWidthVector, 
                    m_spikeOffset*baseScale,
                    getTemplate()->getRestHeightPercent(), getTemplate()->getMaxHeight());            
            }
            else
            {
                newElem.setup_dontChangeState(
                    m_actor,
                    getTemplate(),
                    p1+dir*x, rotatedNormal, 
                    baseWidthVector*baseScale, baseHeightVector*baseScale, spikeWidthVector, 
                    m_spikeOffset*baseScale,
                    getTemplate()->getRestHeightPercent(), getTemplate()->getMaxHeight());            

            }

            f32 scaledBaseWidth = baseWidth*baseScale;
            f32 minSpacingPlusWidth = scaledBaseWidth+getTemplate()->getMinSpacing();
            f32 maxSpacingPlusWidth = scaledBaseWidth+getTemplate()->getMaxSpacing();

            x += m_seeder.GetFloat(minSpacingPlusWidth,maxSpacingPlusWidth);
            itemIndex++;

        }
        mayCreateVertexIndexBuffers();  
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyShellTrapComponent::onActorLoaded(Pickable::HotReloadType _hotReloadType)
    {
        Super::onActorLoaded(_hotReloadType);

        if ( !getTemplate()->getTexturePath().isEmpty() )
        {
            m_texture = m_actor->addResource(Resource::ResourceType_Texture, getTemplate()->getTexturePath());
        }
        m_lastPos = m_actor->getPos();
        populate();
        updateAABB();
    }

    //////////////////////////////////////////////////////////////////////////
    Ray_SpikyShellElement::Ray_SpikyShellElement()
        : m_state(State_Idle)
        , m_maxHeight(1)
        , m_restHeightPercent(0.1f)
        , m_backBaseTexIndex(0)
        , m_frontBaseTexIndex(0)
        , m_spikeTexIndex(0)
        , m_currentHeight(0.1f)
        , m_template(NULL)
        , m_timeInCurrentState(0)
        , m_spikeAnimationFrameTimeLeft(0)
        , m_baseAnimationTimeLeft(0)
        , m_speed(0)
        , m_soundFx(U32_INVALID)
        , m_actor(NULL)
        , m_nearestDistance(MTH_HUGE)

    {
    }

    Ray_SpikyShellElement::~Ray_SpikyShellElement()
    {
        playSound(StringID());
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyShellElement::setup(
        Actor *_act,
        const Ray_SpikyShellTrapComponent_Template *_template,
        u32 _backBaseTexIndex,
        u32 _frontBaseTexIndex,
        u32 _spikeTexIndex,
        const Vec3d &_pos, const Vec3d &_normal, 
        const Vec3d &_baseWidthVect, const Vec3d &_baseHeightVect, 
        const Vec3d &_spikeWidthVec, 
        const Vec3d &_spikeOffsetVec,
        f32 _restHeightPercent, f32 _maxHeight) 
    {
        setup_dontChangeState(_act, _template, _pos, _normal, _baseWidthVect, _baseHeightVect,
            _spikeWidthVec, _spikeOffsetVec, _restHeightPercent, _maxHeight);
        m_spikeAnimationFrameTimeLeft = m_template->getSpikeAnimationFirstFrameDuration();
        m_backBaseTexIndex = _backBaseTexIndex;
        m_frontBaseTexIndex = _frontBaseTexIndex;
        m_spikeTexIndex = _spikeTexIndex;
    }

    void Ray_SpikyShellElement::setup_dontChangeState(
        Actor *_act,
        const Ray_SpikyShellTrapComponent_Template *_template,
        const Vec3d &_pos, const Vec3d &_normal, 
        const Vec3d &_baseWidthVect, const Vec3d &_baseHeightVect, 
        const Vec3d &_spikeWidthVec, 
        const Vec3d &_spikeOffsetVec,
        f32 _restHeightPercent, f32 _maxHeight) 
    {
        m_template = _template;
        m_actor = _act;
        m_pos = _pos; m_normal = _normal; 
        m_baseWidthVect = _baseWidthVect; m_baseHeightVect = _baseHeightVect;
        m_restHeightPercent = _restHeightPercent; m_maxHeight = _maxHeight; 
        m_spikeWidthVec = _spikeWidthVec;
        m_spikeOffsetVec = _spikeOffsetVec;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyShellElement::playSound(const StringID &_id)
    {
        if (!m_actor)
            return;

        FXControllerComponent *fx = m_actor->GetComponent<FXControllerComponent>();
        if (!fx)
            return;

        if (U32_INVALID != m_soundFx)
        {
            fx->stopFX(_id);    
        }
        if (_id.isValid())
        {
            m_soundFx = fx->playFX(_id);
            fx->setFXPos(m_soundFx, m_pos);
        }

    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyShellElement::updateCurrentState(f32 _dt, const ActorList &_enemies, ActorList &_hitEnemies)
    {
        m_timeInCurrentState += _dt;
        //update according to state

        bbool testCollisions = bfalse;
        f32 targetHeight;

        f32 forceDuration =m_template->getSpikeInertia_Out();

        switch(m_state)
        {
        case State_Idle:
            targetHeight = m_template->getRestHeightPercent();
            forceDuration =m_template->getSpikeInertia_Holster();
            break;
        case State_Shaking: 
            targetHeight = m_template->getRestHeightPercent()
                +m_template->getShakeAmplitude()*f32_Sin(m_timeInCurrentState*m_template->getShakePulsation());
            testCollisions = btrue;
            forceDuration =m_template->getSpikeInertia_EmergencyOut();
            //Animation for base
            m_baseAnimationTimeLeft -= _dt;
            if (m_baseAnimationTimeLeft<0)
            {
                m_baseAnimationTimeLeft = m_template->getBaseAnimationFrameDuration();
                m_frontBaseTexIndex++;
                m_backBaseTexIndex++;
                if (m_frontBaseTexIndex>m_template->getFrontBaseLastTexIndex_Shaking())
                {
                    m_frontBaseTexIndex = m_template->getFrontBaseFirstTexIndex_Shaking();
                    m_backBaseTexIndex = m_template->getBackBaseFirstTexIndex_Shaking();
                }
            }
            break;
        case State_Spike:
        {
            f32 emergencyZoneWidth = m_template->getSpikeDetectionRadius()-m_template->getSpikeDetectionRadius_Emergency();
            if (emergencyZoneWidth<=0)
                emergencyZoneWidth = 1.f;

            f32 emergencyInterpolator = 1-(m_nearestDistance-m_template->getSpikeDetectionRadius_Emergency())/emergencyZoneWidth;
            emergencyInterpolator = f32_Clamp(emergencyInterpolator,0,1);
            if (m_timeInCurrentState>2)
                forceDuration = m_template->getSpikeInertia_EmergencyOut();
            else
                forceDuration = m_template->getSpikeInertia_Out()+(m_template->getSpikeInertia_EmergencyOut()-m_template->getSpikeInertia_Out())*emergencyInterpolator;
            f32 spikeOscillation = getAsymetricOscillation(m_timeInCurrentState,m_template->getSpikeVibration_UpDuration(),m_template->getSpikeVibration_DownDuration());
            spikeOscillation *= spikeOscillation;
            targetHeight = m_template->getRisenHeightPercent()+m_template->getSpikeAmplitude()*spikeOscillation;
            testCollisions = btrue; 
        } break;
        default: ITF_ASSERT_CRASH(bfalse, "unknown state"); targetHeight = 0; break;
        }

        //Spike movement : simple formula for reaching a position with a constant force for a given duration
        f32 frameCount = std::max(forceDuration/_dt,1.f);
        f32 sum_dt2 = frameCount * (frameCount - 1) * 0.5f * (_dt*_dt);
        float deltaH = targetHeight-m_currentHeight;

        f32 deltaScale = 1+m_template->getSpikeBounciness(); //was 5.f
        deltaH *= deltaScale;
        f32 acceleration = (deltaH-(frameCount*_dt)*m_speed)*(1.f/sum_dt2);

        f32 force = acceleration;

        m_speed += force*_dt;
        m_currentHeight += m_speed *_dt;
        m_currentHeight = f32_Clamp(m_currentHeight, 0,1);
        
        //collisions
        testCollisions = testCollisions || (m_currentHeight>(m_restHeightPercent+0.01f));
        if (testCollisions)
        {
            addVictimsToList(_enemies, _hitEnemies);
        }

        //spike animation
        m_spikeAnimationFrameTimeLeft -= _dt;
        if (m_spikeAnimationFrameTimeLeft<0)
        {
            m_spikeAnimationFrameTimeLeft = m_template->getSpikeAnimationFrameDuration();
            m_spikeTexIndex++;
            if (m_spikeTexIndex>m_template->getSpikeLastTexIndex())
            {
                m_spikeAnimationFrameTimeLeft = m_template->getSpikeAnimationFirstFrameDuration();
                m_spikeTexIndex = m_template->getSpikeFirstTexIndex();
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyShellElement::addVictimsToList(const ActorList &_enemies, ActorList &_hitEnemies)
    {
        static PhysShapePolygon poly;
        poly.clear();
        if (computeCollisionShape(poly))
        {
            PhysSweepInfo sweepInfo;
            PhysCollisionSolver::calculateSweepInfo(Vec2d::Zero, Vec2d::Zero, 0, &poly, sweepInfo);
            //We do a fast collision test among already filtered actors
            for (u32 i=0; i<_enemies.size(); i++)
            {
                Actor *enemy = _enemies[i];

                EventQueryPhysShape queryShape;
                enemy->onEvent(&queryShape);
                const PhysShape *shape = queryShape.getPhysShape();
                if (!shape)
                {
                    PhantomComponent *enemyPhantom = enemy->GetComponent<PhantomComponent>();
                    if (enemyPhantom)
                    {
                        shape = enemyPhantom->getShape();
                    }
                }
                
                if (shape)
                {
                    const Vec2d shapePos = enemy->get2DPos();

                    {
                        PhysSweepInfo enemySweepInfo;
                        static PhysContactsContainer contacts;
                        contacts.clear();

                        PhysCollisionSolver::calculateSweepInfo(shapePos,
                            shapePos,
                            enemy->getAngle(),
                            shape,
                            enemySweepInfo);

                        PhysCollisionSolver::collide(sweepInfo,enemySweepInfo,contacts);
                        if (contacts.size())
                        {
                            if (_hitEnemies.find(enemy)<0)
                            {
                                _hitEnemies.push_back(enemy);
                            }
                        }

                    }
                }
            }
        }
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyShellElement::signalNearestEnemy(f32 _distance)
    {
        m_nearestDistance = _distance;
        switch(m_state)
        {
        case State_Idle:
            {
                if (_distance<m_template->getShakeDetectionRadius())
                {
                    if (m_currentHeight>(m_template->getRestHeightPercent()+m_template->getShakeAmplitude()+0.1f/*epsilon*/))
                        setState(State_Spike);
                    else
                        setState(State_Shaking);
                }
            } break;
        case State_Shaking:
            {
                if (m_timeInCurrentState>m_template->getMinAlertDuration())
                {
                    if (_distance>m_template->getShakeDetectionRadius())
                    {
                        setState(State_Idle);
                    }
                    else if (_distance<=m_template->getSpikeDetectionRadius())
                    {
                        setState(State_Spike);
                    }
                }
            } break;

        case State_Spike:
            {
                if (m_timeInCurrentState>m_template->getMinSpikeDuration())
                {
                    if (_distance>m_template->getEndOfSpikeDetectionRadius())
                    {
                        setState(State_Idle);
                    }
                }
            } break;

        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyShellElement::setState(State_t _state)
    {
        State_t oldState = m_state;

        m_state = _state;
        m_timeInCurrentState = 0;
        switch(_state)
        {
        case State_Shaking:
            playSound(ITF_GET_STRINGID_CRC(FX_Vibrate,3226252396));
            m_backBaseTexIndex = m_template->getBackBaseFirstTexIndex_Shaking();
            m_frontBaseTexIndex = m_template->getFrontBaseFirstTexIndex_Shaking();
            m_baseAnimationTimeLeft = m_template->getBaseAnimationFrameDuration();
            break;
        case State_Spike:
            playSound(ITF_GET_STRINGID_CRC(FX_Stab,1815265994));
            m_backBaseTexIndex = m_template->getBackBaseTexIndex_Idle();
            m_frontBaseTexIndex = m_template->getFrontBaseTexIndex_Idle();
            break;
        case State_Idle:
            if (oldState==State_Spike)
                playSound(ITF_GET_STRINGID_CRC(FX_Holster,2153893064));
            else
                playSound(StringID());
            m_backBaseTexIndex = m_template->getBackBaseTexIndex_Idle();
            m_frontBaseTexIndex = m_template->getFrontBaseTexIndex_Idle();
            break;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_SpikyShellElement::computeCollisionShape(PhysShapePolygon &_poly)
    {
        Vec2d pointListBuffer[3];
        PhysShapePolygon::PointsList pointList(sizeof(pointListBuffer)/sizeof(Vec2d),MemoryId::mId_Temporary,pointListBuffer);

        f32 currentHeight = std::min(m_currentHeight, m_template->getRisenHeightPercent());
        f32 heightPercent = currentHeight - m_template->getHitMarginPercent();
        if (heightPercent<0.01f)
            return bfalse;

        Vec3d spikeLowLeft, heightVect;
        getSpikeGeometry(heightPercent, spikeLowLeft, heightVect);

        Vec2d spikeWidthVec2d(m_spikeWidthVec.m_x, m_spikeWidthVec.m_y);
        Vec2d spikeHalfWidthVec2d = spikeWidthVec2d * 0.5f;
        Vec2d scaledSpikeHalfWidth = spikeHalfWidthVec2d * m_template->gethitWidthScale();

        Vec2d spikeLowMid2d(spikeLowLeft.m_x+spikeHalfWidthVec2d.m_x, spikeLowLeft.m_y+spikeHalfWidthVec2d.m_y);


        //Vec2d spikeLowLeft2d(spikeLowLeft.m_x, spikeLowLeft.m_y);
        Vec2d heightVect2d(heightVect.m_x, heightVect.m_y);
        



        pointList.push_back(spikeLowMid2d+heightVect2d);
        pointList.push_back(spikeLowMid2d+scaledSpikeHalfWidth);
        pointList.push_back(spikeLowMid2d-scaledSpikeHalfWidth);

        _poly.setPoints(pointList);
        return btrue;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyShellElement::getSpikeGeometry(f32 _currentHeightPercent, Vec3d &_spikeLowLeftCorner, Vec3d &_heightVec)
    {
        _spikeLowLeftCorner = m_pos + (m_baseWidthVect - m_spikeWidthVec)*0.5f+m_spikeOffsetVec;
        _heightVec = m_normal*(m_maxHeight*_currentHeightPercent);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    void Ray_SpikyShellElement::fillBuffers(Texture *_tex, VertexPCT *&_destVertex, VertexPCT *_endVertex, u16 *&_destIndex, u16 *_endIndex, u16 &_indexOffset)
    {
        Vec2d atlasUVs[4];
        const f32 dz = 0.01f;

        const UVAtlas *atlas = _tex->getUVAtlas();        
        if (!atlas)
            return;

        u32 uvCount = atlas->getNumberUV();
        if (uvCount<2)
        {
            ITF_WARNING_CATEGORY(Graph, NULL, bfalse, "Ray_SpikyShellElement: atlas : incorrect UV count");
            return; //
        }

        static VertexPCT vertices[m_vertexCount];
        u16 indices[m_indexCount] = 
        {
            (u16)(0+_indexOffset), (u16)(1+_indexOffset), (u16)(3+_indexOffset), (u16)(1+_indexOffset), (u16)(2+_indexOffset), (u16)(3+_indexOffset),
            (u16)(4+_indexOffset), (u16)(5+_indexOffset), (u16)(7+_indexOffset), (u16)(5+_indexOffset), (u16)(6+_indexOffset), (u16)(7+_indexOffset),
            (u16)(8+_indexOffset), (u16)(9+_indexOffset), (u16)(11+_indexOffset), (u16)(9+_indexOffset), (u16)(10+_indexOffset), (u16)(11+_indexOffset),
        };

        ITF_ASSERT_CRASH((_destVertex+m_vertexCount)<=_endVertex, "buffer overflow for vertices");
        ITF_ASSERT_CRASH((_destIndex+m_indexCount)<=_endIndex, "buffer overflow for indices");

        atlas->get4UVbyIndex(m_backBaseTexIndex, atlasUVs);

        //back of base
        u32 color = 0xFFFFFFFF;
        VertexPCT *v=vertices;
        v->m_pos = m_pos+m_baseHeightVect;
        v->m_color = color;
        v->m_uv = atlasUVs[0];

        v++;

        v->m_pos = m_pos;
        v->m_color = color;
        v->m_uv = atlasUVs[1];
       
        v++;

        v->m_pos = m_pos+m_baseWidthVect;
        v->m_color = color;
        v->m_uv = atlasUVs[2];
        
        v++;

        v->m_pos = m_pos+m_baseWidthVect+m_baseHeightVect;
        v->m_color = color;
        v->m_uv = atlasUVs[3];

        v++;
        //spike
        atlas->get4UVbyIndex(m_spikeTexIndex, atlasUVs);
        Vec3d spikeLowLeft;
        Vec3d heightVect;
        getSpikeGeometry(m_currentHeight, spikeLowLeft, heightVect);
        spikeLowLeft.m_z += dz;

        Vec2d deltaUV((atlasUVs[1]-atlasUVs[0])*m_currentHeight);

        v->m_pos = spikeLowLeft+heightVect;
        v->m_color = color;
        v->m_uv = atlasUVs[0];    

        v++;

        v->m_pos = spikeLowLeft;
        v->m_color = color;
        v->m_uv = atlasUVs[0]+deltaUV;    

        v++;

        v->m_pos = spikeLowLeft+m_spikeWidthVec;
        v->m_color = color;
        v->m_uv = atlasUVs[3]+deltaUV;    

        v++;

        v->m_pos = spikeLowLeft+m_spikeWidthVec+heightVect;
        v->m_color = color;
        v->m_uv = atlasUVs[3];    

        v++;


        //front
        atlas->get4UVbyIndex(m_frontBaseTexIndex, atlasUVs);
        Vec3d posFront = m_pos;
        posFront.m_z += 2*dz;
        v->m_pos = m_pos+m_baseHeightVect;
        v->m_color = color;
        v->m_uv = atlasUVs[0];

        v++;

        v->m_pos = m_pos;
        v->m_color = color;
        v->m_uv = atlasUVs[1];

        v++;

        v->m_pos = m_pos+m_baseWidthVect;
        v->m_color = color;
        v->m_uv = atlasUVs[2];

        v++;

        v->m_pos = m_pos+m_baseWidthVect+m_baseHeightVect;
        v->m_color = color;
        v->m_uv = atlasUVs[3];

        v++;

        //Copy to GPU buffers

        ITF_MemcpyWriteCombined(_destVertex, vertices, (u8*)v-(u8*)vertices);
        ITF_MemcpyWriteCombined(_destIndex, indices, sizeof(indices));

        _destVertex += m_vertexCount;
        _destIndex += m_indexCount;
        _indexOffset += m_vertexCount;
    }

#ifdef ITF_SUPPORT_EDITOR
    //////////////////////////////////////////////////////////////////////////
    void Ray_SpikyShellElement::drawDebugInfo()
    {
        PhysShapePolygon poly;
        if (computeCollisionShape(poly))
        {
            DebugDraw::shape(Vec2d(0,0), m_pos.m_z, 0, &poly,
                Color::red(), 0.f, "phantom", 65 * MTH_DEGTORAD);
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyShellTrapComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        for (u32 i=0; i<m_elements.size(); i++)
        {
            ((Ray_SpikyShellElement&)m_elements[i]).drawDebugInfo();
        }

    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_SpikyShellTrapComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);
        populate();
        updateAABB();
    }
#endif // ITF_SUPPORT_EDITOR
}

