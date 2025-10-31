#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_DJEMBEFRIEZEPLUGIN_H_
#include "rayman/gameplay/Components/Misc/Ray_DjembeFriezePlugin.h"
#endif

#ifndef _ITF_RAY_GAMEMATERIAL_H_
#include "rayman/gameplay/Ray_GameMaterial.h"
#endif //_ITF_RAY_GAMEMATERIAL_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

namespace ITF
{
    #define VERYCOARSE 1
    #define COARSE 3
    #define FINE 5

    template<u32 xCountParam, u32 yCountParam>
    void Ray_DjembeFriezePlugin::generateTessellatedQuad(Frise::MeshStatic_VertexList& _vertexTempList, Frise::MeshStatic_IndexList &_indexTempList, Vec2d *_pos, Vec2d *_uv, u32 *_colors)
    {
        const u32 xCount = xCountParam;
        const u32 yCount = yCountParam;

        f32 alphas[4] = {Color(_colors[0]).getAlpha(), Color(_colors[1]).getAlpha(), Color(_colors[2]).getAlpha(), Color(_colors[3]).getAlpha()};

        Vec2d tPos[yCount+1][xCount+1]; //tessellated pos
        Vec2d tUV[yCount+1][xCount+1];
        u32 tColors[yCount+1][xCount+1];

        //We have to tessellate, because in some cases the alphas are not "coplanar"
        f32 yStep = 1.f/yCount;
        f32 xStep = 1.f/xCount;

        //compute interpolated grid
        for (u32 y=0; y<=yCount; y++)
        {
            f32 interpY = y*yStep;
            for (u32 x=0; x<=xCount; x++)
            {
                f32 interpX = x*xStep;
                f32 f0 = (1-interpX)*(1-interpY);
                f32 f1 = (interpX)*(1-interpY);
                f32 f2 = (1-interpX)*interpY;
                f32 f3 = interpX*interpY;

                tPos[y][x] = _pos[0]*f0 + _pos[1]*f1 + _pos[2]*f2+_pos[3]*f3;
                tUV[y][x] = _uv[0]*f0 + _uv[1]*f1 + _uv[2]*f2 + _uv[3]*f3;
                
                f32 alpha = alphas[0]*f0 + alphas[1]*f1 + alphas[2]*f2 + alphas[3]*f3;
                alpha *= alpha;
                tColors[y][x] = Color(alpha,1,1,1).getAsU32();
            }
        }

        //actually create points
        for (u32 y=0; y<yCount; y++)
            for (u32 x=0; x<xCount; x++)
            {        
                Vec2d subPos[4]={tPos[y][x],tPos[y][x+1],tPos[y+1][x],tPos[y+1][x+1]};
                Vec2d subUV[4]={tUV[y][x],tUV[y][x+1],tUV[y+1][x],tUV[y+1][x+1]};
                u32 subColors[4] = {tColors[y][x],tColors[y][x+1],tColors[y+1][x],tColors[y+1][x+1]};

                m_frieze->buildVB_Static_Quad( _vertexTempList, _indexTempList, subPos, subUV, subColors, bfalse );
            }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_DjembeFriezePlugin::buildFrieze_static(Frise::MeshStatic_VertexList& _vertexTempList, Frise::MeshStatic_IndexList* _indexTempList)
    {
        FriseConfig *config = m_frieze->getConfig();
        if (!config)
        {
            return;
        }
        const FriseConfigPluginSettings &settings = config->m_pluginSettings;

        const MeshElementRouter* elemRouter = m_frieze->getRoutingElement(MeshElementRouter::MeshElementTable_FlagDefault, 1);
        if (!elemRouter)
        {
            return;
        }
        if (elemRouter->m_tableID != MeshElementRouter::MeshElementTable_Static)
        {
            return;
        }

        f32 fadePercent = settings.getValue(ITF_GET_STRINGID_CRC(fadePercent, 2171839580), 10.f) * 0.01f;
        f32 widthMultiplier = settings.getValue(ITF_GET_STRINGID_CRC(widthMultiplier, 2896582045), 1.f);
        f32 height = settings.getValue(ITF_GET_STRINGID_CRC(height, 3204287464), 1.f);
        Vec2d xyOffset(settings.getValue(ITF_GET_STRINGID_CRC(xOffset, 1070778041), 0.f), settings.getValue(ITF_GET_STRINGID_CRC(yOffset, 1921449734),0.f));
        f32 alphaMultiplier = settings.getValue(ITF_GET_STRINGID_CRC(alphaMultiplier, 2878519527), 1.f);

        const SafeArray<PolyLine*>* pCollisionDataWorldSpace = m_frieze->getCollisionData_WorldSpace(); //game materials are only set in m_pFriezeComplexDatas->m_staticCollisionData
        const ITF_VECTOR<PolyPointList>* pCollisionDataLocalSpace = m_frieze->getStaticCollisionData_LocalSpace();
        
        if ( !pCollisionDataLocalSpace || !pCollisionDataWorldSpace ) 
        {
            return;
        }

        if (pCollisionDataLocalSpace->size() != pCollisionDataWorldSpace->size())
        {
            ITF_ASSERT_MSG(bfalse, "incorrect collisions");
            return;
        }

        Vec2d pos[4], uv[4];
        u32 colors[4];

        for (u32 polyLineIndex=0; polyLineIndex<pCollisionDataWorldSpace->size(); polyLineIndex++)
        {
            const PolyLine *pointList = (*pCollisionDataWorldSpace)[polyLineIndex];

            if (!pointList)
            {
                continue;
            }
            const PolyPointList &localCollisionPoly = (*pCollisionDataLocalSpace)[polyLineIndex];

            if (pointList->getVectorsCount()!=localCollisionPoly.getVectorsCount())
            {
                ITF_ASSERT_MSG(bfalse, "incorrect collisions");
                return;
            }
            for (u32 edgeIndex=0; edgeIndex<pointList->getVectorsCount(); edgeIndex++)
            {
                const PolyLineEdge& edge = pointList->getEdgeAt(edgeIndex);
                const StringID materialID = edge.getGameMaterial();
                Ray_GameMaterial_Template *material = IRTTIObject::SafeDynamicCast<Ray_GameMaterial_Template>((GameMaterial_Template*)World::getGameMaterial(materialID),ITF_GET_STRINGID_CRC(Ray_GameMaterial_Template,2777394804));

                if (material && material->getDjembe()>0)
                {

                    Vec2d pos1 = localCollisionPoly.getPosAt(edgeIndex);
                    Vec2d pos2 = localCollisionPoly.getPosAt(edgeIndex+1);
                    Vec2d dir = pos2-pos1;
                    dir.normalize();

                    Vec2d normal(dir.getPerpendicular());

                    Vec2d transformedOffset = dir*xyOffset.m_x + normal*xyOffset.m_y;

                    pos1 += transformedOffset;
                    pos2 += transformedOffset;
                    Vec2d centerPos = (pos1+pos2)*0.5f;

                    Vec2d halfExtent = (pos2-centerPos)*widthMultiplier;
                    pos1 = centerPos - halfExtent;
                    pos2 = centerPos + halfExtent;



                    Vec2d scaledNormal = normal*height;

                    Vec2d pos3 = pos1 + scaledNormal;
                    Vec2d pos4 = pos2 + scaledNormal;
                    Vec2d pos5 = pos3*fadePercent+pos1*(1-fadePercent);
                    Vec2d pos6 = pos4*fadePercent+pos2*(1-fadePercent);
                    Vec2d pos7 = pos6*fadePercent+pos5*(1-fadePercent);
                    Vec2d pos8 = pos5*fadePercent+pos6*(1-fadePercent);
                    Vec2d pos10 = pos2*fadePercent+pos1*(1-fadePercent);
                    Vec2d pos9 = pos1*fadePercent+pos2*(1-fadePercent);
                    Vec2d pos11 = pos4*fadePercent+pos3*(1-fadePercent);
                    Vec2d pos12 = pos3*fadePercent+pos4*(1-fadePercent);

                    

                    //          pos3 pos11 pos12 pos4
                    //           |               |
                    //           |               |
                    //          pos5 pos7  pos8 pos6
                    //           |               |
                    //          pos1 pos10 pos9 pos2
                    // EDGESTART=================EDGEEND

                    u32 opaque = Color(alphaMultiplier,1,1,1).getAsU32();
                    u32 transp = Color::zero().getAsU32();

                    //square1 : pos 1,pos10, pos5, pos7
                    pos[0] = pos1; uv[0].set(0,1); colors[0] = transp;
                    pos[1] = pos10; uv[1].set(fadePercent,1); colors[1] = transp;
                    pos[2] = pos5; uv[2].set(0,1-fadePercent); colors[2] = transp;
                    pos[3] = pos7; uv[3].set(fadePercent,1-fadePercent); colors[3] = opaque;                    
                    generateTessellatedQuad<COARSE,COARSE>( _vertexTempList, _indexTempList[elemRouter->m_indexInTable], pos, uv, colors);

                    //square2 : pos10 pos9 pos7 pos8
                    pos[0] = pos10; uv[0].set(fadePercent,1); colors[0] = transp;
                    pos[1] = pos9; uv[1].set(1-fadePercent,1); colors[1] = transp;
                    pos[2] = pos7; uv[2].set(fadePercent,1-fadePercent); colors[2] = opaque;
                    pos[3] = pos8; uv[3].set(1-fadePercent,1-fadePercent); colors[3] = opaque;                    
                    generateTessellatedQuad<COARSE,COARSE>( _vertexTempList, _indexTempList[elemRouter->m_indexInTable], pos, uv, colors );

                    //square3 : pos9 pos2 pos8 pos6(
                    pos[1] = pos9; uv[1].set(1-fadePercent,1); colors[1] = transp;
                    pos[0] = pos2; uv[0].set(1,1); colors[0] = transp;
                    pos[3] = pos8; uv[3].set(1-fadePercent, 1-fadePercent); colors[3] = opaque;
                    pos[2] = pos6; uv[2].set(1,1-fadePercent); colors[2] = transp;
                    generateTessellatedQuad<COARSE,COARSE>( _vertexTempList, _indexTempList[elemRouter->m_indexInTable], pos, uv, colors);

                    //square4 : pos5, pos7, pos3, pos11
                    pos[0] = pos5; uv[0].set(0, 1-fadePercent); colors[0] = transp;
                    pos[1] = pos7; uv[1].set(fadePercent,1-fadePercent); colors[1] = opaque;
                    pos[2] = pos3; uv[2].set(0, 0); colors[2] = transp;
                    pos[3] = pos11; uv[3].set(fadePercent, 0); colors[3] = transp;
                    generateTessellatedQuad<COARSE,FINE>( _vertexTempList, _indexTempList[elemRouter->m_indexInTable], pos, uv, colors);

                    //square5 : pos7 pos8 pos11 pos12
                    pos[0] = pos7; uv[0].set(fadePercent, 1-fadePercent); colors[0] = opaque;
                    pos[1] = pos8; uv[1].set(1-fadePercent,1-fadePercent); colors[1] = opaque;
                    pos[2] = pos11; uv[2].set(fadePercent, 0); colors[2] = transp;
                    pos[3] = pos12; uv[3].set(1-fadePercent, 0); colors[3] = transp;
                    generateTessellatedQuad<VERYCOARSE,FINE>( _vertexTempList, _indexTempList[elemRouter->m_indexInTable], pos, uv, colors);

                    //square6 : pos8 pos6 pos12 pos4
                    pos[1] = pos8; uv[1].set(1-fadePercent, 1-fadePercent); colors[1] = opaque;
                    pos[0] = pos6; uv[0].set(1,1-fadePercent); colors[0] = transp;
                    pos[3] = pos12; uv[3].set(1-fadePercent, 0); colors[3] = transp;
                    pos[2] = pos4; uv[2].set(1, 0); colors[2] = transp;
                    generateTessellatedQuad<COARSE,FINE>( _vertexTempList, _indexTempList[elemRouter->m_indexInTable], pos, uv, colors);


                }
            }
        }

    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_DjembeFriezePlugin::update(f32 _dt)
    {
        const FriseConfig *config = m_frieze->getConfig();
        if (!config)
        {
            return;
        }

        ITF_Mesh *staticMesh = m_frieze->getStaticMesh();
        if ( !staticMesh )
            return;

        const u32 elementCount = staticMesh->m_ElementList.size();
        if (elementCount!=2)
        {
            return;
        }

        ITF_MeshElement& elem = staticMesh->m_ElementList[elementCount-1];

        const FriseConfigPluginSettings &settings = config->m_pluginSettings;

        const f32 releaseAcceleration = settings.getValue(ITF_GET_STRINGID_CRC(releaseAcceleration, 3140772410), 0.5f);


        m_alpha += m_alphaSpeed*_dt;
        if (m_alpha>=1 && m_alphaSpeed>0)
            m_alphaSpeed = 0;
        m_alpha = f32_Clamp(m_alpha,0,1);

        m_alphaSpeed -=  releaseAcceleration*_dt;
        if (m_alphaSpeed<-10) m_alphaSpeed = -10;
            
        elem.m_material.m_matParams.setfParamsAt(2, m_alpha);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_DjembeFriezePlugin::onEvent(Event *_event)
    {
        FriseConfig *config = m_frieze->getConfig();
        if (!config)
        {
            return;
        }

        if (Ray_EventRegisterBubblePrizeHub* registerEvent = _event->DynamicCast<Ray_EventRegisterBubblePrizeHub>(ITF_GET_STRINGID_CRC(Ray_EventRegisterBubblePrizeHub,3108685932)))
        {
            ITF_WARNING(m_frieze, !m_bubblePrizeHub.isValid(), "Already linked to a bubble prize hub, not supported");
            if (!m_bubblePrizeHub.isValid())
            {
                m_bubblePrizeHub = registerEvent->getSender();
            }
        }
        else if (EventBlockedByPolyline *onStick = _event->DynamicCast<EventBlockedByPolyline>(ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768)))
        {
            PolyLine *poly = IRTTIObject::SafeDynamicCast<PolyLine>(onStick->getPolyline().getObject(),ITF_GET_STRINGID_CRC(PolyLine,1932163747));
            if (poly)
            {                
                processContact(poly, onStick->getEdge(), onStick->getActivator());
            }
        }
        else if (EventStickOnPolyline *onStick = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)))
        {
            if (onStick->isSticked())
            {
                PolyLine *poly = IRTTIObject::SafeDynamicCast<PolyLine>(onStick->getPolyline().getObject(),ITF_GET_STRINGID_CRC(PolyLine,1932163747));
                if (poly)
                {
                    processContact(poly, onStick->getEdge(), onStick->getActor());
                }        
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

     void Ray_DjembeFriezePlugin::processContact( PolyLine* _poly, u32 _edge, ActorRef _activator )
     {
         FriseConfig *config = m_frieze->getConfig();
         if (!config || !_poly)
         {
             return;
         }

         const FriseConfigPluginSettings &settings = config->m_pluginSettings;

             if (_edge != U32_INVALID)
             {
                 const PolyLineEdge &edge = _poly->getEdgeAt(_edge);
                 Ray_GameMaterial_Template *material = IRTTIObject::SafeDynamicCast<Ray_GameMaterial_Template>((GameMaterial_Template*)World::getGameMaterial(edge.getGameMaterial()),ITF_GET_STRINGID_CRC(Ray_GameMaterial_Template,2777394804));
                 if (material && material->getDjembe()>0)
                 {
                     const ITF_Mesh *staticMesh = m_frieze->getStaticMesh();
                     if ( !staticMesh )
                         return;

                     const u32 elementCount = staticMesh->m_ElementList.size();
                     if (elementCount!=2)
                     {
                         return;
                     }

                     {
                         f32 hitSpeed = settings.getValue(ITF_GET_STRINGID_CRC(hitSpeed, 3890303480), 2.f);
                         m_alphaSpeed = hitSpeed;
                         //ITF_MeshElement& elem = staticMesh.m_ElementList[elementCount-1];
                         //elem.m_material.m_uvAnim.m_UVMat.setIdentity();
                     }

                     // notify bubble prize hub
                     if (m_bubblePrizeHub.isValid())
                     {
                         Actor* hub = m_bubblePrizeHub.getActor();
                         ITF_WARNING(m_frieze, hub, "Can't get bubble prize hub");
                         if (hub)
                         {
                             // detect crush attack
                             bbool isCrushAttack = bfalse;

                             if (Actor* actor = _activator.getActor())
                             {
                                 Ray_EventQueryHitType query;
                                 actor->onEvent(&query);
                                 isCrushAttack = (query.getHitType()==PUNCHTYPE_CRUSH);
                             }

                             Ray_EventNotifyBubblePrizeHub notifyEvent;
                             notifyEvent.setSender(m_frieze->getRef());
                             notifyEvent.setCrushAttack(isCrushAttack);
                             hub->onEvent(&notifyEvent);
                         }
                     }
                 }
             }
        


     }

}

