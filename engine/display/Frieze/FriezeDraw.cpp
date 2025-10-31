#include "precompiled_engine.h"

#ifndef _ITF_FRIEZE_DRAW_H_
#include "engine/display/Frieze/FriezeDraw.h"
#endif //_ITF_FRIEZE_DRAW_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FLUIDSHAPER_H_
#include "engine/display/Frieze/FluidShaper.h"
#endif //_ITF_FLUIDSHAPER_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

namespace ITF
{
    static Seeder randomWater(548);



        /*
                1__3 
                |\ | 
                |_\| 
                0  2 
        */

    ITF_INLINE void FluidPropage(FriseConfig *config, Vec2d &curHeight, Vec2d &nextHeight, f32 way, f32 _targetHeight)
    {
        curHeight.m_x =  curHeight.m_x*config->m_fluidElasticity + 
            (nextHeight.m_y - curHeight.m_y) +
            (_targetHeight - curHeight.m_y)*config->m_fluidViscosity;

        curHeight.m_y  += curHeight.m_x * config->m_fluidVelocity;
        nextHeight.m_x += (curHeight.m_x * config->m_fluidAmplification)*way; 
    }





    void  EdgeFluid::Draw(EdgeData *_data, const Vec2d& _pivot, ITF_VECTOR<FluidFriseLayer> &_layerInfos)
    {
        if (!m_heightLevelSet[0].size())
            return;

        u32   steps = m_lastIndex; //NB_FLUID_LEVELS - 1;
        Vec2d p0 = m_pos[0];
        Vec2d p1 = m_pos[1];
        Vec2d p2 = m_pos[2];
        Vec2d p3 = m_pos[3];
        
        if (_data->m_heightPos)
        {
            Vec2d::Lerp(&p1, &p0, &p1, _data->m_heightPos);
            Vec2d::Lerp(&p3, &p2, &p3, _data->m_heightPos);
        }

        Vec2d localArray[NB_FLUID_LEVELS + 1];

        Vec2d baseVect  = p2 - p0;
        Vec2d leftVect  = p1 - p0;
        Vec2d rightVect = p3 - p2;


        float deltaBase1 = (leftVect ).dot(baseVect)/(m_norm*m_norm);
        float deltaBase2 = (rightVect).dot(baseVect)/(m_norm*m_norm);
        float delta1 = deltaBase1/MESH_DEFAULT_VDIV;
        float delta2 = deltaBase2/MESH_DEFAULT_VDIV;

        f32 leftBorderHeight = 0, rightBorderHeight = 0;

        u32 layerCount = _layerInfos.size();
        for (u32 levIndex=0; levIndex<layerCount; levIndex++)
        {
            leftBorderHeight += m_heightLevelSet[levIndex][0].m_y;
            rightBorderHeight += m_heightLevelSet[levIndex][steps].m_y;
        }

        delta1 *= 1.f + leftBorderHeight;
        delta2 *= 1.f + rightBorderHeight;

        f32 yFactor =  _data->m_level;
        if (_data->m_way == 1)
            yFactor *= -1.f;

        const FluidColorMap &colorMap = m_fluidColorMap;
        ITF_ASSERT_CRASH(steps < NB_FLUID_LEVELS, "Too much fluid steps");
        for (u32 i=0; i<=steps; i++)
        {
            localArray[i].m_x = colorMap[i];
#if 0
            // reference code not using colorMap
            localArray[i].m_x = delta1 * (steps - i) + delta2 * i;
#endif
            f32 height = m_heightLevelSet[0][i].m_y;
            localArray[i].m_y = height*yFactor;
        }        

        for (u32 levIndex=1; levIndex<layerCount; levIndex++)
        {
            const FluidHeightMap &heightMap = m_heightLevelSet[levIndex];
            for (u32 i=0; i<=steps; i++)
            {
                f32 height = heightMap[i].m_y;
                localArray[i].m_y += height*yFactor;
            }
        }
        
        FixedArray<Vec2d, 4> uv = m_uv;
        Matrix44 matrix, matrixTmp;

        if (_data->m_way == 0)
        {
            M44_setMatrixRotationZ(&matrix, m_angle - MTH_PIBY2);
            matrix.mulScale( Vec3d( m_height*_data->m_heightScale, m_norm * MESH_DEFAULT_VDIV / steps, 1.f) );
            matrix.setTranslation( (p0 + Vec2d::Up.Rotate(m_angle)*m_height*_data->m_heightScale).to3d(m_depth) + getShift(_data->m_pos) );

            Vec2d deltaUV = _data->m_pos*m_deltaUV;
            for (u32 uvIdx=0; uvIdx<4; uvIdx++)
                uv[uvIdx] += deltaUV;
            
            if (_data->m_cutUV)
            {
                uv[1].m_y = (uv[1].m_y + uv[0].m_y)*0.5f;
                uv[3].m_y = (uv[3].m_y + uv[2].m_y)*0.5f;
            }
        } else
        {
            M44_setMatrixRotationZ(&matrix, - MTH_PIBY2 );
            M44_setMatrixRotationX(&matrixTmp, -MTH_PIBY2*_data->m_way);
            matrix *= matrixTmp;
            M44_setMatrixRotationZ(&matrixTmp, m_angle );
            matrix *= matrixTmp;
            matrix.mulScale( Vec3d( m_height*_data->m_heightScale, m_norm * MESH_DEFAULT_VDIV / steps, m_height) ); // perpedicular use m_height on Z too
            f32     depth   = m_depth + _data->m_posZ;
            Vec2d   pos     = p1 - baseVect * deltaBase1;

            matrix.setTranslation( pos.to3d(depth));

            if (_data->m_cutUV)
            {
                f32 uv1Y  = uv[1].m_y;
                f32 uv3Y  = uv[3].m_y;
                uv[1].m_y = (uv[1].m_y + uv[0].m_y)*0.5f;
                uv[3].m_y = (uv[3].m_y + uv[2].m_y)*0.5f;
                uv[0].m_y = uv1Y;
                uv[2].m_y = uv3Y;
            }
        }

        matrix.fa16[12] += _pivot.m_x;
        matrix.fa16[13] += _pivot.m_y;

        GFX_ADAPTER->setAlphaBlend((GFX_BLENDMODE)m_blendMode);
        GFX_ADAPTER->drawFluid((GMatrix44*)&matrix, m_texture, m_colors, localArray, steps+1, uv, _data->m_way, delta1, delta2, (f32)steps );
    }


    void  EdgeFluid::updateData(FriseConfig *config, u32 _mask)
    {
        if (_mask & EDGEFLUID_MAIN_DATA)
        {
            Vec2d sight = m_pos[2] - m_pos[0];
            m_norm = sight.norm();
            m_angle = atan2f(sight.m_y, sight.m_x);

            m_lastIndex = (u32)ceil(m_norm/config->m_fluidUnityWidth);
            if (m_lastIndex >= NB_FLUID_LEVELS)
                m_lastIndex = NB_FLUID_LEVELS-1;
            if (m_lastIndex < 1)
                m_lastIndex = 1;

            m_meshLevel = 8;
            while (m_meshLevel < m_lastIndex)
                m_meshLevel *= 2;
        }

        if (_mask & EDGEFLUID_VISUAL)
        {
            m_visualProcess.m_posStart  = m_pos[0] + ( m_pos[1] - m_pos[0] ) * config->m_visualOffset;
            m_visualProcess.m_posEnd    = m_pos[2] + ( m_pos[3] - m_pos[2] ) * config->m_visualOffset;

            m_visualProcess.m_count     = m_lastIndex / config->m_fluidPolylineUnityMult;
            if (m_visualProcess.m_count == 0) m_visualProcess.m_count = 1;
            m_visualProcess.m_posOffset = (m_visualProcess.m_posEnd - m_visualProcess.m_posStart) / (f32)m_visualProcess.m_count;
            m_visualProcess.m_normal    = m_visualProcess.m_posOffset.getPerpendicular();
            m_visualProcess.m_normal.normalize();
        }


        if (_mask & EDGEFLUID_COLLISION)
        {
            u32 sideCount = (config->m_methode == Frise::InString)? 2:1;


            for (u32 sideIndex=0; sideIndex<sideCount; sideIndex++)
            {
                EdgeProcessData &collisionProcess = m_collisionProcesses[sideIndex];

                f32 collisionOffset = config->m_collisionFrieze.m_offset;
                if (sideIndex==0)
                {
                    Vec2d::Lerp(&collisionProcess.m_posStart, &m_pos[0], &m_pos[1], collisionOffset);
                    Vec2d::Lerp(&collisionProcess.m_posEnd,   &m_pos[2], &m_pos[3], collisionOffset);
                }
                else
                {
                    Vec2d::Lerp(&collisionProcess.m_posEnd,   &m_pos[0], &m_pos[1], collisionOffset);
                    Vec2d::Lerp(&collisionProcess.m_posStart, &m_pos[2], &m_pos[3], collisionOffset);
                }

                collisionProcess.m_count     = m_lastIndex / config->m_fluidPolylineUnityMult;
                if (collisionProcess.m_count == 0)
                    collisionProcess.m_count = 1;
                collisionProcess.m_posOffset = (collisionProcess.m_posEnd - collisionProcess.m_posStart) / (f32)collisionProcess.m_count;
                collisionProcess.m_normal    = collisionProcess.m_posOffset.getPerpendicular();
                collisionProcess.m_normal.normalize();
            }
        }
    }

    void EdgeFluid::fillPolyline(PolyLine * polyline, AABBBuilder *aabb, u32 _mask, u32 _subIndex)
    {
        EdgeProcessData *procData = NULL;
        if (_mask & EDGEFLUID_VISUAL)
        {
            procData = &m_visualProcess;
            ITF_ASSERT(_subIndex==0);
        } else if (_mask & EDGEFLUID_COLLISION)
        {
            ITF_ASSERT(_subIndex<maxCollisionProcessCount);
            procData = &m_collisionProcesses[_subIndex];
        } else
            return;
        
        int count = procData->m_count - 1;
        if (count < 1) count = 1;
        Vec2d posCur = procData->m_posStart;
        for ( i32 i = 0; i < count; i++ )
        {
            polyline->addPoint( posCur );
            posCur += procData->m_posOffset;
        }
        if (_mask & EDGEFLUID_COLLISION)
            polyline->addPoint( procData->m_posEnd );

        if (aabb)
        {
            aabb->grow(procData->m_posStart);
            aabb->grow(procData->m_posEnd);
        }

    }

    void DataFluid::setFrieze(Frise * _frieze)
    {
        ITF_ASSERT(_frieze);
        if (_frieze)
        {
            m_frieze = _frieze->getRef();
        }
    }

    Frise * DataFluid::getFrieze()
    {
        Frise * frize = (Frise *)GETOBJECT(m_frieze);
        ITF_ASSERT_MSG(frize, "Data fluid with no Frieze ! Init problem ??");
        return frize;
    }

    DataFluid::~DataFluid()
    {
        deleteFluidShapers();
    }

    void DataFluid::deleteFluidShapers()
    {
        for (u32 layerIndex=0; layerIndex<MAX_FLUID_LAYER_COUNT; layerIndex++)
        {
            SafeArray<FluidShaper*> &layer = m_virtualPerturbers[layerIndex];
            for (u32 i=0; i<layer.size(); i++)
            {
                layer[i]->decrementRefCount();
            }
            layer.clear();
        }
        
    }

    void DataFluid::addStringWaveGenerator(ObjectRef _actorRef)
    {
        if (m_stringWaveGenerators.find(_actorRef)<0)
        {
            m_stringWaveGenerators.push_back(_actorRef);
        }
    }
    void DataFluid::removeStringWaveGenerator(ObjectRef _actorRef)
    {
        i32 index = m_stringWaveGenerators.find(_actorRef);
        if (index>=0)
        {
            m_stringWaveGenerators.eraseNoOrder(index);
        }
    }


    void DataFluid::addActor(ObjectRef _actorRef, const Vec2d& _speed, f32 _radius, f32 _waterMultiplier, f32 _weight, bbool _queryPosition)
    {
        Frise* frieze = getFrieze();
        if (!frieze)
            return;

        FriseConfig * config = frieze->getConfig();
        if (!config)
            return;

        Actor *actor = (Actor *)GETOBJECT(_actorRef);
        if (actor)
        {
            for (u32 i=0; i<m_contactActorsInfo.size(); i++)
            {
                if (m_contactActorsInfo[i].m_ref == _actorRef)
                {
                    ITF_ASSERT_MSG(0, "Actor %s is already in the frieze\nplease verify swimingOn/swimingOff", actor->getUserFriendly().cStr());
                    return;
                }
            }


            ActorInfo actorInfo;
            m_contactActorsInfo.push_back(actorInfo);

            ActorInfo *pActorInfo = &m_contactActorsInfo[m_contactActorsInfo.size() - 1];
            pActorInfo->m_ref             = _actorRef;
            // remove speed to process speed
            pActorInfo->m_prevPos         = actor->get2DPos();
            pActorInfo->m_radius          = _radius;
            pActorInfo->m_waterMultiplier = _waterMultiplier;
            pActorInfo->m_mass            = _weight;
            pActorInfo->m_queryPosition   = _queryPosition;

            // special influence on contact !
            pActorInfo->m_prevInfluence   = config->m_fluidEnterMult;

            if (!m_isCushion)
            {
                computeInfluence(actor->get2DPos(), _speed.norm()*_waterMultiplier,
                                 _radius, &pActorInfo->m_prevInfluence,
                                 1, bfalse,
                                 _weight);
            }
        }
    }

    void DataFluid::removeActor(ObjectRef _actorRef, const Vec2d& _speed)
    {
        Frise* frieze = getFrieze();
        if (!frieze)
            return;

        FriseConfig * config = frieze->getConfig();
        if (!config)
            return;

        u32 count = m_contactActorsInfo.size();
        for (u32 i=0; i<count; ++i)
        {
            ActorInfo &actorinfo = m_contactActorsInfo[i];
            if (actorinfo.m_ref == _actorRef)
            {
                Actor *actor = (Actor *)GETOBJECT(_actorRef);
                if (actor && !m_isCushion)
                {
                    actorinfo.m_prevInfluence   = config->m_fluidLeaveMult;

                    computeInfluence(actor->get2DPos(), _speed.norm()*actorinfo.m_waterMultiplier,
                                     actorinfo.m_radius, &actorinfo.m_prevInfluence, -1.f, bfalse,actorinfo.m_mass);
                }
                m_contactActorsInfo.eraseNoOrder(i);
                return;
            }
        }
    }

    void DataFluid::addFluidShaper(u32 _layerIndex, FluidShaper *_shaper)
    {
        Frise* frieze = getFrieze();
        if (!frieze)
            return;

        if (_layerIndex<m_layerInfos.size())
        {
            SafeArray<FluidShaper*> &perturbers = m_virtualPerturbers[_layerIndex];
            if (perturbers.find(_shaper)<0)
            {
                perturbers.push_back(_shaper);
                ITF_ASSERT(perturbers.size()<=10);
                _shaper->incrementRefCount();
            }
        }
    }

    void DataFluid::removeFluidShaper(u32 _layerIndex, FluidShaper *_shaper)
    {
        Frise* frieze = getFrieze();
        if (!frieze)
            return;

        if (_layerIndex<m_layerInfos.size())
        {
            i32 foundIndex = m_virtualPerturbers[_layerIndex].find(_shaper);
            if (foundIndex>=0)
            {
                m_virtualPerturbers[_layerIndex].eraseKeepOrder((u32)foundIndex);
                _shaper->decrementRefCount();
            }
        }
    }


    void DataFluid::sendPerturbation(const Vec2d& _pos, f32 _alteration, f32 _radius, bbool radiusRandom, f32 _mass)
    {
        f32 prevInfluence = 1.f;
        computeInfluence(_pos, _alteration, _radius, &prevInfluence, 1.f, radiusRandom, _mass);
    }



    void DataFluid::applyLocalInfluence(Frise *_frieze, FriseConfig *_config, f32 _deltaHeight, i32 _edgeIndex, i32 _referencePointIndex, i32 _startIndex, i32 _endIndex, i32 _indexStep, bbool _radiusRandom, i32 _influenceRadiusIndices)
    {
        for (i32 k = _startIndex; k !=_endIndex; k+=_indexStep )
        {
            int curIdx  = _edgeIndex;
            i32 kIn     = k;
            while (kIn < 0)
            {
                curIdx--;
                if (curIdx < 0)
                {
                    if (!_frieze->isLooping())
                    {
                        return;
                    }
                    curIdx = m_edgeFluidList.size()-1;
                }
                kIn += m_edgeFluidList[curIdx].m_lastIndex + 1;
            }
            while (kIn > (int)m_edgeFluidList[curIdx].m_lastIndex)
            {
                kIn -= m_edgeFluidList[curIdx].m_lastIndex + 1;
                curIdx++;
                if (curIdx >= (int)m_edgeFluidList.size())
                {
                    if (!_frieze->isLooping())
                    {
                        return;
                    }
                    curIdx = 0;
                }
            }

            if (kIn<(i32)m_edgeFluidList[curIdx].m_heightLevelSet[0].size())
            {
                if (_radiusRandom)
                    m_edgeFluidList[curIdx].m_heightLevelSet[0][kIn].m_y += randomWater.GetFloat(_deltaHeight);
                else
                {
                    f32 percentOfDistanceSQ = (k - _referencePointIndex)/(f32)_influenceRadiusIndices;
                    percentOfDistanceSQ *= percentOfDistanceSQ;
                    m_edgeFluidList[curIdx].m_heightLevelSet[0][kIn].m_y += _deltaHeight*(1-percentOfDistanceSQ);
                }
                if (m_edgeFluidList[curIdx].m_heightLevelSet[0][kIn].m_y > _config->m_fluidTargetMaxHeight)
                    m_edgeFluidList[curIdx].m_heightLevelSet[0][kIn].m_y = _config->m_fluidTargetMaxHeight;
                if (m_edgeFluidList[curIdx].m_heightLevelSet[0][kIn].m_y < _config->m_fluidTargetMinHeight)
                    m_edgeFluidList[curIdx].m_heightLevelSet[0][kIn].m_y = _config->m_fluidTargetMinHeight;
            }
        }
    }

    void DataFluid::computeInfluence(Vec2d _pos, f32 speedNorm, f32 _raidus, f32 * prevInfluence, f32 way, bbool _radiusRandom, f32 _mass)
    {
        Frise* frieze = getFrieze();
        if (!frieze)
            return;

        FriseConfig * config = frieze->getConfig();
        if (!config)
            return;

        _pos -= frieze->get2DPos();

        int delta = 0;
        if (_raidus > 0.f)
            delta = (int)ceil(_raidus*config->m_dstInfluenceMultiplier / config->m_fluidUnityWidth);
        if (delta == 0)
            delta = 1;

        // search "true" contact point between frize and actor -> min distance
        f32 minDst = MTH_HUGE;
        u32 minIdx = U32_INVALID;

        //Detecting horizontal cushion
        bbool isHorizontalCushion = bfalse;
        if (m_isCushion)
        {
            i32 edgeCount = (i32)frieze->getPosCount()-1;
            isHorizontalCushion = btrue;
            for (i32 i=0; i<edgeCount; i++)
            {
                const PolyLineEdge &edge = frieze->getEdgeAt((u32)i);
                if (fabsf(edge.m_normalizedVector.m_x)<0.995f)
                    isHorizontalCushion = bfalse;
            }
        }
        f32 horizontalCushion_EdgeRelative = 0;
        if (isHorizontalCushion)
        {
            //horizontal cushion for more accurate test
            for (u32 j=0; j<m_edgeFluidList.size(); j++)
            {
                const EdgeFluid &edgeFluid = m_edgeFluidList[j];
                f32 edgeStart=edgeFluid.m_pos[1].m_x;
                f32 edgeWidth = edgeFluid.m_pos[3].m_x - edgeStart;

                f32 edgeRelative = _pos.m_x-edgeStart;
                if (edgeWidth!=0)
                {
                    edgeRelative /= edgeWidth;
                    if (edgeRelative>=-0.001f && edgeRelative<=1.001f)
                    {
                        f32 dy = _pos.m_y-edgeFluid.m_pos[1].m_y;

                        f32 dist = f32_Abs(dy);
                        if (dist<minDst)
                        {
                            minDst = dist;
                            minIdx = j;
                            horizontalCushion_EdgeRelative = std::min(std::max(edgeRelative,0.f),1.f);
                        }
                    }

                }

            }
        }
        else
        {
            for (u32 j=0; j<m_edgeFluidList.size(); j++)
            {
                Vec2d edgeVectNorm  = m_edgeFluidList[j].m_pos[2] - m_edgeFluidList[j].m_pos[0];
                edgeVectNorm  = edgeVectNorm.normalize();
                Vec2d actorToEdge   = m_edgeFluidList[j].m_pos[2] - _pos;

                Vec2d contactPoint =  m_edgeFluidList[j].m_pos[2] - edgeVectNorm *(actorToEdge.dot(edgeVectNorm ));
                if (
                    (
                    ((contactPoint - m_edgeFluidList[j].m_pos[0]).dot(edgeVectNorm ) >= 0.f) && 
                    ((contactPoint - m_edgeFluidList[j].m_pos[2]).dot(edgeVectNorm ) <= 0.f)
                    )
                    )

                {
                    Vec2d actorToEdge2  = m_edgeFluidList[j].m_pos[3] - _pos;
                    Vec2d edgeVectNorm2 = m_edgeFluidList[j].m_pos[3] - m_edgeFluidList[j].m_pos[1];
                    edgeVectNorm2 = edgeVectNorm2.normalize();

                    f32 dst;
                    Vec2d v1 = (actorToEdge  - edgeVectNorm*(actorToEdge.dot(edgeVectNorm)));
                    Vec2d v2 = (actorToEdge2 - edgeVectNorm2*(actorToEdge2.dot(edgeVectNorm2)));
                    if (m_isCushion)
                    {
                        Vec2d upVec=edgeVectNorm2.getPerpendicular();
                        dst = actorToEdge2.dot(upVec);
                        if (dst<0)
                            dst = 0;
                    }
                    else
                    {
                        if (v1.dot(v2) < 0) // actor inside edge
                            dst = 0.f;
                        else
                            dst = Min(v1.norm(), v2.norm());
                    }
                    if (dst < minDst)
                    {
                        minIdx = j;
                        minDst = dst;
                    }
                }
            }
        }
        //compute influence from 
        f32 newInfluence = (config->m_fluidMaxDstInfluence - minDst)/config->m_fluidMaxDstInfluence;
        f32 dstInfluence = Max(newInfluence, *prevInfluence);
        *prevInfluence = newInfluence;

        if (minIdx != U32_INVALID && dstInfluence > 0.f)
        {
            //compute index of point in edge
            int     pointIdx;
            if (isHorizontalCushion)
            {
                pointIdx = (int)(horizontalCushion_EdgeRelative*m_edgeFluidList[minIdx].m_lastIndex);
            }
            else
            {
                Vec2d edgeVect      = m_edgeFluidList[minIdx].m_pos[2] - m_edgeFluidList[minIdx].m_pos[0];
                Vec2d edgeVectNorm  = edgeVect;
                edgeVectNorm  = edgeVectNorm.normalize();
                Vec2d actorToEdge   = m_edgeFluidList[minIdx].m_pos[2] - _pos;

                Vec2d   contactPoint  =  m_edgeFluidList[minIdx].m_pos[2] - edgeVectNorm *(actorToEdge.dot(edgeVectNorm ));
                pointIdx = (int)((contactPoint - m_edgeFluidList[minIdx].m_pos[0]).norm()/edgeVect.norm()*m_edgeFluidList[minIdx].m_lastIndex);
            }


            // compute deformation
            f32 val;
            if (m_isCushion)
            {
                float konst=-_mass*m_weightMultiplier;
                val = konst*dstInfluence;
            } else
            {
                val = config->m_fluidTargetAddHeight * speedNorm * dstInfluence * way;
            }
            f32 influenceLimit = std::max(config->m_fluidInfluenceLimit,0.f);
            if (val<-influenceLimit)
            {
                val = -influenceLimit;
            }
            else if (val>influenceLimit)
            {
                val = influenceLimit;
            }
            
            //apply
            delta = std::max(delta,2);
            applyLocalInfluence(frieze, config, val, minIdx, pointIdx, pointIdx,pointIdx+delta, 1, _radiusRandom, delta);
            applyLocalInfluence(frieze, config, val, minIdx, pointIdx, pointIdx-1,pointIdx-delta, -1, _radiusRandom, delta);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void DataFluid::setLayerCollisionHeightMultiplier(u32 _layerIndex, f32 _multiplier)
    {
        ITF_ASSERT(_layerIndex<m_layerInfos.size());
        if (_layerIndex<m_layerInfos.size())
        {
            m_layerInfos[_layerIndex].m_collisionHeightMultiplier = _multiplier;
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void DataFluid::resetAllLayerSettings()
    {
        for (u32 layerIndex=0; layerIndex<m_layerInfos.size(); layerIndex++)
        {
            setLayerCollisionHeightMultiplier(layerIndex, 1.f);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void DataFluid::processVirtualPerturbers(Frise *frieze, FriseConfig *_config)
    {
        //reinitialize colors
        for (u32 edgeIndex=0; edgeIndex<m_edgeFluidList.size(); edgeIndex++)
        {
            EdgeFluid &edgeFluid = m_edgeFluidList[edgeIndex];
            FluidColorMap &colorMap = edgeFluid.m_fluidColorMap;
            FluidHeightMap &heightMap = edgeFluid.m_heightLevelSet[0];
            FluidGameMaterialMap &gameMaterialMap = edgeFluid.m_fluidGameMaterialMap;

            if (heightMap.size()==0)
                continue;                

            u32 maxSize = edgeFluid.m_lastIndex;
            for (u32 i=0; i<=maxSize; i++)
            {
                colorMap[i]=0;
            }

            for (u32 i=0; i<=maxSize; i++)
            {
                gameMaterialMap[i]=0;
            }

        }

        //
        for (u8 layerIndex=0; layerIndex<_config->m_fluidLayerCount; layerIndex++)
        {
            SafeArray<FluidShaper*> &objectsInlayer = m_virtualPerturbers[layerIndex];

            for (u32 edgeIndex=0; edgeIndex<m_edgeFluidList.size(); edgeIndex++)
            {
                EdgeFluid &edgeFluid = m_edgeFluidList[edgeIndex];
                FluidHeightMap &heightMap = edgeFluid.m_heightLevelSet[layerIndex];
                FluidColorMap &colorMap = edgeFluid.m_fluidColorMap;
                FluidGameMaterialMap &gameMaterialMap = edgeFluid.m_fluidGameMaterialMap;
                if (heightMap.size()==0)
                    continue;                

                u32 maxSize = edgeFluid.m_lastIndex;
                Vec2d posScale = (edgeFluid.m_pos[2]-edgeFluid.m_pos[0])/(float)maxSize;
                Vec2d pos0 = edgeFluid.m_pos[0]+frieze->get2DPos();
                Vec2d posX = pos0;

                for (u32 i=0; i<=maxSize; (i++), (posX += posScale))
                {                    
                    f32 y=0;
                    u32 objectCount = objectsInlayer.size();
                    bbool foundAny = bfalse;
                    bbool modifyColor = bfalse;
                    bbool modifyGameMaterial = bfalse;
                    u8 presentLayerIndex=0;
                    f32 waveWeight = 0;
                    for (u32 perturbIndex = 0; perturbIndex<objectCount;perturbIndex++)
                    {
                        FluidShaper *perturber = objectsInlayer[perturbIndex];
                        bbool foundOne = bfalse;
                        f32 localWeight = 0;
                        bbool hasColor = bfalse;
                        bbool hasGMT = bfalse;
                        y += perturber->getHeight_2D(posX, foundOne,localWeight, hasColor, hasGMT);
                        if (foundOne)
                        {
                            foundAny = btrue;
                            modifyColor = modifyColor | hasColor;
                            modifyGameMaterial = modifyGameMaterial | hasGMT;
                            presentLayerIndex = layerIndex;
                            waveWeight = std::min(std::max(waveWeight, localWeight*2.f),1.f);

                        }
                    }
                    if (foundAny)
                    {
                        heightMap[i].m_y = heightMap[i].m_y*(1-waveWeight)+y*waveWeight;
                    }
                    if (modifyColor)
                    {
                        u8 &color = colorMap[i];
                        color = presentLayerIndex; //NB : WAS STD::MAX
                        if (modifyGameMaterial) //We can modify game material only if we modify color
                        {
                            gameMaterialMap[i] = presentLayerIndex;
                        }
                    }
                }

            }
        }
    }

    void DataFluid::ProcessEdgeFluidList_InString(f32 _elapsed)
    {
        if (m_edgeFluidList.size() == 0)
            return;

        Frise* frieze = getFrieze();
        if (!frieze)
            return;

        FriseConfig * config = frieze->getConfig();
        if (!config)
            return;

        updateUV(_elapsed);

        processVirtualPerturbers(frieze,config);

        checkElevationMapInitialization(config);

        checkActorInfluence();

        handleAbsorptionAtEdgeStartEnd_String(config);

        handlePropagation(frieze, config);

        updateCollisionPolyLine_InString(frieze, config);

        updateVisualPolyLine(frieze, config);

    }

    void DataFluid::checkElevationMapInitialization(FriseConfig *config)
    {
        u32 steps = NB_FLUID_LEVELS - 1;
        EdgeFluid *curEdgeFluid;

        for (u8 layerIndex=0; layerIndex<config->m_fluidLayerCount; layerIndex++)
        {
            if (!m_edgeFluidList[0].m_heightLevelSet[layerIndex].size()) // verify if init ok
            {
                f32 targetHeight = (layerIndex==0)? config->m_fluidTargetHeight:0.f;

                for (ITF_VECTOR<EdgeFluid>::iterator edgeIter = m_edgeFluidList.begin();
                    edgeIter != m_edgeFluidList.end(); ++edgeIter)
                {
                    curEdgeFluid = &(*edgeIter);
                    FluidHeightMap &curEdgeFluidHeightMap = curEdgeFluid->m_heightLevelSet[layerIndex];
                    FluidColorMap &curEdgeFluidColorMap = curEdgeFluid->m_fluidColorMap;
                    FluidGameMaterialMap &curEdgeFluidGMTMap = curEdgeFluid->m_fluidGameMaterialMap;
                    if (!curEdgeFluidHeightMap.size())
                    {
                        curEdgeFluidHeightMap.resize(steps+1);
                        curEdgeFluidColorMap.resize(steps+1);
                        curEdgeFluidGMTMap.resize(steps+1);

                        for (u32 i=0; i<steps; i++)
                        {
                            curEdgeFluidHeightMap[i].m_x = 0;
                            curEdgeFluidHeightMap[i].m_y = targetHeight;
                            curEdgeFluidColorMap[i] = 0;
                            curEdgeFluidGMTMap[i] = 0;
                        }
                        curEdgeFluidHeightMap[steps].m_x = 0;
                        curEdgeFluidHeightMap[steps].m_y = targetHeight;
                        curEdgeFluidColorMap[steps] = 0;
                        curEdgeFluidGMTMap[steps] = 0;
                    }
                }
            }
        }
    }

    void DataFluid::checkActorInfluence()
    {
        for (u32 i = 0; i < m_contactActorsInfo.size(); i++)
        {
            ActorInfo &actorInfo = m_contactActorsInfo[i];
            Actor * actor = (Actor *)GETOBJECT(actorInfo.m_ref);
            if (!actor)
                continue;
            f32 speedNorm = 0.0f;
            Vec2d pos;
            if (actorInfo.m_queryPosition)
            {
                EventQueryWaterInfluence query;
                query.setSender(m_frieze);
                actor->onEvent(&query);
                pos = query.getPos();

            }
            else
            {
                pos = actor->get2DPos();
            }
            speedNorm = (pos - actorInfo.m_prevPos).norm()/LOGICDT;
            actorInfo.m_prevPos = pos;

            if (speedNorm < MTH_EPSILON && !m_isCushion)
                continue;

            computeInfluence(pos, speedNorm*actorInfo.m_waterMultiplier,
                actorInfo.m_radius,  &actorInfo.m_prevInfluence,
                1.f,
                bfalse,
                actorInfo.m_mass);
        }
    }

    void DataFluid::handleAbsorptionAtEdgeStartEnd_String(FriseConfig *config)
    {
        if ((config->m_fluidAbsorptionAtEdgeStart==0 && config->m_fluidAbsorptionAtEdgeEnd==0) || (config->m_fluidAbsorptionAtEdge_Length<0) )
        {
            return;
        }

        EdgeFluid *curEdgeFluid=NULL;

        for (u32 layerIndex=0; layerIndex<config->m_fluidLayerCount; layerIndex++)
        {
            f32 targetHeight = (layerIndex==0)? config->m_fluidTargetHeight:0.f;

            EdgeFluid *prevEdge = NULL;

            u32 nextEdgeIndex;
            u32 edgeCount = m_edgeFluidList.size();
            nextEdgeIndex=1;

            for (ITF_VECTOR<EdgeFluid>::iterator edgeIter = m_edgeFluidList.begin();
                edgeIter < m_edgeFluidList.end(); ++edgeIter, ++nextEdgeIndex, prevEdge = curEdgeFluid)
            {
                curEdgeFluid = &(*edgeIter);
                EdgeFluid *nextEdge = NULL;
                if (nextEdgeIndex<edgeCount)
                {
                    nextEdge = &m_edgeFluidList[nextEdgeIndex];
                }

                FluidHeightMap &curEdgeHeightMap = curEdgeFluid->m_heightLevelSet[layerIndex];
                if (curEdgeHeightMap.size()==0)
                {
                    continue;
                }

                f32 edgeLen = (curEdgeFluid->m_pos[2]-curEdgeFluid->m_pos[0]).norm();
                if (edgeLen<MTH_EPSILON)
                {
                    continue;
                }

                
                f32 absorptionLenPercent = config->m_fluidAbsorptionAtEdge_Length/edgeLen;
                absorptionLenPercent = std::min(absorptionLenPercent,1.f);
                u32 absorptionLength = (u32)(curEdgeFluid->m_lastIndex*absorptionLenPercent);

                bbool hasPreviousEdge = (prevEdge!=NULL && curEdgeFluid->m_pos[0].IsEqual(prevEdge->m_pos[2], MTH_EPSILON));

                //absorption at start
                if (!hasPreviousEdge)
                {
                f32 absorptionAtStart = config->m_fluidAbsorptionAtEdgeStart;
                    f32 currentAbsorption = absorptionAtStart;
                    f32 absorptionStep = currentAbsorption/(f32)(absorptionLength+1);
                    for (u32 i=0; i<=absorptionLength; i++, currentAbsorption-=absorptionStep)
                {
                    f32 &height = curEdgeHeightMap[i].m_y;

                        height = height-(height-targetHeight)*currentAbsorption;
                    }
                }

                //absorption at end
                bbool hasNextEdge = (nextEdge!=NULL && curEdgeFluid->m_pos[2].IsEqual(nextEdge->m_pos[0], MTH_EPSILON));
                if (!hasNextEdge)
                {
                u32 lastIndex = curEdgeFluid->m_lastIndex;
                f32 absorptionAtEnd = config->m_fluidAbsorptionAtEdgeEnd;
                    f32 currentAbsorption = 0;
                    f32 absorptionStep = absorptionAtEnd/(f32)(absorptionLength+1);
                    for (u32 i=lastIndex-absorptionLength; i<=lastIndex; i++, currentAbsorption += absorptionStep)
                {
                    f32 &height = curEdgeHeightMap[i].m_y;

                        height = height-(height-targetHeight)*currentAbsorption;
                    }
                }
            }
        }
    }

    void DataFluid::handlePropagation(Frise *frieze, FriseConfig *config)
    {
        bbool isLooping = frieze->isLooping();

        EdgeFluid *curEdgeFluid;
        EdgeFluid *prevEdgeFluid;
        EdgeFluid *nextEdgeFluid;

        for (u32 layerIndex=0; layerIndex<config->m_fluidLayerCount; layerIndex++)
        {
            f32 targetHeight = (layerIndex==0)? config->m_fluidTargetHeight:0.f;

            for (ITF_VECTOR<EdgeFluid>::iterator edgeIter = m_edgeFluidList.begin();
                edgeIter < m_edgeFluidList.end(); ++edgeIter)
            {
                curEdgeFluid = &(*edgeIter);
                if (edgeIter != (m_edgeFluidList.end() - 1))
                    nextEdgeFluid = &(*(edgeIter+1));
                else if (isLooping)
                    nextEdgeFluid = &(*m_edgeFluidList.begin());
                else
                    nextEdgeFluid = NULL;

                if (nextEdgeFluid && !curEdgeFluid->m_pos[2].IsEqual(nextEdgeFluid->m_pos[0], MTH_EPSILON))
                    nextEdgeFluid = NULL;

                FluidHeightMap &curEdgeHeightMap = curEdgeFluid->m_heightLevelSet[layerIndex];

                for (u32 i=0; i < curEdgeFluid->m_lastIndex;i++) 
                { 
                    FluidPropage(config, curEdgeHeightMap[i],curEdgeHeightMap[i+1], 1.f, targetHeight);
                }
                if (nextEdgeFluid)
                {
                    FluidHeightMap &nextEdgeHeightMap = nextEdgeFluid->m_heightLevelSet[layerIndex];
                    FluidPropage(config, curEdgeHeightMap[curEdgeFluid->m_lastIndex],
                        nextEdgeHeightMap[1], 1.f,
                        targetHeight);
                    nextEdgeHeightMap[0] = curEdgeHeightMap[curEdgeFluid->m_lastIndex];
                } else
                {
                    FluidPropage(config, curEdgeHeightMap[curEdgeFluid->m_lastIndex],
                        curEdgeHeightMap[curEdgeFluid->m_lastIndex-1], -1.f, 
                        targetHeight);
                }
            }
        }

        for (u32 layerIndex=0; layerIndex<config->m_fluidLayerCount; layerIndex++)
        {
            f32 targetHeight = (layerIndex==0)? config->m_fluidTargetHeight:0.f;
            i32 last = (i32)m_edgeFluidList.size() - 1;
            for (i32 i=last; i >= 0; --i)
            {
                curEdgeFluid = &m_edgeFluidList[i];
                if (i > 0)
                    prevEdgeFluid = &m_edgeFluidList[i-1];
                else if (isLooping)
                    prevEdgeFluid = &m_edgeFluidList[last];
                else
                    prevEdgeFluid = NULL;            

                if (prevEdgeFluid && !curEdgeFluid->m_pos[0].IsEqual(prevEdgeFluid->m_pos[2], MTH_EPSILON))
                    prevEdgeFluid = NULL;            

                FluidHeightMap &curEdgeHeightMap = curEdgeFluid->m_heightLevelSet[layerIndex];

                for (i32 i=(i32)curEdgeFluid->m_lastIndex-1; i > 0; i--) 
                { 
                    FluidPropage(config, curEdgeHeightMap[i],
                        curEdgeHeightMap[i-1], 1.f,
                        targetHeight);
                }
                if (prevEdgeFluid)
                {
                    FluidHeightMap &prevEdgeHeightMap = prevEdgeFluid->m_heightLevelSet[layerIndex];
                    FluidPropage(config, curEdgeHeightMap[0],
                        prevEdgeHeightMap[prevEdgeFluid->m_lastIndex-1], 1.f,
                        targetHeight);
                    prevEdgeHeightMap[prevEdgeFluid->m_lastIndex] = curEdgeHeightMap[0];
                } else
                {
                    FluidPropage(config, curEdgeHeightMap[0],
                        curEdgeHeightMap[1], -1.f,
                        targetHeight);
                }
            }
        }

    }

    u8 DataFluid::getColorIndexAt_InString(const EdgeFluid *_edge, u32 _edgeIndex, i32 _posIndex) const
    {
        while (_posIndex<0)
        {
            if (_edgeIndex==0)
            {
                return 0;
            }
            _edgeIndex--;
            _edge = &m_edgeFluidList[_edgeIndex];
            _posIndex += (i32)_edge->m_lastIndex+1;
        }
        while (_posIndex>(i32)_edge->m_lastIndex)
        {
            u32 edgeCount = m_edgeFluidList.size();
            _posIndex -= (i32)_edge->m_lastIndex+1;
            _edgeIndex++;
            if (_edgeIndex>=edgeCount)
            {
                return 0;
            }
            _edge = &m_edgeFluidList[_edgeIndex];
        }
        return _edge->m_fluidColorMap[_posIndex];
    }

    u8 DataFluid::getGameMaterialIndexAt_InString(const EdgeFluid *_edge, u32 _edgeIndex, i32 _posIndex) const
    {
        while (_posIndex<0)
        {
            if (_edgeIndex==0)
            {
                return 0;
            }
            _edgeIndex--;
            _edge = &m_edgeFluidList[_edgeIndex];
            _posIndex += (i32)_edge->m_lastIndex+1;
        }
        while (_posIndex>(i32)_edge->m_lastIndex)
        {
            u32 edgeCount = m_edgeFluidList.size();
            _posIndex -= (i32)_edge->m_lastIndex+1;
            _edgeIndex++;
            if (_edgeIndex>=edgeCount)
            {
                return 0;
            }
            _edge = &m_edgeFluidList[_edgeIndex];
        }
        return _edge->m_fluidGameMaterialMap[_posIndex];
    }

    f32 DataFluid::computeFluidHeight_InString(FriseConfig *_config, EdgeFluid &_edge, u32 _edgeIndex, i32 _indexForCollision, u32 _sideIndex, StringID::StringIdValueType &_gameMaterialID)
    {
        f32 totalHeight = (_sideIndex==0)?0.f:0.2f;
        u32 layerCount = m_layerInfos.size();

        _gameMaterialID = _config->m_gameMaterial.getStringID().GetValue();

        for (u8 layerIndex=0; layerIndex<layerCount; layerIndex++)
        {
            f32 height = _edge.m_heightLevelSet[layerIndex][_indexForCollision].m_y;
            u8 col = getGameMaterialIndexAt_InString(&_edge, _edgeIndex, _indexForCollision);
            const FluidFriseLayer &layerInfos = m_layerInfos[layerIndex];

            if (col==layerIndex)
            {
                bbool isTooEroded = bfalse;
                bbool isABitEroded = bfalse;

                //Check erosion at this point
                if (layerInfos.m_erosion)
                {
                    i32 erosionStep = (i32)_config->m_fluidPolylineUnityMult;
                    i32 erosion = (i32)layerInfos.m_erosion*erosionStep+(erosionStep>>1);

                    u8 otherCol = getGameMaterialIndexAt_InString(&_edge, _edgeIndex, _indexForCollision-erosion);
                    if (otherCol!=col)
                    {
                        isABitEroded = btrue;
                    }
                    else
                    {
                        u8 otherCol = getGameMaterialIndexAt_InString(&_edge, _edgeIndex, _indexForCollision+erosion);
                        if (otherCol!=col)
                        {
                            isABitEroded = btrue;
                        }
                    }


                    {
                        for (i32 erosionDistance=-erosion+erosionStep; erosionDistance<0; erosionDistance+=erosionStep) 
                        {
                            u8 otherCol = getGameMaterialIndexAt_InString(&_edge, _edgeIndex, _indexForCollision+erosionDistance);
                            if (otherCol!=col)
                            {
                                isTooEroded = btrue;
                                break;
                            }
                        }
                        if (!isTooEroded)
                        {
                            for (i32 erosionDistance=erosionStep; erosionDistance<=(erosion-erosionStep); erosionDistance+=erosionStep) 
                            {
                                u8 otherCol = getGameMaterialIndexAt_InString(&_edge, _edgeIndex, _indexForCollision+erosionDistance);
                                if (otherCol!=col)
                                {
                                    isTooEroded = btrue;
                                    break;
                                }
                            }
                        }
                    }
                }
                
                //compute collision height and game material
                if (!(isABitEroded || isTooEroded))
                {
                    if (layerInfos.m_collisionHeight!=FluidFriseLayer::InvalidCollisionHeight) //special case for "Red" layer : it generates a collision height
                    {
                        height = layerInfos.m_collisionHeight * layerInfos.m_collisionHeightMultiplier;
                    }
                }
                else
                {
                    if (layerInfos.m_hasForcedHeightWhenNotColored)
                    {                    
                        height = layerInfos.m_forcedHeightWhenNotColored;
                    }
                }

                if (!isTooEroded)
                {
                    if (height>=layerInfos.m_threshold)
                    {
                        const StringID &layerGameMaterial = m_layerInfos[layerIndex].m_gameMaterialIds[_sideIndex];
                        if (layerGameMaterial.isValid())
                        {
                            _gameMaterialID = layerGameMaterial.GetValue();
                        }
                    }
                }
            }
            else
            {
                if (layerInfos.m_hasForcedHeightWhenNotColored)
                {                    
                    height = layerInfos.m_forcedHeightWhenNotColored;
                }
            }

            if (_sideIndex==1)
            {
                if (layerInfos.m_hasBacksideInversion) ///////TODO : REMOVE HARDCODED BEHAVIOR
                {
                    if (height>=0)
                    {
                        height=-height;
                    }
                    else
                    {
                        height = f32_Abs(height);
                    }
                }
                else
                {
                    height = f32_Abs(height);
                }
            }

            totalHeight += height;
        }
        return totalHeight;
    }
    

    void DataFluid::updateCollisionPolyLine_InString(Frise *frieze, FriseConfig *config)
    {
        // update polyline collision
        u32 edgeCount = m_edgeFluidList.size();
        for (u32 edgeIndex=0; edgeIndex<edgeCount; edgeIndex++)
        {
            EdgeFluid &edge = m_edgeFluidList[edgeIndex];

            u32 collisionProcessCount = config->m_fluidSideCount;
            for (u32 sideIndex = 0; sideIndex<collisionProcessCount; sideIndex++)
            {
                EdgeProcessData *procData = &edge.m_collisionProcesses[sideIndex];
                const SafeArray<PolyLine*>* pCollisionData = frieze->getCollisionData_WorldSpace();

                if ( procData->m_id >= 0 && pCollisionData && (*pCollisionData)[procData->m_id] )
                {
                    PolyLine        *poly       = (*pCollisionData)[procData->m_id];
                    i32             indexForCollision, stepForCollisionIndex;
                    bbool           somethingChanged = btrue; /////////////

                    u32 lastPointIndex = poly->getPosCount() - 1;

                    u32 pointIndex;
                    poly->offsetData(-frieze->get2DPos());

                    Vec2d           posCur      = procData->m_posStart;
                    if (sideIndex==1) //back side
                    {
                        indexForCollision = ((i32)lastPointIndex-1)*config->m_fluidPolylineUnityMult;
                        stepForCollisionIndex = -(i32)config->m_fluidPolylineUnityMult;
                        posCur = procData->m_posEnd - ((f32)lastPointIndex-1)*procData->m_posOffset;
                        pointIndex = 1;
                    }
                    else
                    {
                        indexForCollision = 0;
                        pointIndex = 0;
                        stepForCollisionIndex = config->m_fluidPolylineUnityMult;
                    }


                    for (u32 i=0; i<lastPointIndex; i++, (posCur+=procData->m_posOffset), (indexForCollision+= stepForCollisionIndex), (pointIndex++))
                    {
                        Vec2d Pos = posCur;
                        
                        StringID::StringIdValueType gameMaterialID;
                        f32 totalHeight = computeFluidHeight_InString(config, edge, edgeIndex, indexForCollision, sideIndex, gameMaterialID);

                        Pos += procData->m_normal*totalHeight;

                        const Vec2d oldPos = poly->getPosAt(pointIndex);
                        poly->setPosAt( Pos, pointIndex );
                        if (!oldPos.IsEqual(Pos,0.001f))
                        {
                            somethingChanged = btrue;
                        }
                        poly->setGameMaterialAt(gameMaterialID,pointIndex);
                    }

                    StringID::StringIdValueType finalGameMaterialID;
                    f32 finalHeight = computeFluidHeight_InString(config, edge, edgeIndex, edge.m_lastIndex, sideIndex, finalGameMaterialID);                    

                    if (somethingChanged)
                    {
                        u32 boundActorCount = m_contactActorsInfo.size();
                        for (u32 boundActorIndex = 0; boundActorIndex<boundActorCount; boundActorIndex++)
                        {
                            ObjectRef objectRef(m_contactActorsInfo[boundActorIndex].m_ref);
                            BaseObject *object = objectRef.getObject();
                            if (object)
                            {
                                Actor *actor = object->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
                                if (actor)
                                {
                                    EventFluidCollisionPolylineChanged fluidEvent;
                                    fluidEvent.setSender(m_frieze);
                                    actor->onEvent(&fluidEvent);
                                }
                            }
                        }
                    }

                    if (sideIndex==0)
                    {
                        Vec2d Pos = procData->m_posEnd + procData->m_normal*finalHeight;
                        u32 finalPointIndex = lastPointIndex;
                        poly->setPosAt( Pos, finalPointIndex );
                    }
                    else
                    {
                        Vec2d Pos = procData->m_posStart + procData->m_normal*finalHeight;
                        u32 finalPointIndex = 0;
                        poly->setPosAt( Pos, finalPointIndex );
                    }

                    poly->offsetData(frieze->get2DPos());

                    PolyLine *previous = poly->getPrevious();
                    if (previous)
                    {
                        if (previous->getPosCount() && poly->getPosCount())
                        {
                            const Vec2d &endOfPrevious = previous->getPosAt(previous->getPosCount()-1);
                            poly->setPosAt(endOfPrevious,0);
                        }
                    }


                    // We are SURE that polyine is NOT a frise
                    poly->forceRecomputeData();
                }
            }
        }
    }

    void DataFluid::updateCollisionPolyLine_InFluid(Frise *frieze, FriseConfig *config)
    {
        // update polyline collision
        for (ITF_VECTOR<EdgeFluid>::iterator edgeIter = m_edgeFluidList.begin();
            edgeIter < m_edgeFluidList.end(); ++edgeIter)
        {
            EdgeFluid &edge = *edgeIter;
            const SafeArray<PolyLine*>* pCollisionData = frieze->getCollisionData_WorldSpace();

            if  ( edge.m_collisionProcesses[0].m_id >= 0 && pCollisionData && (*pCollisionData)[edge.m_collisionProcesses[0].m_id] )
            {
                EdgeProcessData *procData   = &(edge.m_collisionProcesses[0]);
                Vec2d           posCur      = procData->m_posStart;
                PolyLine        *poly       = (*pCollisionData)[edge.m_collisionProcesses[0].m_id];
                u32 polyCount = poly->getPosCount() - 1;

                poly->offsetData(-frieze->get2DPos());
                u32 layerCount = config->m_fluidLayerCount;
                u32 indexForCollision = 0;
                for (u32 i=0; i<polyCount; i++, (posCur+=procData->m_posOffset), (indexForCollision+= config->m_fluidPolylineUnityMult))
                {
                    Vec2d Pos = posCur;
                    StringID::StringIdValueType gameMaterialID = config->m_gameMaterial.getStringID().GetValue();
                    for (u32 layerIndex=0; layerIndex<layerCount; layerIndex++)
                    {
                        const Vec2d &height = edge.m_heightLevelSet[layerIndex][indexForCollision];
                        Pos += procData->m_normal*height;
                        f32 col = edge.m_fluidGameMaterialMap[indexForCollision];
                        const FluidFriseLayer &layerInfos = m_layerInfos[layerIndex];
                        if ((col>=(f32)layerIndex) && (height.m_y>=layerInfos.m_threshold))
                        {
                            const StringID &layerGameMaterial = m_layerInfos[layerIndex].m_gameMaterialIds[0];
                            if (layerGameMaterial.isValid())
                            {
                                gameMaterialID = layerGameMaterial.GetValue();
                            }
                        }
                    }
                    poly->setPosAt( Pos, i );
                    poly->setGameMaterialAt(gameMaterialID,i);


                }
                f32 finalHeight = 0;
                for (u32 layerIndex=0; layerIndex<layerCount; layerIndex++)
                {
                    finalHeight += edge.m_heightLevelSet[layerIndex][edge.m_lastIndex].m_y;
                }
                Vec2d Pos = procData->m_posEnd + procData->m_normal*finalHeight;
                poly->setPosAt( Pos, polyCount );
                if (poly->isLooping())
                    poly->setPosAt( poly->getPosAt(0), poly->getPosCount() - 1 );

                poly->offsetData(frieze->get2DPos());

                PolyLine *previous = poly->getPrevious();
                if (previous)
                {
                    if (previous->getPosCount() && poly->getPosCount())
                    {
                        const Vec2d &endOfPrevious = previous->getPosAt(previous->getPosCount()-1);
                        poly->setPosAt(endOfPrevious,0);
                    }
                }

                // We are SURE that polyine is NOT a frise
                poly->forceRecomputeData();
            }
        }

    }

    void DataFluid::updateVisualPolyLine(Frise *frieze, FriseConfig *config)
    {
        // update polyline visual
        ObjectRef visualPolyRef = frieze->getVisualPoly();
        PolyLine * visualPoly = (PolyLine *)visualPolyRef.getObject();
        if (visualPolyRef != frieze->getRef() && visualPoly)
        {
            visualPoly->offsetData(-frieze->get2DPos());
            for (ITF_VECTOR<EdgeFluid>::iterator edgeIter = m_edgeFluidList.begin();
                edgeIter < m_edgeFluidList.end(); ++edgeIter)
            {
                EdgeProcessData *procData   = &((*edgeIter).m_visualProcess);
                Vec2d           posCur      = procData->m_posStart;
                u32 layerCount = config->m_fluidLayerCount;

                for (i32 i=procData->m_indexStart; i<=procData->m_indexEnd; i++, posCur += procData->m_posOffset)
                {
                    f32 finalHeight = 0;
                    for (u32 layerIndex=0; layerIndex<layerCount; layerIndex++)
                    {
                        finalHeight += (*edgeIter).m_heightLevelSet[layerIndex][(i-procData->m_indexStart)*config->m_fluidPolylineUnityMult].m_y;
                    }
                    Vec2d Pos = posCur + procData->m_normal*finalHeight;
                    visualPoly->setPosAt( Pos, i );
                }
            }
            if (visualPoly->isLooping())
                visualPoly->setPosAt( visualPoly->getPosAt(0), visualPoly->getPosCount() - 1 );

            // We are SURE that polyline is NOT a frise
            visualPoly->offsetData(frieze->get2DPos());
            visualPoly->recomputeData();

            // this visual frieze is never updated so we force the update here !
            visualPoly->update( 0 );
        }
    }

    void DataFluid::updateUV(f32 _elapsed)
    {
        if (fabs(_elapsed) < MTH_EPSILON)
            return;

        EdgeFluid *curEdgeFluid;
        for (ITF_VECTOR<EdgeFluid>::iterator edgeIter = m_edgeFluidList.begin();
            edgeIter != m_edgeFluidList.end(); ++edgeIter)
        {
            curEdgeFluid = &(*edgeIter);
            if (!curEdgeFluid->m_uvAnimTrans.IsEqual(Vec2d::Zero))
            {
                for (u32 i=0; i<4; i++)
                {
                    curEdgeFluid->m_uv[i] += curEdgeFluid->m_uvAnimTrans * _elapsed;
                }
            }
        }
    }

    void DataFluid::ProcessEdgeFluidList_InFluid(f32 _elapsed)
    {
        // This part will be done externally
        // to Add
        if (m_edgeFluidList.size() == 0)
            return;

        Frise* frieze = getFrieze();
        if (!frieze)
            return;

        FriseConfig * config = frieze->getConfig();
        if (!config)
            return;


        updateUV(_elapsed);

        checkElevationMapInitialization(config);

        checkActorInfluence();

        handlePropagation(frieze, config);

        updateCollisionPolyLine_InFluid(frieze, config);

        updateVisualPolyLine(frieze, config);
    }

    void DataFluid::computeEdgeFluidLevels(u32 _levelsFront, u32 _levelBack, bbool _cutUV)
    {
        ITF_ASSERT(_levelsFront + _levelBack < 31);
        f32 levelTab[32];
        u32 fullSize = (_levelsFront+_levelBack+1);
        u32 i=0;
        if (_levelBack != 0)
        {
            f32 step    = 1.f/(_levelBack+1);
            f32 val     = 1.f - step*_levelBack;
            for (; i<_levelBack; i++, val+=step)
            {
                levelTab[i] = val;
            }
        }
        levelTab[i++] = 1.f;
        if (_levelsFront != 0)
        {
            f32 step    = 1.f/(_levelsFront+1);
            f32 val     = 1.f - step;
            for (; i<fullSize; i++, val-=step)
            {
                levelTab[i] = val;
            }
        }
         
        m_edgeFluidListLevels.clear();
        m_edgeFluidListLevels.reserve(m_edgeFluidList.size()*fullSize);
        
        ITF_VECTOR<EdgeFluid>::iterator edgeIter = m_edgeFluidList.begin();
        for (; edgeIter != m_edgeFluidList.end(); ++edgeIter)
        {
            for (u32 i=0; i<fullSize; i++)
            {
                EdgeFluidLevel edgeFluidLevel;
                edgeFluidLevel.m_edgeFluid      = &(*edgeIter);
                edgeFluidLevel.m_data.m_level   = levelTab[i];
                edgeFluidLevel.m_data.m_pos     = f32((i32)i-(i32)_levelBack)/fullSize;
                edgeFluidLevel.m_data.m_cutUV   = _cutUV;
                edgeFluidLevel.m_layerInfos     = m_layerInfos;

                m_edgeFluidListLevels.push_back(edgeFluidLevel);
            }
        }
    }

    void DataFluid::addEdgeFluidLevelPerpendicular(i32 _way, f32 _heightScale, f32 _heightPos, f32 _posZ, f32 _z)
    {
        ITF_ASSERT(_way == 1 || _way == -1);

        ITF_VECTOR<EdgeFluid>::iterator edgeIter = m_edgeFluidList.begin();
        for (; edgeIter != m_edgeFluidList.end(); ++edgeIter)
        {
            EdgeFluidLevel edgeFluidLevel;
            edgeFluidLevel.m_edgeFluid          = &(*edgeIter);
            edgeFluidLevel.m_data.m_pos         = _z;
            edgeFluidLevel.m_data.m_cutUV       = btrue;
            edgeFluidLevel.m_data.m_way         = _way;
            edgeFluidLevel.m_data.m_heightScale = _heightScale;
            edgeFluidLevel.m_data.m_heightPos   = _heightPos;
            edgeFluidLevel.m_data.m_posZ        = _posZ;
            edgeFluidLevel.m_layerInfos         = m_layerInfos;

            m_edgeFluidListLevels.push_back(edgeFluidLevel);
        }
    }


    void DataFluid::computeAABB(AABBBuilder *_aabb)
    {
        Frise* frieze = getFrieze();
        if (!frieze)
            return;

        FriseConfig * config = frieze->getConfig();
        if (!config)
            return;

        f32 extrudeCoeffX = config->m_fluidPerpendicularBackScale *0.25f;
        f32 extrudeCoeffY = config->m_fluidPerpendicularBackScale *0.16f; 

        ITF_VECTOR<EdgeFluid>::iterator edgeIter = m_edgeFluidList.begin();

        for (; edgeIter != m_edgeFluidList.end();edgeIter++)
        {
            EdgeFluid &edgeFluid = *edgeIter;
            edgeFluid.m_aabb = AABB(edgeFluid.m_pos[0]);
            for (u32 i=1; i<4; i++)
                edgeFluid.m_aabb.grow(edgeFluid.m_pos[i]);

            // add wave marge
            Vec2d dir = edgeFluid.m_pos[1]-edgeFluid.m_pos[0];
            edgeFluid.m_aabb.grow(edgeFluid.m_pos[0] + dir*config->m_fluidTargetMinHeight);
            edgeFluid.m_aabb.grow(edgeFluid.m_pos[1] + dir*config->m_fluidTargetMaxHeight);
                  dir = edgeFluid.m_pos[3]-edgeFluid.m_pos[2];
            edgeFluid.m_aabb.grow(edgeFluid.m_pos[2] + dir*config->m_fluidTargetMinHeight);
            edgeFluid.m_aabb.grow(edgeFluid.m_pos[3] + dir*config->m_fluidTargetMaxHeight);

            _aabb->grow(edgeFluid.m_aabb);

            // add fluid marge
            if ( config->m_methode == Frise::InFluid )
            {
                Vec2d sightNormalized = (edgeFluid.m_pos[2] -edgeFluid.m_pos[0]).normalize();
                Vec2d extrude = sightNormalized *extrudeCoeffX +sightNormalized.getPerpendicular() *extrudeCoeffY;
                _aabb->grow( edgeFluid.m_pos[0] -extrude);
                _aabb->grow( edgeFluid.m_pos[2] +extrude);
            }            
        }
    }


    void DataFluid::clear()
    { 
        m_edgeFluidList.clear();
        m_contactActorsInfo.clear();
        m_edgeFluidListLevels.clear();
    }
} // namespace ITF
