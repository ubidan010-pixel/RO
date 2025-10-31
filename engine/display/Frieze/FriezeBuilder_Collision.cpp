#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_GAMEMATERIAL_H_
#include "engine/gameplay/GameMaterial.h"
#endif //_ITF_GAMEMATERIAL_H_

namespace ITF
{
    static f32 collision_normMinSqr = 0.01f;       

    void Frise::setupCollisionPolyline(PolyLine* _col) const
    {
        _col->m_ownerFrise = const_cast<Frise*>(this);
        _col->setDepth(getDepth());
    }

    void Frise::addDynamicCollisionPolyline(PolyLine* _col)
    {
        ITF_ASSERT(isDynamic());

        setupCollisionPolyline(_col);

        // Do not call this here to prevent recompute of polyline (Done in function Frise::finalizeCollisionList() called later)
        //_col->onLoaded();

        m_pFriezeFullData->m_dynamicCollisionData.push_back(_col);
    }

    void Frise::addStaticCollisionPolyline( PolyLine* _col )
    {
        ITF_ASSERT(m_pFriezeFullData->m_staticCollisionData.size() == m_pFriezeFullData->m_staticCollisionsLocalSpace.size());
        setupCollisionPolyline(_col);

        // Do not call this here to prevent recompute of polyline (Done in function Frise::finalizeCollisionList() called later)
        //_col->onLoaded();

        m_pFriezeFullData->m_staticCollisionData.push_back(_col);
        PolyPointList list;
        for (u32 i = 0; i < _col->getPosCount(); i++)
            list.addPoint(_col->getPosAt(i));
        m_pFriezeFullData->m_staticCollisionsLocalSpace.push_back(list);
        ITF_ASSERT(m_pFriezeFullData->m_staticCollisionData.size() == m_pFriezeFullData->m_staticCollisionsLocalSpace.size());
    }

    void Frise::createCollisionData()
    {
        PolyLine* _col = newAlloc(mId_Frieze,PolyLine());
        addStaticCollisionPolyline( _col );
        if (isDynamic())
        {
            PolyLine* _col = newAlloc(mId_Frieze,PolyLine());
            addDynamicCollisionPolyline( _col );
        }
    }

    void Frise::initCollision( u32 _index )
    {
        m_pFriezeFullData->m_staticCollisionData[_index]->clear();
        m_pFriezeFullData->m_staticCollisionData[_index]->forceLoop(bfalse);
        m_pFriezeFullData->m_staticCollisionsLocalSpace[_index].clear();

        if (isDynamic())
        {
            m_pFriezeFullData->m_dynamicCollisionData[_index]->clear();
            m_pFriezeFullData->m_dynamicCollisionData[_index]->forceLoop(bfalse);
        }
    }

    void Frise::initCollisionData( u32 _collisionWantedCount )
    {
        if ( _collisionWantedCount && !m_pFriezeFullData )
            createFriezeFullData();

        bbool newDynamicFriseValue = m_isDynamicFrieze;
        if (isDynamic() != newDynamicFriseValue)
        {
            if (!newDynamicFriseValue && isDynamic())
            {
                setDynamicTransform(GMatrixIdentity);
                applyDynamicTransform();
            }
            clearCollisionData();
            m_bIsDynamic = newDynamicFriseValue;
            for (u32 i=0; i<_collisionWantedCount; i++)        
            {
                createCollisionData();
            }
            return;
        }

        if ( !m_pFriezeFullData )
            return;

        u32 collisionDoneCount = m_pFriezeFullData->m_staticCollisionData.size();
        //  reinit des collision data existantes
        for( u32 i=0; i<collisionDoneCount; i++)
        {
            initCollision(i);
        }

        //  s'il existe moins de polyline de collision que nécessaire
        if ( collisionDoneCount <= _collisionWantedCount )
        {
            //  création des collisions data manquantes
            for (u32 i=collisionDoneCount; i<_collisionWantedCount; i++)        
            {
                createCollisionData();
            }
        }
        else
        {
            //  clear des collision data inutiles
            ITF_ASSERT(m_pFriezeFullData->m_staticCollisionData.size() == m_pFriezeFullData->m_staticCollisionsLocalSpace.size());
            //  delete collision data
            for (u32 i=_collisionWantedCount; i<collisionDoneCount; i++)
            {
                PolyLine *line = m_pFriezeFullData->m_staticCollisionData[m_pFriezeFullData->m_staticCollisionData.size() -1];
                line->m_connection.reset();
                SF_DEL(line);
                if (isDynamic())
                {
                    PolyLine *line = m_pFriezeFullData->m_dynamicCollisionData[m_pFriezeFullData->m_dynamicCollisionData.size() -1];
                    SF_DEL(line);
                    m_pFriezeFullData->m_dynamicCollisionData.pop_back();
                }
                m_pFriezeFullData->m_staticCollisionData.pop_back();
                m_pFriezeFullData->m_staticCollisionsLocalSpace.pop_back();
            }
        }
    }

    void Frise::clearCollisionData()
    {
        unregisterPhysic();

        if ( m_pFriezeFullData )
        {
            const u32 m_collisionDataCount = m_pFriezeFullData->m_staticCollisionData.size();

            if ( m_collisionDataCount == 0 )
                return;

            for( u32 i=0; i<m_collisionDataCount; i++)
            {
                m_pFriezeFullData->m_staticCollisionData[i]->m_connection.reset();
                if (isDynamic())
                    SF_DEL(m_pFriezeFullData->m_dynamicCollisionData[i]);
                SF_DEL(m_pFriezeFullData->m_staticCollisionData[i]);

            }

            m_pFriezeFullData->m_staticCollisionData.clear();
            m_pFriezeFullData->m_dynamicCollisionData.clear();
            m_pFriezeFullData->m_staticCollisionsLocalSpace.clear();
        }
    }

    void Frise::makeDynamicCollisionsFromStatic()
    {
        //Matrix44 matrix;

        ITF_ASSERT(isDynamic());
        ITF_ASSERT(m_pFriezeFullData->m_dynamicCollisionData.size() == m_pFriezeFullData->m_staticCollisionData.size());
        //computeDynamicDisplayMatrix(matrix); //        M44_load(m_pFriezeComplexDatas->m_dynamicTransform.fa16, &matrix);

        // bbool isAABBInvalid = btrue;

        switchCollisionsToGlobalSpace();

        for (u32 lineIndex=0; lineIndex<m_pFriezeFullData->m_staticCollisionData.size(); lineIndex++)
        {
            PolyLine *staticLine = m_pFriezeFullData->m_staticCollisionData[lineIndex];
            PolyLine *dynLine = m_pFriezeFullData->m_dynamicCollisionData[lineIndex];

            ITF_ASSERT(staticLine->getPosCount() == dynLine->getPosCount());

            u32 posCount = staticLine->getPosCount();
            //            f32 depth = getDepth();
            for (u32 i=0; i<posCount; i++)
            {
                const Vec2d &pos = staticLine->getPosAt(i);
                /*   Float4 pos4;
                Float4 transformed4;
                setFloat4(pos4, pos.m_x, pos.m_y  , depth, 1.f);
                M44_matrixCoordTransform(&transformed4, &matrix, &pos4);
                Vec2d transformed2d= *(Vec2d*)&transformed4;
                dynLine->setPosAt( transformed2d, i );
                if (isAABBInvalid)
                {
                m_pFriezeComplexDatas->m_AABB_Collision.setMin(transformed2d);
                m_pFriezeComplexDatas->m_AABB_Collision.setMax(transformed2d);
                isAABBInvalid = bfalse;
                }
                else
                {
                m_pFriezeComplexDatas->m_AABB_Collision.grow(transformed2d);
                }*/
                dynLine->setPosAt( pos, i );
                /*
                if (isAABBInvalid)
                {
                m_pFriezeComplexDatas->m_AABB_Collision = pos;
                isAABBInvalid = bfalse;
                }
                else
                */
                {
                    m_pFriezeFullData->m_AABB_Collision.grow(pos);
                }
            }
            dynLine->forceRecomputeData();
        }
    }

    bbool Frise::addPoinLastToCloseCollisionDouble( collisionRun& _colRun ) const
    {
        return _colRun.m_collisionDouble==DoubleAndLoop && m_pFriezeFullData->m_staticCollisionData[_colRun.m_index]->getPosCount() >0;
    }

    void Frise::setCollisionExtremityGameMaterial( collisionRun& _colRun, bbool _startExtremity)
    {
        if ( _colRun.m_collisionDouble == DoubleAndLoop )
        {
            FriseConfig* config = getConfig();

            if ( config )
            {
                Path gMat;

                if ( _startExtremity )
                    gMat = config->m_gameMaterialStartExtremity;
                else
                    gMat = config->m_gameMaterialStopExtremity;

                if ( gMat != GameMaterial_Template::nullMatName )
                {
                    m_pFriezeFullData->m_staticCollisionData[_colRun.m_index]->setGameMaterialAt( gMat.getStringID(), m_pFriezeFullData->m_staticCollisionData[_colRun.m_index]->getPosCount() -1 );
                }                
            }
        }
    }

    void Frise::addPointsCollision( const FriseConfig* _config, SafeArray<Vec2d>& collision, ITF_VECTOR<CollisionByGameMat>& _collisionList, collisionRun& _colRun )
    {                
        if ( collision.size() < 2 )
            return;

        Vec2d posLast(Vec2d::Infinity);
        Vec2d posLastOld(-Vec2d::Infinity);
        i32 collisionListCount = _collisionList.size(); 

        u32 pointCollisionCount = m_pFriezeFullData->m_staticCollisionData[_colRun.m_index]->getPosCount();
        u32 indexPoint = pointCollisionCount;

        if ( pointCollisionCount > 0 )
        {
            posLast = m_pFriezeFullData->m_staticCollisionData[_colRun.m_index]->getPosAt(pointCollisionCount-1);

            if ( pointCollisionCount > 1 )
            {
                posLastOld = m_pFriezeFullData->m_staticCollisionData[_colRun.m_index]->getPosAt(pointCollisionCount-2);
            }

            setCollisionExtremityGameMaterial( _colRun, bfalse);
        }

        bbool flipWanted = flipCollision();
        if ( _colRun.m_collisionDouble != None )
        {
            flipWanted = !flipWanted;
        }

        if ( flipWanted )
        {
            for ( i32 idCol = collisionListCount -1; idCol >= 0; idCol -- )
            {
                CollisionByGameMat& collisionCur = _collisionList[idCol];

                if ( collisionCur.m_gameMat != GameMaterial_Template::nullMatName )
                {
                    for ( i32 idPoint = collisionCur.m_stopId -1; idPoint >= collisionCur.m_startId; idPoint-- )
                        AddPointInCollisionDataWithGMat( _config, collision[idPoint], posLast, posLastOld, indexPoint, collisionCur.m_gameMat, _colRun.m_index );

                    if ( addPoinLastToCloseCollisionDouble(_colRun) )
                    {
                        setCollisionExtremityGameMaterial( _colRun, btrue);

                        Vec2d pointFirst = m_pFriezeFullData->m_staticCollisionData[_colRun.m_index]->getPosAt(0);
                        AddPointInCollisionDataWithGMat( _config, pointFirst, posLast, posLastOld, indexPoint, collisionCur.m_gameMat, _colRun.m_index );
                    }
                }
                else
                {
                    for ( i32 idPoint = collisionCur.m_stopId -1; idPoint >= collisionCur.m_startId; idPoint-- )
                        AddPointInCollisionData( _config, collision[idPoint], posLast, posLastOld, indexPoint, _colRun.m_index, collisionCur.m_gameMat );

                    if ( addPoinLastToCloseCollisionDouble(_colRun) )
                    {
                        setCollisionExtremityGameMaterial( _colRun, btrue);

                        Vec2d pointFirst = m_pFriezeFullData->m_staticCollisionData[_colRun.m_index]->getPosAt(0);
                        AddPointInCollisionData( _config, pointFirst, posLast, posLastOld, indexPoint, _colRun.m_index, collisionCur.m_gameMat );
                    }
                }
            }
        }
        else
        {
            for ( i32 idCol = 0; idCol < collisionListCount; idCol ++ )
            {
                CollisionByGameMat& collisionCur = _collisionList[idCol];

                if ( collisionCur.m_gameMat != GameMaterial_Template::nullMatName )
                {
                    for ( i32 idPoint = collisionCur.m_startId; idPoint < collisionCur.m_stopId; idPoint++ )
                        AddPointInCollisionDataWithGMat( _config, collision[idPoint], posLast, posLastOld, indexPoint, collisionCur.m_gameMat, _colRun.m_index );

                    if ( addPoinLastToCloseCollisionDouble(_colRun) )
                    {
                        setCollisionExtremityGameMaterial( _colRun, btrue);

                        Vec2d pointFirst = m_pFriezeFullData->m_staticCollisionData[_colRun.m_index]->getPosAt(0);
                        AddPointInCollisionDataWithGMat( _config, pointFirst, posLast, posLastOld, indexPoint, collisionCur.m_gameMat, _colRun.m_index );
                    }
                }
                else
                {
                    for ( i32 idPoint = collisionCur.m_startId; idPoint < collisionCur.m_stopId; idPoint++ )
                        AddPointInCollisionData( _config, collision[idPoint], posLast, posLastOld, indexPoint, _colRun.m_index, collisionCur.m_gameMat );

                    if ( addPoinLastToCloseCollisionDouble(_colRun) )
                    {
                        setCollisionExtremityGameMaterial( _colRun, btrue);

                        Vec2d pointFirst = m_pFriezeFullData->m_staticCollisionData[_colRun.m_index]->getPosAt(0);
                        AddPointInCollisionData( _config, pointFirst, posLast, posLastOld, indexPoint, _colRun.m_index, collisionCur.m_gameMat );
                    }
                }
            }
        }
    }

    bbool Frise::isParallelEdges( const Vec2d& _posCur, const Vec2d& _posLast, const Vec2d& _posLastOld, f32 _distMax) const
    {
        Vec2d sight = (_posCur -_posLastOld).getPerpendicular().normalize();

        f32 dist = f32_Abs(sight.dot(_posLast -_posLastOld));

        if ( dist > _distMax )
            return bfalse;

        return btrue;
    }

    bbool Frise::skipEdgeParallelInCollisionData( const FriseConfig* _config, Vec2d& _posCur, Vec2d& _posLast, Vec2d& _posLastOld, u32 _indexPoint, u32 _indexCollisionData, Path& _gameMat )
    {
        if ( _indexPoint < 2 )
            return bfalse;

        if ( !isParallelEdges( _posCur, _posLast, _posLastOld, _config->m_collisionDistMaxToSkipParallelEdge ) )
            return bfalse;

        // if parallel edges: posCur isn't added --> move last point added on posCur
        m_pFriezeFullData->m_staticCollisionData[_indexCollisionData]->setPosAt( _posCur, _indexPoint -1 );

        // set GameMaterial
        m_pFriezeFullData->m_staticCollisionData[_indexCollisionData]->setGameMaterialAt( _gameMat.getStringID(), _indexPoint -1 );

        // grow aabb
        m_pFriezeFullData->m_AABB_Collision.grow(_posCur);

        // only update last position 
        _posLast = _posCur;

        return btrue;
    }

    bbool Frise::AddPointInCollisionData( const FriseConfig* _config, Vec2d& _posCur, Vec2d& _posLast, Vec2d& _posLastOld, u32& _indexPoint, u32 _indexCollisionData, Path& _gameMat )
    {
        if ( ( _posCur -_posLast ).sqrnorm() < collision_normMinSqr )
            return bfalse;

        if ( skipEdgeParallelInCollisionData( _config, _posCur, _posLast, _posLastOld, _indexPoint, _indexCollisionData, _gameMat ) )
            return bfalse;

        // add point
        PolyLine* polyline = m_pFriezeFullData->m_staticCollisionData[_indexCollisionData];
        polyline->addPoint( _posCur );

        m_pFriezeFullData->m_AABB_Collision.grow(_posCur);

        _indexPoint++; 

        // update last positions
        _posLastOld = _posLast;
        _posLast = _posCur;

        return btrue;
    }

    void Frise::AddPointInCollisionDataWithGMat( const FriseConfig* _config, Vec2d& _posCur, Vec2d& _posLast, Vec2d& _posLastOld, u32& _indexPoint, Path& _gameMat, u32 _indexCollisionData )
    {
        if ( AddPointInCollisionData( _config, _posCur, _posLast, _posLastOld, _indexPoint, _indexCollisionData, _gameMat ) )
        {
            m_pFriezeFullData->m_staticCollisionData[_indexCollisionData]->setGameMaterialAt( _gameMat.getStringID(), _indexPoint -1 );
        }
    }

    bbool Frise::flipCollision() const
    {
        FriseConfig* config = getConfig();
        if ( !config )
            return bfalse;

        return config->m_isCollisionFlip;
    }

    bbool Frise::isEmptyCollision( u32 _index ) const
    {
        return m_pFriezeFullData->m_staticCollisionData[_index]->getPosCount() < 2 ;
    }

    i32 Frise::getIdEmptyCollision()
    {
        u32 collisionListCount = m_pFriezeFullData->m_staticCollisionData.size();

        for ( u32 index = 0; index < collisionListCount; index++ )
        {
            if ( isEmptyCollision( index ) )
            {
                m_pFriezeFullData->m_staticCollisionData[index]->clear();
                if (isDynamic())
                {
                    m_pFriezeFullData->m_dynamicCollisionData[index]->clear();
                }
                return (i32)index;
            }
        }
        return -1;
    }

    void Frise::finalizeCollisionList()
    {
        if ( !m_pFriezeFullData )
            return;

        u32 collisionListCount = m_pFriezeFullData->m_staticCollisionData.size();
        u32 idCol = 0;

        for ( u32 count = 0; count < collisionListCount; count++ )
        {
            ITF_WARNING_CATEGORY(LD,this, m_pFriezeFullData->m_staticCollisionData[idCol]->getPosCount() <=50, " Frieze has %i collision edges ( max = 50 ), you have to split it or create hole collision! ", m_pFriezeFullData->m_staticCollisionData[idCol]->getPosCount());
            
            // delete empty collision
            if ( isEmptyCollision( idCol ) )
            {                                
                if (isDynamic())
                {
                    SF_DEL(m_pFriezeFullData->m_dynamicCollisionData[idCol]);
                    m_pFriezeFullData->m_dynamicCollisionData.eraseKeepOrder(idCol);
                }

                SF_DEL(m_pFriezeFullData->m_staticCollisionData[idCol]);
                m_pFriezeFullData->m_staticCollisionData.eraseKeepOrder(idCol);
                m_pFriezeFullData->m_staticCollisionsLocalSpace.erase( m_pFriezeFullData->m_staticCollisionsLocalSpace.begin() +idCol);

                continue;
            }
            else if (isDynamic())
            {
                switchCollisionsToGlobalSpace();
                u32 posCount = m_pFriezeFullData->m_staticCollisionData[idCol]->getPosCount();
                Matrix44 matrix;
                Float4 curPos3d;
                Float4 dynamicCurPos;

                ITF_ASSERT(m_pFriezeFullData->m_dynamicCollisionData[idCol]->getPosCount()==0);

                for (u32 indexPoint=0; indexPoint<posCount; indexPoint++)
                {
                    Vec2d posPoint = m_pFriezeFullData->m_staticCollisionData[idCol]->getPosAt(indexPoint);

                    M44_load(m_pFriezeFullData->m_dynamicTransform.fa16, &matrix);
                    setFloat4(curPos3d,posPoint.m_x, posPoint.m_y, getDepth(),1.f);
                    M44_matrixCoordTransform(&dynamicCurPos, &matrix, &curPos3d);

                    m_pFriezeFullData->m_dynamicCollisionData[idCol]->addPoint( (Vec2d&)dynamicCurPos );
                    m_pFriezeFullData->m_dynamicCollisionData[idCol]->setGameMaterialAt( m_pFriezeFullData->m_staticCollisionData[idCol]->getEdgeAt(indexPoint).getGameMaterial(), indexPoint );

                    m_pFriezeFullData->m_AABB_Collision.grow((Vec2d&)dynamicCurPos);
                }
            }

            // loop
            if ( m_pFriezeFullData->m_staticCollisionData[idCol]->getPosAt(0).IsEqual( m_pFriezeFullData->m_staticCollisionData[idCol]->getPosAt( m_pFriezeFullData->m_staticCollisionData[idCol]->getPosCount()-1 ), 0.1f ) )
            {
                m_pFriezeFullData->m_staticCollisionData[idCol]->forceLoop(btrue);
                if (isDynamic())
                {
                    m_pFriezeFullData->m_dynamicCollisionData[idCol]->forceLoop(btrue);
                }
#ifdef DEVELOPER_JAY_FRIEZE
                Vec2d posStart(m_pFriezeFullData->m_staticCollisionData[idCol]->getPosAt(0));
                AABB pointLoop(posStart);
                Vec2d offset(0.1f,0.1f);
                pointLoop.setMax(posStart+offset);
                pointLoop.setMin(posStart-offset);
                GFX_ADAPTER->drawDBGAABB( pointLoop, Color::red(), 0.f, 5.f );
#endif
            }

            m_pFriezeFullData->m_staticCollisionData[idCol]->onLoaded();

            if (isDynamic())
            {
                m_pFriezeFullData->m_dynamicCollisionData[idCol]->onLoaded();
            }

            idCol++;            
        }

        // need check connections    
        if ( m_pFriezeFullData->m_staticCollisionData.size() )
        {
            World* pWorld = getWorld();
            if ( pWorld )
            {
                pWorld->checkFriezeConnections();     
            }            
        }

#ifdef DEVELOPER_JAY_FRIEZE
        for ( u32 count = 0; count < m_pFriezeFullData->m_staticCollisionData.size(); count++ )
        {
            u32 edgeCount = m_pFriezeFullData->m_staticCollisionData[count]->getPosCount();

            for ( u32 i = 0; i < edgeCount; i++ )
            {
                const PolyLineEdge& edge = m_pFriezeFullData->m_staticCollisionData[count]->getEdgeAt(i);

                Vec2d pos = edge.getPos() /*+ edge.m_normalizedVector.getPerpendicular()*/ /*+ edge.m_vector *0.5f*/;
                debugDrawInt( transformPos(pos), i, Color::red() );
            }
        }
#endif
    }

    bbool Frise::isEdgeRunCollisionable( const FriseConfig* _config, const edgeRun& _edgeRun, const edgeFrieze& _edge ) const
    {
        if ( _edgeRun.m_idTex == -1 )
            return _config->m_collisionFrieze.m_build;

        switch ( _config->m_methode )
        {
        case InFluid:
        case InString:
            return _config->m_textureConfigs[_edgeRun.m_idTex].m_collisionSimple.m_build && isEdgeWithHoleVisual( _edge );
            break;

        default:
            return _config->m_textureConfigs[_edgeRun.m_idTex].m_collisionSimple.m_build;
            break;
        }        
    }

    void Frise::getIdEdgeRunStartCollision( const FriseConfig* _friseConfig, ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, u32& _idEdgeRunStart, u32& _edgeCountStart ) const
    {
        if ( !m_pointsList.isLooping() )
            return;

        u32 edgeRunListCount = _edgeRunList.size();
        edgeFrieze& startEdge = _edgeList[_edgeRunList[_idEdgeRunStart].m_idEdgeStart];        

        if ( !isEdgeRunCollisionable( _friseConfig, _edgeRunList[_idEdgeRunStart], startEdge ) || isEdgeRunWithCollisionHole( _edgeList, _edgeRunList[_idEdgeRunStart], _idEdgeRunStart, _edgeCountStart, edgeRunListCount ) )
            return;

        u32 searchId = _edgeRunList.size() -1;

        while ( searchId > 0 && isEdgeRunCollisionable( _friseConfig, _edgeRunList[searchId], _edgeList[_edgeRunList[searchId].m_idEdgeStart] ) )
        {            
            _idEdgeRunStart = searchId; 

            if ( isEdgeRunWithCollisionHole( _edgeList, _edgeRunList[_idEdgeRunStart], _idEdgeRunStart, _edgeCountStart, edgeRunListCount ) )
                return;

            searchId--;
        }
    }

    bbool Frise::isEdgeRunWithCollisionHole( ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun& _edgeRun, u32& _idEdgeRunStart, u32& _edgeCountStart, u32 _edgeRunListCount ) const
    {
        ITF_ASSERT( _edgeRun.m_edgeCount > 0 );

        i32 countStop = _edgeRun.m_edgeCount -1;
        for ( i32 count = countStop; count >= 0 ; count -- )
        {
            u32 idEdge = ( _edgeRun.m_idEdgeStart + count ) % m_recomputeData.m_edgeListCount;

            if ( isEdgeWithHoleCollision( _edgeList[idEdge] ) )
            {
                if ( count == countStop )
                    _idEdgeRunStart = ( _idEdgeRunStart +1 ) % _edgeRunListCount;
                else
                    _edgeCountStart = u32(count +1);

                return btrue;
            }
        }

        return bfalse;
    }

    void Frise::initCollisionRun( const FriseConfig* _friseConfig, ITF_VECTOR<edgeRun>& _edgeRunList, collisionRun& _collisionRun, u32 _idEdgeRunStart, u32 _idEdgeStart, u32 _edgeCountStartOffset ) const
    {
        _collisionRun.m_idEdgeRunStart = _idEdgeRunStart;
        _collisionRun.m_edgeRunCount = 1;
        _collisionRun.m_idEdgeStart = _idEdgeStart;
        _collisionRun.m_edgeCount = 1;
        _collisionRun.m_edgeCountStartOffset = _edgeCountStartOffset;
        _collisionRun.m_collisionOffsetLast = -1.f;
        _collisionRun.m_collisionOffsetNext = -1.f;

        // collision offset with edgeRunLast
        if ( _edgeCountStartOffset != 0 )
            return;

        if ( !m_pointsList.isLooping() && _idEdgeRunStart == 0 )
            return;

        u32 edgeRunListCount = _edgeRunList.size();
        u32 idEdgeRunLast = ( _idEdgeRunStart +edgeRunListCount -1 ) % edgeRunListCount;

        _collisionRun.m_collisionOffsetLast = getCollisionOffSet( _friseConfig, &_edgeRunList[idEdgeRunLast], _collisionRun );
    }

    bbool Frise::checkStartCollisionRun( const FriseConfig* _friseConfig, ITF_VECTOR<edgeRun>& _edgeRunList, collisionRun& _collisionRun, bbool& _build, u32& _idEdgeRunOld, u32 _idEdgeRun, u32 _idEdge, u32 _edgeCountStartOffset ) const
    {
        if ( _build )
        {
            _collisionRun.m_edgeCount++;

            if ( _idEdgeRunOld != _idEdgeRun )
            {
                _idEdgeRunOld = _idEdgeRun;
                _collisionRun.m_edgeRunCount++;
            }

            return bfalse;
        } 
        else
        {
            _build = btrue;
            _idEdgeRunOld = _idEdgeRun;

            initCollisionRun( _friseConfig, _edgeRunList, _collisionRun, _idEdgeRun, _idEdge, _edgeCountStartOffset );

            return btrue;
        }
    }

    void Frise::finalizeCollisionRun( const FriseConfig* _friseConfig, ITF_VECTOR<edgeRun>& _edgeRunList, collisionRun& _collisionRun, u32 _edgeCountStopOffset, u32 _idEdgeRun ) const
    {
        _collisionRun.m_edgeCountStopOffset = _edgeCountStopOffset;

        // collision offset with edgeRunLast
        if ( _edgeCountStopOffset != 0 )
            return;

        u32 edgeRunListCount = _edgeRunList.size();

        if ( !m_pointsList.isLooping() && _idEdgeRun == edgeRunListCount -1 )
            return;

        u32 idEdgeRunNext = ( _idEdgeRun +1 ) % edgeRunListCount;

        _collisionRun.m_collisionOffsetNext = getCollisionOffSet( _friseConfig, &_edgeRunList[idEdgeRunNext], _collisionRun );
    }

    bbool Frise::checkStopCollisionRun( const FriseConfig* _friseConfig, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList, collisionRun& _collisionRun, bbool& _build, u32 _edgeCountStopOffset, u32 _idEdgeRun )
    {
        if ( _build )
        {
            _build = bfalse;
            _collisionRun.m_index = _collisionRunList.size();

            finalizeCollisionRun( _friseConfig, _edgeRunList, _collisionRun, _edgeCountStopOffset, _idEdgeRun );
            _collisionRunList.push_back( _collisionRun );

            return btrue;
        }

        return bfalse;
    }

    bbool Frise::buildCollisionRunList( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList )
    {
        u32 edgeRunListCount = _edgeRunList.size();
        FriseConfig* config = getConfig();

        if ( !config || !config->m_collisionFrieze.m_build || edgeRunListCount == 0 )
        {
            bbool newDynamicFriseValue = bfalse;
            if (config)
            {
                newDynamicFriseValue = m_isDynamicFrieze;
            }
            if (!newDynamicFriseValue && isDynamic())
            {
                setDynamicTransform(GMatrixIdentity);
                applyDynamicTransform();
            }
            clearCollisionData();
            m_bIsDynamic = newDynamicFriseValue;
            return bfalse;
        }

        u32 idEdgeRunStart = 0;
        u32 edgeCountStart = 0;        
        getIdEdgeRunStartCollision( config, _edgeList, _edgeRunList, idEdgeRunStart, edgeCountStart );

        u32 edgeCountStop = edgeCountStart;       
        bbool buildCollisionRun = bfalse;
        collisionRun collisionRunCur;
        u32 idEdgeRunOld = 0;

        for ( u32 edgeRunCount = 0; edgeRunCount < edgeRunListCount; edgeRunCount++ )
        {
            u32 idEdgeRunCur = ( idEdgeRunStart + edgeRunCount ) % edgeRunListCount;
            edgeRun& edgeRunCur = _edgeRunList[idEdgeRunCur];

            if ( isEdgeRunCollisionable( config, edgeRunCur, _edgeList[edgeRunCur.m_idEdgeStart] ) )
            {         
                for ( u32 edgeCount = edgeCountStart; edgeCount < edgeRunCur.m_edgeCount; edgeCount++ )
                {
                    u32 idEdgeCur = ( edgeRunCur.m_idEdgeStart + edgeCount ) % m_recomputeData.m_edgeListCount;

                    if ( isEdgeWithHoleCollision( _edgeList[idEdgeCur] ) )
                    {
                        if ( edgeCount == 0 )
                            checkStopCollisionRun( config, _edgeRunList, _collisionRunList, collisionRunCur, buildCollisionRun, 0, idEdgeRunOld );
                        else
                            checkStopCollisionRun( config, _edgeRunList, _collisionRunList, collisionRunCur, buildCollisionRun, edgeRunCur.m_edgeCount -edgeCount, idEdgeRunOld );
                    }
                    else
                        checkStartCollisionRun( config, _edgeRunList, collisionRunCur, buildCollisionRun, idEdgeRunOld, idEdgeRunCur, idEdgeCur, edgeCount );
                }               
            }
            else
                checkStopCollisionRun( config, _edgeRunList, _collisionRunList, collisionRunCur, buildCollisionRun, 0, idEdgeRunOld );

            edgeCountStart = 0;
        }

        // check startEdgeRun
        edgeRun& edgeRunCur = _edgeRunList[idEdgeRunStart];
        ITF_ASSERT ( edgeCountStop <= edgeRunCur.m_edgeCount);
        if ( isEdgeRunCollisionable( config, edgeRunCur, _edgeList[edgeRunCur.m_idEdgeStart] ) )
        {                     
            for ( u32 edgeCount = 0; edgeCount < edgeCountStop; edgeCount++ )
            {
                u32 idEdgeCur = ( edgeRunCur.m_idEdgeStart + edgeCount ) % m_recomputeData.m_edgeListCount;

                if ( isEdgeWithHoleCollision( _edgeList[idEdgeCur] ) )
                {
                    if ( edgeCount == 0 )
                        checkStopCollisionRun( config, _edgeRunList, _collisionRunList, collisionRunCur, buildCollisionRun, 0, idEdgeRunOld );
                    else
                        checkStopCollisionRun( config, _edgeRunList, _collisionRunList, collisionRunCur, buildCollisionRun, edgeRunCur.m_edgeCount -edgeCount, idEdgeRunOld);
                }
                else
                    checkStartCollisionRun( config, _edgeRunList, collisionRunCur, buildCollisionRun, idEdgeRunOld, idEdgeRunStart, idEdgeCur, edgeCount );
            }               
        }

        checkStopCollisionRun( config, _edgeRunList, _collisionRunList, collisionRunCur, buildCollisionRun, 0, idEdgeRunOld );

        u32 collisionRunListCount = _collisionRunList.size();
        if ( collisionRunListCount == 0 )
        {
            bbool newDynamicFriseValue = m_isDynamicFrieze;
            if (!newDynamicFriseValue && isDynamic())
            {
                setDynamicTransform(GMatrixIdentity);
                applyDynamicTransform();
            }

            clearCollisionData();
            m_bIsDynamic = newDynamicFriseValue;

            return bfalse;
        }

        if ( config->m_collisionDouble )
        {
            collisionRunListCount = buildCollisionRunListDouble( _collisionRunList, config );            
        }

        initCollisionData( collisionRunListCount );

        verifyCollision( _edgeRunList, _collisionRunList );

        return btrue;
    }

    void Frise::verifyCollision( ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList ) const
    {
        if ( !m_pointsList.isLooping() )
            return;

        if ( _edgeRunList.size() > 1 )
            return;

        u32 collisionRunListCount = _collisionRunList.size();
        for ( u32 i = 0; i < collisionRunListCount; i++ )
        {
            ITF_ASSERT(m_recomputeData.m_edgeListCount >= _collisionRunList[i].m_edgeCountStartOffset);

            u32 countOffset = m_recomputeData.m_edgeListCount -_collisionRunList[i].m_edgeCountStartOffset;

            if ( countOffset < _collisionRunList[i].m_edgeCountStopOffset )
            {                    
                // update collisionRunList offset                              
                for ( u32 j = 0; j < collisionRunListCount; j++ )
                {
                    collisionRun& colRun = _collisionRunList[j];

                    colRun.m_edgeCountStartOffset = ( colRun.m_edgeCountStartOffset + countOffset ) % m_recomputeData.m_edgeListCount;                        
                    colRun.m_edgeCountStopOffset  = ( colRun.m_edgeCountStopOffset - countOffset + m_recomputeData.m_edgeListCount ) % m_recomputeData.m_edgeListCount;  
                }

                // update edgeRun
                edgeRun& edgeRunCur = _edgeRunList[0];
                edgeRunCur.m_idEdgeStart = ( edgeRunCur.m_idEdgeStart -countOffset + m_recomputeData.m_edgeListCount ) % m_recomputeData.m_edgeListCount;

                return;
            }
        }
    }

    const Path& Frise::getCollisionGameMaterial( const FriseConfig* _friseConfig, i32 _idTex ) const
    {
        // GMat from texture config
        if ( _idTex > -1 && _friseConfig->m_textureConfigs[_idTex].m_gameMaterial != GameMaterial_Template::nullMatName )
        {
            return _friseConfig->m_textureConfigs[_idTex].m_gameMaterial;
        }

        // GMat from frieze config
        if ( _friseConfig->m_gameMaterial != GameMaterial_Template::nullMatName )
        {
            return _friseConfig->m_gameMaterial;
        }

        return GameMaterial_Template::nullMatName;        
    }

    f32 Frise::getCollisionOffSetSimple( const FriseConfig* config, const edgeRun* _edgeRun ) const
    {
        f32 offsetCur;

        if ( _edgeRun->m_idTex == -1 )
            offsetCur = config->m_collisionFrieze.m_offset;
        else
            offsetCur = config->m_textureConfigs[_edgeRun->m_idTex].m_collisionSimple.m_offset;

        offsetCur = Clamp( offsetCur, 0.f, 1.f );

        return offsetCur;
    }

    f32 Frise::getCollisionOffSet( const FriseConfig* config, const edgeRun* _edgeRun, collisionRun& _colRun ) const
    {
        f32 offsetCur = getCollisionOffSetSimple( config, _edgeRun );

        if ( _colRun.m_collisionDouble != None )
            offsetCur = 1.f -offsetCur;

        return offsetCur;
    }

    void Frise::buildCollision_EdgeRunList( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList )
    {
        FriseConfig* config = getConfig();
        if ( !config )
            return;

        SafeArray<Vec2d> collision;
        collision.reserve( m_recomputeData.m_edgeListCount *3 +3 );

        edgeRun* edgeRunLast = NULL;
        f32 offsetLast = -1.f;

        u32 edgeRunListCount = _edgeRunList.size();
        u32 collisionRunListCount = _collisionRunList.size();

        for ( u32 idCol = 0; idCol < collisionRunListCount; idCol++ )
        {
            collisionRun& colRun = _collisionRunList[idCol];            
            edgeRun* edgeRunCur = &_edgeRunList[colRun.m_idEdgeRunStart];

            // init offset position
            f32 offsetCur = getCollisionOffSet( config, edgeRunCur, colRun);

            ITF_VECTOR<CollisionByGameMat> collisionList;
            CollisionByGameMat collisionByGMat;
            collisionByGMat.m_startId = 0;
            collisionByGMat.m_gameMat = getCollisionGameMaterial( config, edgeRunCur->m_idTex );

            // countOffsets
            u32 edgeCountStartOffset = colRun.m_edgeCountStartOffset;

            // build start points   
            if ( colRun.m_collisionOffsetLast != -1.f && colRun.m_collisionOffsetLast != offsetCur )
            {
                buildOutline_CornerSquareBlending_startEdgeRun( collision, _edgeList, colRun.m_idEdgeStart, offsetCur, colRun.m_collisionOffsetLast );
            }
            else
            {
                buildCollision_CornerSquare_StartEdgeRun( config, _edgeList, collision, &_edgeList[colRun.m_idEdgeStart], colRun.m_idEdgeStart, offsetCur, colRun );
            }

            for ( u32 edgeRunCount = 0; edgeRunCount < colRun.m_edgeRunCount; edgeRunCount++ )
            {
                u32 idEdgeRun = ( edgeRunCount + colRun.m_idEdgeRunStart ) % edgeRunListCount;
                edgeRunCur = &_edgeRunList[idEdgeRun];

                // update offset position
                offsetCur = getCollisionOffSet( config, edgeRunCur, colRun);         

                if ( edgeRunLast )
                {
                    u32 buildCount = collision.size();

                    // transition between two edgeRun
                    buildOutline_StartEdgeRunCornerSquare( config, collision,_edgeList, edgeRunLast, edgeRunCur, offsetCur, offsetLast);

                    const Path& gameMat = getCollisionGameMaterial(config, edgeRunCur->m_idTex );

                    // add collision
                    if ( gameMat != collisionByGMat.m_gameMat )
                    {
                        u32 buildAddBy2 = Max( 0, i32(collision.size() -buildCount) -2);
                        collisionByGMat.m_stopId = buildCount + buildAddBy2;

                        collisionList.push_back(collisionByGMat);

                        collisionByGMat.m_startId = collisionByGMat.m_stopId;
                        collisionByGMat.m_gameMat = gameMat;
                    }
                }

                // outline edgeRun
                u32 edgeCountStopOffset = 0;
                if ( edgeRunCount == colRun.m_edgeRunCount -1)
                    edgeCountStopOffset = colRun.m_edgeCountStopOffset;

                if ( config->m_smoothCollision )
                    buildOutline_EdgeRunWithCornerRounder( config, collision, _edgeList, edgeRunCur, offsetCur, config->m_smoothFactorCollision, edgeCountStartOffset, edgeCountStopOffset );
                else
                    buildOutline_EdgeRunWithCornerSquare( config, collision, _edgeList, edgeRunCur, offsetCur, edgeCountStartOffset, edgeCountStopOffset );

                edgeRunLast = edgeRunCur;
                offsetLast = offsetCur;
                edgeCountStartOffset = 0;
            }

            // build stop points          
            u32 idStopEdge = ( colRun.m_idEdgeStart + colRun.m_edgeCount + m_recomputeData.m_edgeListCount -1 ) % m_recomputeData.m_edgeListCount;
            if ( colRun.m_collisionOffsetNext != -1.f && colRun.m_collisionOffsetNext != offsetCur )
                buildOutline_CornerSquareBlending_stopEdgeRun( collision, _edgeList, idStopEdge, offsetCur, colRun.m_collisionOffsetNext );
            else
            {          
                buildCollision_CornerSquare_StopEdgeRun( config, collision, _edgeList, idStopEdge, offsetCur, colRun );
            }

            // add collision
            collisionByGMat.m_stopId = collision.size();
            collisionList.push_back(collisionByGMat);

            addPointsCollision( config, collision, collisionList, colRun );
            collision.clear();

            edgeRunLast = NULL;            
        }       
    }

    extern void rotateAABB(AABBBuilder& _aabb, const Frise& _frise, bbool _useTranslation);

    void Frise::computeOrientedGlobalAABB(AABBBuilder& _aabb, u32 _colIndex) const
    {
        ITF_ASSERT(_colIndex < m_pFriezeFullData->m_staticCollisionsLocalSpace.size());
        if (_colIndex < m_pFriezeFullData->m_staticCollisionsLocalSpace.size())
        {
            const PolyPointList& localColPts = m_pFriezeFullData->m_staticCollisionsLocalSpace[_colIndex];
            for (u32 icol=0; icol<localColPts.getPosCount(); icol++)
            {
                if (!icol)
                    _aabb.set(localColPts.getPosAt(icol));
                else
                    _aabb.grow(localColPts.getPosAt(icol));
            }
            rotateAABB(_aabb, *this, btrue);

        }
    }

    // check connections only for Previous Polyline
    void Frise::checkConnections(PickableList& _friseList, ITF_VECTOR<FriezeConnectionResult>& _results)
    {
        const f32 deltaConnection = 0.1f;
        const u32 friezeListCount = _friseList.size();
        const Vec2d scaleAABB( deltaConnection, deltaConnection);
        

        SafeArray<PolyLine*, 20> curFriezeCollisions;

        for ( u32 currentFriezeIndex = 0; currentFriezeIndex < friezeListCount; ++currentFriezeIndex )
        {           
            Frise* pCurrentFrieze = (Frise*)_friseList[currentFriezeIndex];

            if ( !pCurrentFrieze->m_pFriezeFullData )
                continue;

            const u32 collisionDataCountRef = pCurrentFrieze->m_pFriezeFullData->m_staticCollisionData.size();

            if (!pCurrentFrieze->canConnectToOtherFrieze())
                continue;

            // build polyline ref list
            curFriezeCollisions.clear();

            for ( u32 i = 0; i < collisionDataCountRef; i++)
            {
                PolyLine* polRef = pCurrentFrieze->m_pFriezeFullData->m_staticCollisionData[i];

                if ( !polRef->isLooping() && !polRef->getPrevious() && polRef->getPosCount() > 1 )
                    curFriezeCollisions.push_back(polRef);
            }

            // no polyline to check
            if ( curFriezeCollisions.size() == 0 )
                continue;

            AABB curFriezeAABB = pCurrentFrieze->getAABB();
            curFriezeAABB.grow( curFriezeAABB.getMin() - scaleAABB );
            curFriezeAABB.grow( curFriezeAABB.getMax() + scaleAABB );

            // check with others friezes
            for ( u32 compareFriezeIndex = 0; compareFriezeIndex < friezeListCount && curFriezeCollisions.size(); ++compareFriezeIndex )
            {          
                if ( compareFriezeIndex == currentFriezeIndex )
                    continue;

                Frise* pCompareFrieze = (Frise*)_friseList[compareFriezeIndex];

                if ( !pCompareFrieze->m_pFriezeFullData )
                    continue;

                if (!pCompareFrieze->canConnectToOtherFrieze())
                    continue;

                const u32 collisionDataCount = pCompareFrieze->m_pFriezeFullData->m_staticCollisionData.size();

                // no polyline to check
                if ( collisionDataCount == 0 )
                    continue;

                // loop polyline
                if ( collisionDataCount == 1 && pCompareFrieze->m_pFriezeFullData->m_staticCollisionData[0]->isLooping()  )
                    continue;

                // test frieze AABB
                AABB compareFriezeAABB = pCompareFrieze->getAABB();

                if ( !curFriezeAABB.checkOverlap( compareFriezeAABB, MTH_EPSILON ) )
                    continue;

                // for each polylineRef
                for ( i32 polIdRef = 0; polIdRef < (i32)curFriezeCollisions.size(); polIdRef++ )
                {
                    PolyLine* pCurFriezeCol = curFriezeCollisions[polIdRef];
                    AABBBuilder pCurFriezeColAABB;
                    pCurrentFrieze->computeOrientedGlobalAABB(pCurFriezeColAABB, polIdRef);

                    pCurFriezeColAABB.grow( pCurFriezeColAABB.getMin() -scaleAABB );
                    pCurFriezeColAABB.grow( pCurFriezeColAABB.getMax() +scaleAABB );

                    // for each polyline
                    for ( u32 polId = 0; polId < collisionDataCount; polId++ )
                    {
                        PolyLine* compareFriezeCol = pCompareFrieze->m_pFriezeFullData->m_staticCollisionData[polId];

                        // no connection possible if looping compareFriezeColyline
                        if ( compareFriezeCol->isLooping() )
                            continue;

                        // connection done
                        if ( compareFriezeCol->getNext() )
                            continue;

                        // test col AABB
                        AABBBuilder compareFriezeColAABB;
                        pCompareFrieze->computeOrientedGlobalAABB(compareFriezeColAABB, polId);

                        if ( ! pCurFriezeColAABB.get().checkOverlap( compareFriezeColAABB.get(), MTH_EPSILON ) )
                            continue;

                        const u32 posCount = compareFriezeCol->getPosCount();
                        if ( posCount < 2 )
                            continue;

                        // check previous pCurFriezeCol
                        const Vec2d& poscompareFriezeCol = compareFriezeCol->getPosAt( posCount -1 );

                        if ( Vec2d(pCurFriezeCol->getPosAt(0)).IsEqual( poscompareFriezeCol, deltaConnection ) )
                        {
                            // test length before connect
                            if ( ( Vec2d(pCurFriezeCol->getPosAt(1)) - poscompareFriezeCol ).sqrnorm() < collision_normMinSqr )
                                continue;

                            // set connection
                            _results.push_back(FriezeConnectionResult(compareFriezeCol, poscompareFriezeCol, pCurFriezeCol));

                            curFriezeCollisions.eraseNoOrder(polIdRef);
                            polIdRef--;

                            break;
                        }
                    }
                }
            }
        }
    }

    Vec2d Frise::buildCollision_getCornerExtremityWithCoord( const FriseTextureConfig& _texConfig, collisionRun& _colRun, edgeFrieze* _edgeCur, edgeFrieze* _edgeLast) const
    {
        Vec2d posSquare;

        if ( _edgeCur->m_cornerAngle > 0.f )
        {
            Vec2d cornerOut(_texConfig.m_collisionSimple.m_cornerOut);
            if ( _colRun.m_collisionDouble != None)
                cornerOut.m_y = 1.f -cornerOut.m_y;

            posSquare = getPosOnCornerEdge( _edgeCur, cornerOut.m_y);

            if ( _texConfig.m_collisionSimple.m_cornerOutCur )
                posSquare += _edgeCur->m_normal *cornerOut.m_x *_edgeCur->m_heightStart;
            else
                posSquare += _edgeLast->m_normal *cornerOut.m_x *_edgeLast->m_heightStop;
        }
        else
        {
            Vec2d cornerIn(_texConfig.m_collisionSimple.m_cornerIn);
            if ( _colRun.m_collisionDouble != None )
                cornerIn.m_y = 1.f -cornerIn.m_y;

            posSquare = getPosOnCornerEdge( _edgeCur, 1.f -cornerIn.m_y);

            if ( _texConfig.m_collisionSimple.m_cornerInCur )
                posSquare -= _edgeCur->m_normal *cornerIn.m_x *_edgeCur->m_heightStart;
            else
                posSquare -= _edgeLast->m_normal *cornerIn.m_x *_edgeLast->m_heightStop;
        }

        return posSquare;
    }

    void Frise::buildCollision_ExtremityWithCoord( const FriseConfig* config, SafeArray<Vec2d>& _outline, ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun* _edgeRunCur, collisionRun& _colRun, f32 _offsetCur, f32 _offsetLast, u32 _startEdgeRun ) const
    {     
        // edge cur
        edgeFrieze* edgeCur = &_edgeList[_edgeRunCur->m_idEdgeStart];

        // edge last
        u32 idEdgeLast = ( _edgeRunCur->m_idEdgeStart + m_recomputeData.m_edgeListCount -1) % m_recomputeData.m_edgeListCount;
        edgeFrieze* edgeLast = &_edgeList[idEdgeLast];

        FriseTextureConfig texConfig = config->m_textureConfigs[_edgeRunCur->m_idTex];   

        Vec2d posStart = getPosStopOnNormalEdge( edgeLast, _offsetLast);
        Vec2d posStop = getPosStartOnNormalEdge( edgeCur, _offsetCur);
        Vec2d posSquare = buildCollision_getCornerExtremityWithCoord( texConfig, _colRun, edgeCur, edgeLast);

        if ( _startEdgeRun == btrue )
        {
            _outline.push_back( posSquare );
            _outline.push_back( posStop );
        }
        else if ( _startEdgeRun == bfalse )
        {
            _outline.push_back( posStart );
            _outline.push_back( posSquare );
        }
        else
        {
            _outline.push_back( posStart );
            _outline.push_back( posSquare );
            _outline.push_back( posStop );
        }
    }

    void Frise::buildCollision_CornerSquare_StartEdgeRun( const FriseConfig* _config, ITF_VECTOR<edgeFrieze>& _edgeList, SafeArray<Vec2d>& _outline, edgeFrieze* _edgeCur, u32 _idEdgeStart, f32 _offset, collisionRun& _collisionRun ) const
    {     
        // hole collisions
        if ( _idEdgeStart >0 || isLooping() )
        {
            u32 idEdgeLast = ( _idEdgeStart + m_recomputeData.m_edgeListCount -1) % m_recomputeData.m_edgeListCount;
            const edgeFrieze& edgeLast = _edgeList[idEdgeLast];

            if ( isEdgeWithHoleCollision(edgeLast))
            {
                buildOutline_PosStartOnEdgeWithHoleCollision( _config, _outline, *_edgeCur, _offset);
                return;
            }
        }

        if ( _edgeCur->m_cornerAngle == 0.f )
        {          
            // collision 1
            if ( _config->m_collisionFrieze.m_extremity.m_x != F32_INFINITY )
            {
                Vec2d extremity(_config->m_collisionFrieze.m_extremity);
                if ( _collisionRun.m_collisionDouble != None )
                {
                    extremity.m_y = 1.f -extremity.m_y;
                }
                
                Vec2d col = _edgeCur->m_points[0] -_edgeCur->m_sightNormalized *extremity.m_x *_edgeCur->m_heightStart +_edgeCur->m_normal*extremity.m_y *_edgeCur->m_heightStart;
                _outline.push_back( col );
            }

            // collision 2
            if ( _config->m_collisionFrieze.m_extremity2.m_x != F32_INFINITY )
            {
                Vec2d extremity2(_config->m_collisionFrieze.m_extremity2);
                if ( _collisionRun.m_collisionDouble != None )
                {
                    extremity2.m_y = 1.f -extremity2.m_y;
                }
                
                Vec2d col = _edgeCur->m_points[0] -_edgeCur->m_sightNormalized *extremity2.m_x *_edgeCur->m_heightStart +_edgeCur->m_normal*extremity2.m_y *_edgeCur->m_heightStart;
                _outline.push_back( col );
            }

            _outline.push_back( getPosStartOnNormalEdge( _edgeCur, _offset ) );

            return;
        }

        _outline.push_back( getPosOnCornerEdge( _edgeCur, _offset) );
        _outline.push_back( getPosStartOnNormalEdge( _edgeCur, _offset) );
    }

    void Frise::buildCollision_CornerSquare_StopEdgeRun( const FriseConfig* _config, SafeArray<Vec2d>& _outline, ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idEdgeStop, f32 _offset, collisionRun& _collisionRun ) const
    {     
        const edgeFrieze* edgeLast = &_edgeList[_idEdgeStop];
        u32 idEdgeNext;

        if ( m_pointsList.isLooping() )
            idEdgeNext = ( _idEdgeStop + 1 ) % m_recomputeData.m_edgeListCount;
        else
        {
            if ( _idEdgeStop == m_recomputeData.m_edgeListCount -1 )
            {                                    
                // collision 1
                _outline.push_back( getPosStopOnNormalEdge( edgeLast, _offset));

                // collision 2
                if ( _config->m_collisionFrieze.m_extremity2.m_x != F32_INFINITY )
                {
                    Vec2d extremity2(_config->m_collisionFrieze.m_extremity2);
                    if ( _collisionRun.m_collisionDouble != None )
                    {
                        extremity2.m_y = 1.f -extremity2.m_y;
                    }
                    
                    Vec2d col = edgeLast->m_points[2] +edgeLast->m_sightNormalized *extremity2.m_x *edgeLast->m_heightStop +edgeLast->m_normal*extremity2.m_y *edgeLast->m_heightStop;
                    _outline.push_back( col );
                }

                // collision 3
                if ( _config->m_collisionFrieze.m_extremity.m_x != F32_INFINITY )
                {
                    Vec2d extremity(_config->m_collisionFrieze.m_extremity);
                    if ( _collisionRun.m_collisionDouble != None )
                    {
                        extremity.m_y = 1.f -extremity.m_y;
                    }
                    
                    Vec2d col = edgeLast->m_points[2] +edgeLast->m_sightNormalized *extremity.m_x *edgeLast->m_heightStop +edgeLast->m_normal*extremity.m_y *edgeLast->m_heightStop;
                    _outline.push_back( col );
                }

                return;
            }

            idEdgeNext = _idEdgeStop +1;
        }

        const edgeFrieze* edgeCur = &_edgeList[idEdgeNext];

        // hole collision
        if ( isEdgeWithHoleCollision(*edgeCur))
        {
            buildOutline_PosStopOnEdgeWithHoleCollision( _config, _outline, *edgeLast, _offset, edgeCur->m_idPoint);
            return;
        }

        if ( edgeCur->m_cornerAngle == 0.f )
        {
            _outline.push_back( getPosStartOnNormalEdge( edgeCur, _offset));
            return;
        }

        _outline.push_back( getPosStopOnNormalEdge( edgeLast, _offset) );
        _outline.push_back( getPosOnCornerEdge( edgeCur, _offset));
    }

    u32 Frise::buildCollisionRunListDouble( ITF_VECTOR<collisionRun>& _collisionRunList, FriseConfig* _config ) const
    {
        u32 collisionRunListCount = _collisionRunList.size();

        if ( m_pointsList.isLooping() && collisionRunListCount == 1 && _collisionRunList[0].m_edgeCount == m_recomputeData.m_edgeListCount )
        {
            collisionRun collisionRunDouble = _collisionRunList[0];
            collisionRunDouble.m_collisionDouble = DoubleAndLoop;
            collisionRunDouble.m_index = collisionRunListCount;
            setCollisionRunDoubleOffset(collisionRunDouble);

            _collisionRunList.push_back(collisionRunDouble);

            return 2;
        }

        if ( _config->m_collisionDouble == 1 )
        {
            for ( u32 i = 0; i < collisionRunListCount; i++ )
            {
                collisionRun collisionRunDouble = _collisionRunList[i];
                collisionRunDouble.m_collisionDouble = Double;
                collisionRunDouble.m_index = collisionRunListCount +i;
                setCollisionRunDoubleOffset(collisionRunDouble);

                _collisionRunList.push_back(collisionRunDouble);
            }

            return collisionRunListCount *2;
        }

        for ( u32 i = 0; i < collisionRunListCount; i++ )
        {
            collisionRun collisionRunDouble = _collisionRunList[i];
            collisionRunDouble.m_collisionDouble = DoubleAndLoop;
            setCollisionRunDoubleOffset(collisionRunDouble);

            _collisionRunList.push_back(collisionRunDouble);
        }

        return collisionRunListCount;

    }

    void Frise::setCollisionRunDoubleOffset( collisionRun& _collisionRunDouble ) const
    {
        if ( _collisionRunDouble.m_collisionOffsetLast != -1.f )
        {
            _collisionRunDouble.m_collisionOffsetLast = 1.f -_collisionRunDouble.m_collisionOffsetLast;
        }
        if ( _collisionRunDouble.m_collisionOffsetNext != -1.f )
        {
            _collisionRunDouble.m_collisionOffsetNext = 1.f -_collisionRunDouble.m_collisionOffsetNext;
        }
    }

} // namespace ITF


