#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_FISHSWARMAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_FishSwarmAIComponent.h"
#endif //_ITF_RAY_FISHSWARMAICOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#define PLAYER_SPEED_MIN 4.0f
#define PLAYER_SPEED_MAX 8.0f

//#define NO_FISH

    namespace ITF
    {
    
    IMPLEMENT_OBJECT_RTTI(Ray_FishSwarmAIComponent)
    
    BEGIN_SERIALIZATION_CHILD(Ray_FishSwarmAIComponent)
        SERIALIZE_MEMBER("widthZone",       m_widthZone);
        SERIALIZE_MEMBER("heightZone",      m_heightZone);
        SERIALIZE_MEMBER("frontColor",      m_frontColor);
        SERIALIZE_MEMBER("frontFogColor",      m_frontFogColor);
        SERIALIZE_MEMBER("backColor",      m_backColor);
        SERIALIZE_MEMBER("backFogColor",      m_backFogColor);
        SERIALIZE_FUNCTION(postLoadProperties, ESerialize_PropertyEdit_Load);        
    END_SERIALIZATION()


    BEGIN_VALIDATE_COMPONENT(Ray_FishSwarmAIComponent)
    END_VALIDATE_COMPONENT()

    //*****************************************************************************

    Ray_FishSwarmAIComponent::Ray_FishSwarmAIComponent()
    : m_fxControllerComponent(NULL)
    , m_trame(0)
    , m_time(0.0f)
    , m_countAtlasUv(0)
    , m_countMaxParticles(0)
    , m_widthZone(10.0f)
    , m_heightZone(10.0f)
    , m_globalIntensity(0.5f)
    , m_globalFrequency(2.0f)
    {
        m_frontColor = Color::white();
        m_frontFogColor = Color::zero();
        m_backColor = Color::white();
        m_backFogColor = Color::zero();
        // none
    }

    //*****************************************************************************

    Ray_FishSwarmAIComponent::~Ray_FishSwarmAIComponent()
    {
        // Clean atlas
        m_atlasSwarm.clear();
    }

    //*****************************************************************************

    void Ray_FishSwarmAIComponent::postLoadProperties()
    {
        // Clean atlas
        //m_atlasSwarm.clear();
        //init();
    }

    //*****************************************************************************

    void Ray_FishSwarmAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Compos
        m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();

        // Events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);

        // Init Texture Atlas
        m_atlasSwarm.createTexture(m_actor->getResourceGroup(), getTemplate()->getPathAtlas());
        m_atlasSwarmLayer2.createTexture(m_actor->getResourceGroup(), getTemplate()->getPathAtlas());

        Texture * texture = m_atlasSwarm.getAtlasTexture();
        if(texture)
        {
            const UVAtlas* uvAtlas = texture->getUVAtlas();

            if (uvAtlas)
            {
                m_countAtlasUv = uvAtlas->getNumberUV();

                m_listUV.clear();
                Vec2d* listUV = &m_listUV.incrSize(m_countAtlasUv * 4);
                for (u32 i = 0; i < m_countAtlasUv; i++)
                {
                    uvAtlas->get4UVAt(i, listUV+i*4);
                }
            }
        }

        // Init IA
        init();


    }

    //*****************************************************************************

    void Ray_FishSwarmAIComponent::Update( f32 _dt )
    {
#ifdef NO_FISH
        return;
#endif
        Super::Update(_dt);

        m_atlasSwarm.setColor(m_frontColor);
        m_atlasSwarm.setFogColor(m_frontFogColor);
        m_atlasSwarmLayer2.setColor(m_backColor);
        m_atlasSwarmLayer2.setFogColor(m_backFogColor);

        m_time += _dt;
        m_trame++;

        // Global sinus
        f32 sinY = m_globalIntensity * f32_Sin(m_time * m_globalFrequency);
        Vec3d initPos = m_actor->getBoundWorldInitialPos();
        m_actor->setPos(initPos + Vec3d(0.0f, sinY, 0.0f));

        updateObj(_dt);
        growAABB();
    }
   
    //*****************************************************************************

    void Ray_FishSwarmAIComponent::growAABB()
    {

        AABB aabb(m_actor->getPos());

        f32 xMin = m_actor->getPos().m_x - m_widthZone * 0.6f;
        f32 YMin = m_actor->getPos().m_y - m_heightZone * 0.6f;

        f32 xMax = m_actor->getPos().m_x + m_widthZone * 0.6f;
        f32 YMax = m_actor->getPos().m_y + m_heightZone * 0.6f;

        aabb.grow(Vec3d(xMin, YMin, 0.0f));
        aabb.grow(Vec3d(xMax, YMax, 0.0f));

         m_actor->growRelativeAABBFromAbsolute(aabb);
    }

    //*****************************************************************************

    void Ray_FishSwarmAIComponent::updateObj( f32 _dt )
    {
        ActorList playerList;
        AIUtils::getLivePlayers(playerList);
        const u32 countPlayer = playerList.size();

        // Update player speed
        SafeArray<PlayerInfo> playerInfoList;
        for(u32 p = 0; p < 4; p++)
        {
            PlayerInfo curPlayer;

            if (p <  countPlayer)
            {

                Actor* player = playerList[p];
                curPlayer.m_pos = player->get2DPos();

                if (StickToPolylinePhysComponent * physCompo = player->GetComponent<StickToPolylinePhysComponent>())
                {
                    curPlayer.m_speed = physCompo->getSpeed().norm();
                    curPlayer.m_dir = physCompo->getSpeed();
                    curPlayer.m_dir.normalize();
                }

                curPlayer.m_selector = 1.0f;

            }
            else
            {
                curPlayer.m_selector = 0.0f;
            }


            playerInfoList.push_back(curPlayer);

        }


        // Update Sinus
        const u32 countSin = m_listSin.size();
        for(u32 i = 0; i < countSin; i++)
        {
            SinusInfo& curSin = m_listSin[i];
            curSin.m_value = curSin.m_intensity * f32_Sin((curSin.m_dephase + m_time) * curSin.m_frequency);
        }


        const f32 radiusMinScale = getTemplate()->getRadiusFree();
        const f32 sqrRadiusMinScale = radiusMinScale * radiusMinScale;
        const f32 repulseForce = getTemplate()->getRepulseForce();
        const f32 repulseForceMax = getTemplate()->getRepulseForceMax();
        const u32 countTrameByAnim = getTemplate()->getCountTrameByAnimation();

        updateObjList(m_listObj, repulseForce, m_widthZone, m_heightZone, countPlayer, playerInfoList, sqrRadiusMinScale, repulseForceMax, countTrameByAnim, _dt);
        updateObjList(m_listObjZ, repulseForce, m_widthZone, m_heightZone, countPlayer, playerInfoList, sqrRadiusMinScale, repulseForceMax, countTrameByAnim, _dt);
    }

    //*****************************************************************************

    void Ray_FishSwarmAIComponent::init()
    {

        // Global
        const f32 globalIntensityMin = getTemplate()->getGlobalIntensityMin();
        const f32 globalIntensityMax = getTemplate()->getGlobalIntensityMax();
        const f32 globalFrequencyMin = getTemplate()->getGlobalFrequencyMin();
        const f32 globalFrequencyMax = getTemplate()->getGlobalFrequencyMax();

        m_globalIntensity = Seeder::getSharedSeeder().GetFloat(globalIntensityMin, globalIntensityMax);
        m_globalFrequency = Seeder::getSharedSeeder().GetFloat(globalFrequencyMin, globalFrequencyMax);

        // Clear 
        m_listObjZ.clear();
        m_listObj.clear();
        m_listSin.clear();

        // Density
        const f32 widthDensity = getTemplate()->getWidthDensity();
        const f32 heightDensity = getTemplate()->getHeightDensity();

        const u32 countColumns = (u32)(m_widthZone / widthDensity);
        const u32 countParticles = (u32)(m_heightZone / heightDensity);
        m_countMaxParticles = countColumns * countParticles;

        const f32 speedMin = getTemplate()->getSpeedMin();
        const f32 speedMax = getTemplate()->getSpeedMax();

        const f32 intensityMin = getTemplate()->getIntensityMin();
        const f32 intensityMax = getTemplate()->getIntensityMax();
        const f32 frequencyMin = getTemplate()->getFrequencyMin();
        const f32 frequencyMax = getTemplate()->getFrequencyMax();
        const f32 dephaseMin = getTemplate()->getDephaseMin();
        const f32 dephaseMax = getTemplate()->getDephaseMax();

        const u32 countAnimation = getTemplate()->getCountAnimation();
        const u32 countTrameByAnim = getTemplate()->getCountTrameByAnimation();


        ITF_ASSERT(intensityMin <= intensityMax);
        ITF_ASSERT(frequencyMin <= frequencyMax);
        ITF_ASSERT(dephaseMin <= dephaseMax);

        const f32 scaleMin = getTemplate()->getScaleMin();
        const f32 scaleMax = getTemplate()->getScaleMax();

        // Init sinus
        const u32  countSinus=getTemplate()->getCountSinus();
        m_listSin.reserve(countSinus);
        for(u32 i = 0; i < countSinus; i++)
        {
            SinusInfo curSin;

            curSin.m_intensity = Seeder::getSharedSeeder().GetFloat(intensityMin, intensityMax);
            curSin.m_frequency = Seeder::getSharedSeeder().GetFloat(frequencyMin, frequencyMax);
            curSin.m_dephase   = Seeder::getSharedSeeder().GetFloat(dephaseMin, dephaseMax);

            m_listSin.push_back(curSin);
        }

        // Offset X
        f32 addOffsetX  = m_widthZone / countColumns;
        f32 offsetX = -(m_widthZone * 0.5f);

        // Offset Y
        f32 addOffsetY  = m_heightZone / countParticles;
        f32 offsetY = -(m_heightZone * 0.5f);

        u32 index = 0;

        u32 countObjs = ((countColumns*countParticles)>>1)+1;
        m_listObjZ.reserve(countObjs);
        m_listObj.reserve(countObjs);

        for (u32 j = 0; j < countColumns; j++)
        {

            for (u32 i = 0; i < countParticles; i++)
            {
                SwarmObj newObj;

                // offset
                newObj.m_pos.m_x = (offsetX + addOffsetX * j);
                newObj.m_pos.m_y = (offsetY + addOffsetY * i);

                newObj.m_posTarget = newObj.m_pos;

                newObj.m_originalY = newObj.m_pos.m_y;

                newObj.m_fSpeed = Seeder::getSharedSeeder().GetFloat(speedMin, speedMax); 
                newObj.m_indexSinus = (u8)Seeder::getSharedSeeder().GetU32(0, m_listSin.size() - 1); 

                newObj.m_indexAnimation = (u8)Seeder::getSharedSeeder().GetU32(0, countAnimation - 1); 
                newObj.m_curIndexAtlas = (u8)Seeder::getSharedSeeder().GetU32(0, countTrameByAnim - 1); 

                newObj.m_scale = Seeder::getSharedSeeder().GetFloat(scaleMin, scaleMax); 

                if (index%2 == 0)
                {
                    newObj.m_dir = Vec2d::Right;
                    m_listObjZ.push_back(newObj);
                }
                else
                {
                    newObj.m_dir = -Vec2d::Right;
                    newObj.m_flipped = 1.0f;
                    m_listObj.push_back(newObj);
                }


                
                index++;

            }

        }

        u32 maxParticles = (u32)((m_countMaxParticles * 0.5f) + 100);

        m_atlasSwarm.initVertexBuffer(maxParticles);
        m_atlasSwarmLayer2.initVertexBuffer(maxParticles);

    }


    //*****************************************************************************

    void Ray_FishSwarmAIComponent::Draw()
    {
#ifdef NO_FISH
        return;
#endif
        Super::Draw();

        const f32 zActor =  m_actor->getDepth();
        const f32 widthSize = getTemplate()->getSizeWidthParticles();
        const f32 heightSize = getTemplate()->getSizeHeightParticles();
        const f32 semiWidthEdge = widthSize * 0.5f;
        const f32 semiHeightEdge = heightSize * 0.5f;
        const Vec2d actorPos = m_actor->get2DPos();

        drawObjList(m_listObj,actorPos, semiWidthEdge, semiHeightEdge, zActor, &m_atlasSwarm);
        drawObjList(m_listObjZ, actorPos, semiWidthEdge, semiHeightEdge, zActor, &m_atlasSwarmLayer2);
     
        m_atlasSwarm.drawVertexBuffer(zActor, m_listObj.size());
        m_atlasSwarmLayer2.drawVertexBuffer(zActor + getTemplate()->getZOffsetBetweenLayer(), m_listObjZ.size());

    }

    //*****************************************************************************

    void Ray_FishSwarmAIComponent::updateObjList( SwarmObjList & _listObj , const f32 repulseForce, const f32 zoneWidth, const f32 zoneHeight, const u32 countPlayer, SafeArray<PlayerInfo>& playerInfoList, const f32 sqrRadiusMinScale, const f32 repulseForceMax, const u32 countTrameByAnim, f32 _dt )
    {
        // Update animation trame ?
        u32 mod = m_trame%getTemplate()->getModAnimation();
        u8 addIndexAtlas = 0;
        if (mod == 0)
        {
            addIndexAtlas = 1;
        }

        const f32 smoothFactorMove = getTemplate()->getSmoothFactorMove();
        const f32 smoothFactorRotate = getTemplate()->getSmoothFactorRotate();
        
        const f32 semiZoneWidth = zoneWidth*0.5f;
        const f32 semiZoneHeight = zoneHeight*0.5f;

        const f32 diffForce = repulseForceMax - repulseForce;

        const f32 divSqrRadiusMinScale = 1.0f / sqrRadiusMinScale;
        const f32 divDiffSpeedPlayer = 1.0f /  (PLAYER_SPEED_MAX - PLAYER_SPEED_MIN);

        const u32 countObj = _listObj.size();
        const Vec2d& actorPos = m_actor->get2DPos();

        // For 4 players

        for (u32 i = 0; i < countObj; i++)
        {
            SwarmObj& curObj = _listObj[i];

            f32 repulseForceBySpeed = repulseForce;
            curObj.m_posDisplayPrev = curObj.m_posDisplay;

            // Update animation
            curObj.m_curIndexAtlas += addIndexAtlas;
            curObj.m_curIndexAtlas = curObj.m_curIndexAtlas%countTrameByAnim;
 
            // Update dir 
            const f32 right = f32_Sel(curObj.m_posTarget.m_x - semiZoneWidth,1.0f,-1.0f);
            const f32 left = f32_Sel(-curObj.m_posTarget.m_x - semiZoneWidth,1.0f,-1.0f);

            curObj.m_dir.m_x = f32_Sel(right,-1.0f,curObj.m_dir.m_x);
            //curObj.m_dir.m_y = f32_Sel(right,0.0f,curObj.m_dir.m_y);
            curObj.m_flipped = f32_Sel(right,1.0f,curObj.m_flipped);

            curObj.m_dir.m_x = f32_Sel(left,1.0f,curObj.m_dir.m_x);
            //curObj.m_dir.m_y = f32_Sel(left,0.0f,curObj.m_dir.m_y);
            curObj.m_flipped = f32_Sel(left,-1.0f,curObj.m_flipped);

            bbool isRepulse = bfalse;

            Vec2d actorPosToTargetPos = actorPos + curObj.m_posTarget;

            // Get repulse from players (Duplicate code for optim version)        
            // ***********************************************************************************************
            // Player 1
            // ***********************************************************************************************
            PlayerInfo& playerInfo1 = playerInfoList[0];
            const f32 p1Speed       = playerInfo1.m_speed;
            const f32 p1Selector    = playerInfo1.m_selector;
            const Vec2d& p1Pos      = playerInfo1.m_pos;

            Vec2d vecFromPlayer1 = actorPosToTargetPos - p1Pos;
            const f32 sqrDistFromPlayer1 = vecFromPlayer1.sqrnorm();

            const f32 ratioPenetration1 = 1.0f - (sqrDistFromPlayer1 * divSqrRadiusMinScale);                    
            const f32 ratioSpeed1 = Clamp((p1Speed - PLAYER_SPEED_MIN) * divDiffSpeedPlayer, 0.0f, 1.0f);

            vecFromPlayer1.normalize();

            repulseForceBySpeed =  repulseForce + (diffForce * ratioSpeed1);

            const f32 ratio1 = f32_Sel(sqrRadiusMinScale - sqrDistFromPlayer1,1.0f,0.0f);

            Vec2d forceRepulse = (vecFromPlayer1 * repulseForceBySpeed) * ratioPenetration1 * ratio1 * p1Selector;

            f32 isRepulseF1 = f32_Sel(sqrRadiusMinScale - sqrDistFromPlayer1, 1.0f, 0.0f);


            // ***********************************************************************************************
            // Player 2
            // ***********************************************************************************************
            PlayerInfo& playerInfo2 = playerInfoList[1];
            const f32 p2Speed       = playerInfo2.m_speed;
            const f32 p2Selector    = playerInfo2.m_selector;
            const Vec2d& p2Pos      = playerInfo2.m_pos;

            Vec2d vecFromPlayer2 = actorPosToTargetPos - p2Pos;
            const f32 sqrDistFromPlayer2 = vecFromPlayer2.sqrnorm();

            const f32 ratioPenetration2 = 1.0f - (sqrDistFromPlayer2 * divSqrRadiusMinScale);                    
            const f32 ratioSpeed2 = Clamp((p2Speed - PLAYER_SPEED_MIN) * divDiffSpeedPlayer, 0.0f, 1.0f);

            vecFromPlayer2.normalize();

            repulseForceBySpeed =  repulseForce + (diffForce * ratioSpeed2);

            const f32 ratio2 = f32_Sel(sqrRadiusMinScale - sqrDistFromPlayer2,1.0f,0.0f);

            forceRepulse += (vecFromPlayer2 * repulseForceBySpeed) * ratioPenetration2 * ratio2 * p2Selector;

            f32 isRepulseF2 = f32_Sel(sqrRadiusMinScale - sqrDistFromPlayer2, 1.0f, 0.0f);


            // ***********************************************************************************************
            // Player 3
            // ***********************************************************************************************
            PlayerInfo& playerInfo3 = playerInfoList[2];
            const f32 p3Speed       = playerInfo3.m_speed;
            const f32 p3Selector    = playerInfo3.m_selector;
            const Vec2d& p3Pos      = playerInfo3.m_pos;

            Vec2d vecFromPlayer3 = actorPosToTargetPos - p3Pos;
            const f32 sqrDistFromPlayer3 = vecFromPlayer3.sqrnorm();

            const f32 ratioPenetration3 = 1.0f - (sqrDistFromPlayer3 * divSqrRadiusMinScale);                    
            const f32 ratioSpeed3 = Clamp((p3Speed - PLAYER_SPEED_MIN) * divDiffSpeedPlayer, 0.0f, 1.0f);

            vecFromPlayer3.normalize();

            repulseForceBySpeed =  repulseForce + (diffForce * ratioSpeed3);

            const f32 ratio3 = f32_Sel(sqrRadiusMinScale - sqrDistFromPlayer3,1.0f,0.0f);

            forceRepulse += (vecFromPlayer3 * repulseForceBySpeed) * ratioPenetration3 * ratio3 * p3Selector;

            f32 isRepulseF3 = f32_Sel(sqrRadiusMinScale - sqrDistFromPlayer3, 1.0f, 0.0f);


            // ***********************************************************************************************
            // Player 4
            // ***********************************************************************************************
            PlayerInfo& playerInfo4 = playerInfoList[3];
            const f32 p4Speed       = playerInfo4.m_speed;
            const f32 p4Selector    = playerInfo4.m_selector;
            const Vec2d& p4Pos      = playerInfo4.m_pos;

            Vec2d vecFromPlayer4 = actorPosToTargetPos - p4Pos;
            const f32 sqrDistFromPlayer4 = vecFromPlayer4.sqrnorm();

            const f32 ratioPenetration4 = 1.0f - (sqrDistFromPlayer4 * divSqrRadiusMinScale);                    
            const f32 ratioSpeed4 = Clamp((p4Speed - PLAYER_SPEED_MIN) * divDiffSpeedPlayer, 0.0f, 1.0f);

            vecFromPlayer4.normalize();

            repulseForceBySpeed =  repulseForce + (diffForce * ratioSpeed4);

            const f32 ratio4 = f32_Sel(sqrRadiusMinScale - sqrDistFromPlayer4,1.0f,0.0f);

            forceRepulse += (vecFromPlayer4 * repulseForceBySpeed) * ratioPenetration4 * ratio4 * p4Selector;

            f32 isRepulseF4 = f32_Sel(sqrRadiusMinScale - sqrDistFromPlayer4, 1.0f, 0.0f);


            // Original force
            Vec2d forceDir = curObj.m_dir * curObj.m_fSpeed;

            // Repulse from up or Down
            const f32 up = f32_Sel(curObj.m_posTarget.m_y - semiZoneHeight,1.0f, 0.0f);
            const f32 down = f32_Sel(-curObj.m_posTarget.m_y - semiZoneHeight,1.0f, 0.0f);

            Vec2d forceBorder = (-Vec2d::Up * 10.0f) * up;
            forceDir = forceDir +  ((forceDir * 3.0f) * up);

            forceBorder += (Vec2d::Up * 10.0f) * down;
            forceDir = forceDir +  ((forceDir * 3.0f) * down);

            //if (curObj.m_pos.m_y >= semiZoneHeight)
            //{
            //    forceBorder = -Vec2d::Up * 10.0f;
            //    forceDir *= 3.0f;
            //}
            //else if (curObj.m_pos.m_y <= -semiZoneHeight)
            //{
            //    forceBorder = Vec2d::Up * 10.0f;
            //    forceDir *= 3.0f;
            //}

            Vec2d forceAlignment;

            // Clamp
            f32 maxSpeed = 0.0f;
            isRepulse = isRepulseF1 + isRepulseF2 + isRepulseF3 + isRepulseF4 > 0.0f;

            if (isRepulse)
            {
                maxSpeed = repulseForceBySpeed;
            }
            else
            {
                // Restore original position
                Vec2d ditToOriginY = Vec2d(curObj.m_pos.m_x, curObj.m_originalY) - curObj.m_pos;

                if (ditToOriginY.sqrnorm() > 0.01f) // TODO ?
                {
                    ditToOriginY.normalize();
                    forceAlignment = ditToOriginY * curObj.m_fSpeed;
                }             

                maxSpeed = curObj.m_fSpeed;
            }

            // Compute force
            curObj.m_speed += forceRepulse + forceDir + forceAlignment + forceBorder;


            // Clamp speed
            // optim version
            const f32 speedNorm = curObj.m_speed.norm();
            const f32 isValid = f32_Sel(speedNorm - MTH_EPSILON,1.0f,-1.0f);

            Vec2d normedSpeed(0.0f,0.0f);
            const f32 recSpeedNorm = f32_Inv(speedNorm);
            normedSpeed.m_x = f32_Sel(isValid,curObj.m_speed.m_x*recSpeedNorm,0.0f);
            normedSpeed.m_y = f32_Sel(isValid,curObj.m_speed.m_y*recSpeedNorm,0.0f);

            const f32 newNorm = f32_Sel(speedNorm - maxSpeed, maxSpeed, speedNorm);
            curObj.m_speed = normedSpeed * newNorm;

            // algo version
            //if (curObj.m_speed.sqrnorm() > maxSpeed * maxSpeed)
            //{
            //    curObj.m_speed = curObj.m_speed.normalize() * maxSpeed;
            //}


            curObj.m_posTarget += curObj.m_speed * _dt;
            curObj.m_pos = Interpolate(curObj.m_pos, curObj.m_posTarget, smoothFactorMove);

            // Compute SinY
            const f32 sinY =  m_listSin[curObj.m_indexSinus].m_value;
            curObj.m_posDisplay =  curObj.m_pos + (Vec2d::Up * sinY);

            // Orientation
            Vec2d speedDir = curObj.m_posDisplay - curObj.m_posDisplayPrev;
            speedDir.normalize();

            f32 speedAngle = speedDir.getAngle();
            curObj.m_angle += getShortestAngleDelta(curObj.m_angle, speedAngle) * smoothFactorRotate;
        }
    }

    //*****************************************************************************

    void Ray_FishSwarmAIComponent::drawObjList( SwarmObjList & _listObj, const Vec2d actorPos, const f32 semiWidthEdge,  const f32 semiHeightEdge, const f32 zActor,AtlasDynamicObject* atlas )
    {
        const u32 countObj = _listObj.size();

        if (!atlas->isValid())
            return;

        const u32 countTrameByAnim = getTemplate()->getCountTrameByAnimation();
        
        atlas->startFillVertexBuffer();
        ITF_VertexBuffer * vtxBuff = atlas->getCurrentVertexBuffer();
        const u32 color = Color::white().getAsU32();

        Vec2d decalPoint1(-semiWidthEdge, semiHeightEdge);
        Vec2d decalPoint2(-semiWidthEdge, -semiHeightEdge);
        Vec2d decalPoint3(semiWidthEdge, -semiHeightEdge);
        Vec2d decalPoint4(semiWidthEdge, semiHeightEdge);

        Vec2d p1, p2, p3, p4, curObjPos, cs, p1s, p2s, p3s, p4s;
        for (u32 i = 0; i < countObj; i++)
        {

            const SwarmObj& curObj = _listObj[i];

            //p1 ----- p4
            //|        |
            //|        |
            //p2 ----- p3

            curObjPos = actorPos + curObj.m_posDisplay;

            Vec2d::ScaleAdd(&p1, &decalPoint1, curObj.m_scale, &curObjPos);
            Vec2d::ScaleAdd(&p2, &decalPoint2, curObj.m_scale, &curObjPos);
            Vec2d::ScaleAdd(&p3, &decalPoint3, curObj.m_scale, &curObjPos);
            Vec2d::ScaleAdd(&p4, &decalPoint4, curObj.m_scale, &curObjPos);

            // Orientation
            const f32 angleRotate = f32_Sel(curObj.m_flipped, curObj.m_angle + MTH_PI, curObj.m_angle);

            Vec2d::CosSin(&cs, angleRotate);
            p1 = p1.RotateAroundCS(curObjPos, cs );
            p2 = p2.RotateAroundCS(curObjPos, cs );
            p3 = p3.RotateAroundCS(curObjPos, cs );
            p4 = p4.RotateAroundCS(curObjPos, cs );

            Vec2d::Sel(&p1s, curObj.m_flipped, &p1, &p4);
            Vec2d::Sel(&p4s, curObj.m_flipped, &p4, &p1);
            Vec2d::Sel(&p2s, curObj.m_flipped, &p2, &p3);
            Vec2d::Sel(&p3s, curObj.m_flipped, &p3, &p2);

            // index atlas
            u32 index = ((curObj.m_indexAnimation * countTrameByAnim) + curObj.m_curIndexAtlas) * 4;
  
            vtxBuff->wgp_write(color, zActor, p1s, m_listUV[index]);
            vtxBuff->wgp_write(color, zActor, p2s, m_listUV[index+1]);
            vtxBuff->wgp_write(color, zActor, p3s, m_listUV[index+2]);
            vtxBuff->wgp_write(color, zActor, p4s, m_listUV[index+3]);
        }
        atlas->endFillVertexBuffer();
    }

#ifdef ITF_SUPPORT_EDITOR
    //*****************************************************************************
    void Ray_FishSwarmAIComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        DebugDraw::text("FishSwarm : %d particles", m_countMaxParticles);
         

        ActorList playerList;
        AIUtils::getLivePlayers(playerList);

        for(u32 p = 0; p < playerList.size(); p++)
        {
            Vec3d playerPos = playerList[p]->getPos();

            DebugDraw::circle(playerPos, getTemplate()->getRadiusFree(), Color::green());
        }



    }

#endif // ITF_SUPPORT_EDITOR

    //*****************************************************************************
    // Ray_FishSwarmAIComponent::SwarmObj
    //*****************************************************************************

    Ray_FishSwarmAIComponent::SwarmObj::SwarmObj()
    : m_scale(1.0f)
    , m_flipped(-1.0f)
    , m_originalY(0.0f)
    , m_fSpeed(0.0f)
    , m_indexSinus(0)
    , m_indexAnimation(0)
    , m_curIndexAtlas(0)
    , m_angle(0.0f)
    {
        // none
    }

    //*****************************************************************************

    Ray_FishSwarmAIComponent::SwarmObj::~SwarmObj()
    {
        // none
    }

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_FishSwarmAIComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_FishSwarmAIComponent_Template)

        SERIALIZE_MEMBER("pathAtlas",         m_pathAtlas);
        SERIALIZE_MEMBER("zOffsetBetweenLayer", m_zOffsetBetweenLayer);

        SERIALIZE_MEMBER("widthDensity",      m_widthDensity);
        SERIALIZE_MEMBER("heightDensity",     m_heightDensity);

        SERIALIZE_MEMBER("speedMin",          m_speedMin);
        SERIALIZE_MEMBER("speedMax",          m_speedMax);
        SERIALIZE_MEMBER("radiusFree",        m_radiusFree);
        SERIALIZE_MEMBER("repulseForce",      m_repulseForce);
        SERIALIZE_MEMBER("repulseForceMax",   m_repulseForceMax);

        SERIALIZE_MEMBER("countSinus",        m_countSinus);

        SERIALIZE_MEMBER("intensityMin",      m_intensityMin);
        SERIALIZE_MEMBER("intensityMax",      m_intensityMax);
        SERIALIZE_MEMBER("frequencyMin",      m_frequencyMin);
        SERIALIZE_MEMBER("frequencyMax",      m_frequencyMax);
        SERIALIZE_MEMBER("dephaseMin",        m_dephaseMin);
        SERIALIZE_MEMBER("dephaseMax",        m_dephaseMax);

        SERIALIZE_MEMBER("sizeWidthParticles",    m_sizeWidthParticles);
        SERIALIZE_MEMBER("sizeHeightParticles",   m_sizeHeightParticles);

        SERIALIZE_MEMBER("scaleMin",          m_scaleMin);
        SERIALIZE_MEMBER("scaleMax",          m_scaleMax);
        
        SERIALIZE_MEMBER("countAnimation",          m_countAnimation);
        SERIALIZE_MEMBER("countTrameByAnimation",   m_countTrameByAnimation);

        SERIALIZE_MEMBER("modAnimation",   m_modAnimation);

        SERIALIZE_MEMBER("smoothFactorMove",    m_smoothFactorMove);
        SERIALIZE_MEMBER("smoothFactorRotate",  m_smoothFactorRotate);

        SERIALIZE_MEMBER("globalIntensityMin",  m_globalIntensityMin);
        SERIALIZE_MEMBER("globalIntensityMax",  m_globalIntensityMax);
        SERIALIZE_MEMBER("globalFrequencyMin",  m_globalFrequencyMin);
        SERIALIZE_MEMBER("globalFrequencyMax",  m_globalFrequencyMax);


    END_SERIALIZATION()

    //*****************************************************************************

    Ray_FishSwarmAIComponent_Template::Ray_FishSwarmAIComponent_Template()
    : m_speedMin(2.0f)
    , m_speedMax(4.0f)
    , m_radiusFree(1.0f)

    , m_repulseForce(4.0f)
    , m_repulseForceMax(8.0f)
    , m_countSinus(5)

    , m_intensityMin(0.0f)
    , m_intensityMax(0.5f)
    , m_frequencyMin(2.0f)
    , m_frequencyMax(2.0f)
    , m_dephaseMin(2.0f)
    , m_dephaseMax(5.0f)

    , m_sizeWidthParticles(1.0f)
    , m_sizeHeightParticles(1.0f)
    , m_scaleMin(1.0f)
    , m_scaleMax(1.0f)

    , m_zOffsetBetweenLayer(1.0f)

    , m_countAnimation(1)
    , m_countTrameByAnimation(1)

    , m_modAnimation(2)

    , m_widthDensity(1.0f)
    , m_heightDensity(1.0f)

    , m_smoothFactorMove(0.1f)
    , m_smoothFactorRotate(0.1f)

    , m_globalIntensityMin(0.5f)
    , m_globalIntensityMax(0.5f)
    , m_globalFrequencyMin(2.0f)
    , m_globalFrequencyMax(2.0f)

    {
        // none
    }

    //*****************************************************************************

    Ray_FishSwarmAIComponent_Template::~Ray_FishSwarmAIComponent_Template()
    {
        // none
    }

    //*****************************************************************************



}
