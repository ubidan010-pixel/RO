#include "precompiled_engine.h"

#ifndef _ITF_JOB_H_
#include "engine/scheduler/job.h"
#endif //_ITF_JOB_H_

#ifndef _ITF_PARTICULEGENERATOR_H_
#include "engine/display/particle/ParticuleGenerator.h"
#endif //_ITF_PARTICULEGENERATOR_H_

#ifndef _ITF_JOBUPDATEPARTICLE_H_
#include "engine/display/particle/JobUpdateParticle.h"
#endif //_ITF_JOBUPDATEPARTICLE_H_

#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

namespace ITF
{

    u32 JobUpdateParticle::_jobTag_UpdateParticle    = U32_INVALID;

    void JobUpdateParticle::execute(bbool _immediate)
    {
        mainJobUpdateParticle(&m_context,_immediate);
    }

    //------------------------------------------------------------------------------------------
    Seeder randomParticle(666);

    Vec3d getRandomVectorParticle()
    {
        Vec3d v;
        v.m_z = randomParticle.GetFloat( -1.0f, 1.0f );

        f32 r = (float)f32_Sqrt(1 - v.m_z * v.m_z);

        f32 t = randomParticle.GetFloat( -MTH_PI, MTH_PI );

        f32_CosSin(t, &v.m_x, &v.m_y);
        v.m_x *= r;
        v.m_y *= r;

        return v;
    }

    //------------------------------------------------------------------------------------------

    void updateBlend(ITF_Particle* _particle, f32 _elapsedTime)
    {
        f32 f = 0.0f;
        if (_particle->m_phaselifeTime!=0.0f)
        {
             f = _elapsedTime / _particle->m_phaselifeTime;
        }

        ITFCOLOR_BLEND(&_particle->m_curColor.m_color, &_particle->m_initColor.m_color, &_particle->m_dstColor.m_color, f);
        _particle->m_curSize.Lerp(_particle->m_initSize, _particle->m_dstSize, f);
    }

    //------------------------------------------------------------------------------------------

    void updateModeManual( ITF_ParticleGenerator* _pParticleGenerator,f32 _dt )
    {
        ITF_Particle  *pParticle;
        ITF_Particle **ppParticle;

        /// Update our particle system timer...
        _pParticleGenerator->incCurrentTime(_dt);

        /// Start at the head of the active list.
        ppParticle = &_pParticleGenerator->m_pActiveList;

        const f32 currentTime = _pParticleGenerator->getCurrentTime();
        const ParticleGeneratorParameters* pParams = _pParticleGenerator->getParametersConst(); 
        const u32 nbPhase       = pParams->getNumberPhase();

        Texture* pTexture = static_cast<Texture*>( _pParticleGenerator->getParameters()->getTexture().getResource());
        if ((!pTexture) || (!pTexture->isPhysicallyLoaded()))
            return;

#ifdef ITF_SUPPORT_MULTITHREADED
        VertexPCT* pdata = (VertexPCT*)_pParticleGenerator->getCachedMemory();
        const Vec2d scaleUV(pParams->isUseTextureMirrorU() ? 2.f : 1.f, pParams->isUseTextureMirrorV() ? 2.f : 1.f);
#endif // ITF_SUPPORT_MULTITHREADED

        u32 vertexCountToDisplay = 0;
        Vec3d dir;

        while( *ppParticle )
        {
            /// Set a pointer to the head.
            pParticle = *ppParticle;

            if (pParticle->m_bAlive)
            {
                /// Calculate new position
                f32 elapsedTime = currentTime - pParticle->m_initTime;

                if ( elapsedTime >= pParticle->m_phaselifeTime )
                {
                    pParticle->m_curPhase++;

                    if (pParticle->m_curPhase == nbPhase && (pParticle->m_flag & PAR_F_LOOP))
                    {
                        pParticle->m_curPhase = 0;
                    }
                    _pParticleGenerator->getParameters()->initToNextPhase(pParticle, currentTime, _pParticleGenerator);
                }

                {
                    /// blend.
                    if (pParticle->m_flag & PAR_F_BLENDPHASE)
                        updateBlend(pParticle, elapsedTime);

                    if (pParticle->m_flag & PAR_F_ORIENTDIR)
                    {
                        /// Orient to dir.
                        dir = pParticle->m_vel;
                        dir.normalize();
                        pParticle->m_angle.SetRadians(atan2f(dir.m_y, dir.m_x));
                    }

                    #ifdef ITF_SUPPORT_MULTITHREADED
                    _pParticleGenerator->fillNormal(pParticle,pTexture,pdata,scaleUV);
                    pdata += 4;
                    #endif // ITF_SUPPORT_MULTITHREADED
                    vertexCountToDisplay ++;
                }

                if (pParticle->m_flag & PAR_F_USEANIM)
                    _pParticleGenerator->updateParticleAnim(pParticle, elapsedTime);

            }
            ppParticle = &pParticle->m_pNext;
        }

        _pParticleGenerator->setParticlesCountVtxToDisplay( vertexCountToDisplay);
    }

    //------------------------------------------------------------------------------------------

    void updateModeFollow(ITF_ParticleGenerator* _pParticleGenerator,f32 _dt, bbool _bDisableCreation)
    {
        ITF_Particle* __restrict pParticle = NULL;
        Vec3d Vel;

        /// Update our particle system timer...
        _pParticleGenerator->incCurrentTime(_dt);

        // Start at the head of the active list
        const f32 currentTime = _pParticleGenerator->getCurrentTime();
        const ParticleGeneratorParameters* pParams = _pParticleGenerator->getParametersConst();     
        const u32 nbPhase       = pParams->getNumberPhase();
        Texture* pTexture = static_cast<Texture*>( _pParticleGenerator->getParameters()->getTexture().getResource());
        if ((!pTexture) || (!pTexture->isPhysicallyLoaded()))
            return;

        #ifdef ITF_SUPPORT_MULTITHREADED
        VertexPCT* pdata = (VertexPCT*)_pParticleGenerator->getCachedMemory();
        const Vec2d scaleUV(pParams->isUseTextureMirrorU() ? 2.f : 1.f, pParams->isUseTextureMirrorV() ? 2.f : 1.f);
        #endif // ITF_SUPPORT_MULTITHREADED

        u32 vertexCountToDisplay = 0;

        const f32 particlesToEmitExact = (currentTime - _pParticleGenerator->m_lastUpdateTime) / _pParticleGenerator->getParameters()->getFrequency() + _pParticleGenerator->m_lastUpdateRest;
        u32 NumParticlesToEmit = (u32)particlesToEmitExact;

        if (_bDisableCreation || (!_pParticleGenerator->m_generationStarted))
            NumParticlesToEmit = 0;

        if (_bDisableCreation)
        {
            _pParticleGenerator->m_lastUpdateTime = currentTime;
            _pParticleGenerator->m_lastUpdateRest = particlesToEmitExact - NumParticlesToEmit;
        }

        if( NumParticlesToEmit > 0 )
        {
            _pParticleGenerator->m_lastUpdateTime = currentTime;
            _pParticleGenerator->m_lastUpdateRest = particlesToEmitExact - NumParticlesToEmit;

            if (_pParticleGenerator->m_totalParticlesGenerated > pParams->getNumToEmit())
                return;

            NumParticlesToEmit = Min(pParams->getNumToEmit()-_pParticleGenerator->m_totalParticlesGenerated,NumParticlesToEmit);

            for( u32 i = 0; i < NumParticlesToEmit; ++i )
            {
                if( _pParticleGenerator->m_activeParticlesCount < _pParticleGenerator->m_maxParticlesNeed)
                {
                    pParticle = _pParticleGenerator->allocateParticle();

                    _pParticleGenerator->initNewParticleFollow(pParticle);

                    ++_pParticleGenerator->m_activeParticlesCount;
                    ++_pParticleGenerator->m_totalParticlesGenerated;
                }
            }
        }

#ifdef ITF_X360
        for( int i = 0; i < 1024; i += 128 )
            __dcbt( i, _pParticleGenerator->m_pParticleArrayIndex );
#endif //ITF_X360

        for (u32 index = 0;index<_pParticleGenerator->m_maxParticlesNeed;++index)
        {
            pParticle = &_pParticleGenerator->m_pParticleArrayIndex[index];

#ifdef ITF_X360
            for( int i = 0; i < 4096; i += 128 )
                __dcbt( i+index*sizeof(ITF_Particle), _pParticleGenerator->m_pParticleArrayIndex  );
#endif //ITF_X360

            /// Set a pointer to the head
            if (pParticle->m_bAlive)
            {
                /// Calculate new position
                f32 elapsedTime =  currentTime - pParticle->m_initTime;

                if( elapsedTime >= pParticle->m_phaselifeTime )
                {
                    pParticle->m_curPhase++;
                    if (pParticle->m_curPhase == nbPhase && (pParticle->m_flag & PAR_F_LOOP))
                        pParticle->m_curPhase = 0;
                    if (pParticle->m_curPhase == nbPhase)
                    {
                        _pParticleGenerator->deallocateParticle(pParticle); 
                        --_pParticleGenerator->m_activeParticlesCount;
                        continue;
                    }
                    else
                    {
                        _pParticleGenerator->getParameters()->initToNextPhase(pParticle, currentTime, _pParticleGenerator);
                        elapsedTime =  currentTime - pParticle->m_initTime;
                    }
                }

                pParticle->m_curtimeTarget -= _dt;

                /// new target ??.
                if (pParticle->m_curtimeTarget < 0.f)
                {
                    Vec3d randomVec = getRandomVectorParticle();
                    pParticle->m_vel = pParticle->m_velTarget;
                    pParticle->m_velTarget = randomVec * randomParticle.GetFloat(0.1f, 1.f);
                    pParticle->m_timeTarget = randomParticle.GetFloat(3.5, 14.f);
                    pParticle->m_curtimeTarget = pParticle->m_timeTarget;
                }

                f32 r = (pParticle->m_timeTarget - pParticle->m_curtimeTarget) / pParticle->m_timeTarget;
                Vel.m_x = f32_Lerp(pParticle->m_velTarget.m_x, pParticle->m_vel.m_x , r);
                Vel.m_y = f32_Lerp(pParticle->m_velTarget.m_y, pParticle->m_vel.m_y , r);
                Vel.m_z = 0.f;

                pParticle->m_pos += Vel * _dt;

                pParticle->m_angle += pParticle->m_angularSpeed * _dt;

                /// blend.
                if (pParticle->m_flag & PAR_F_BLENDPHASE)
                    updateBlend(pParticle, elapsedTime);

                if (pParticle->m_flag & PAR_F_USEANIM)
                    _pParticleGenerator->updateParticleAnim(pParticle, elapsedTime);

                _pParticleGenerator->SetUVFromUVmode(pParticle);

                if (_pParticleGenerator->isComputeAABB())
                    _pParticleGenerator->updateAABB(pParticle);

                #ifdef ITF_SUPPORT_MULTITHREADED
                _pParticleGenerator->fillNormal(pParticle,pTexture,pdata, scaleUV);
                pdata += 4;
                #endif // ITF_SUPPORT_MULTITHREADED
                vertexCountToDisplay ++;
            }
        }

        _pParticleGenerator->setParticlesCountVtxToDisplay( vertexCountToDisplay);

        //if( m_currentTime - m_lastUpdateTime > m_emitInterval )
    }

    ///----------------------------------------------------------------------------//

    void updateModeComplex(ITF_ParticleGenerator* _pParticleGenerator,f32 _dt, bbool _bDisableCreation)
    {
        ITF_Particle* __restrict pParticle;

        /// Update our particle system timer...
        _pParticleGenerator->incCurrentTime(_dt);

        const f32 currentTime   = _pParticleGenerator->getCurrentTime();
        const ParticleGeneratorParameters* pParams = _pParticleGenerator->getParametersConst();     
        const u32 nbPhase       = pParams->getNumberPhase();
        const Vec2d& pivot      = pParams->getPivot();    
        Texture* pTexture = static_cast<Texture*>( _pParticleGenerator->getParameters()->getTexture().getResource());
        if ((!pTexture) || (!pTexture->isPhysicallyLoaded()))
            return;

        #ifdef ITF_SUPPORT_MULTITHREADED
        VertexPCT* pdata = (VertexPCT*)_pParticleGenerator->getCachedMemory();
        const Vec2d scaleUV(pParams->isUseTextureMirrorU() ? 2.f : 1.f, pParams->isUseTextureMirrorV() ? 2.f : 1.f);
        #endif // ITF_SUPPORT_MULTITHREADED

        u32 vertexCountToDisplay = 0;

        const f32 particlesToEmitExact = ((currentTime - _pParticleGenerator->m_lastUpdateTime) / _pParticleGenerator->getParameters()->getFrequency()) + _pParticleGenerator->m_lastUpdateRest;
        u32 NumParticlesToEmit = (u32)particlesToEmitExact;

        if (_bDisableCreation || (!_pParticleGenerator->m_generationStarted))
            NumParticlesToEmit = 0;

        if (_bDisableCreation)
        {
            _pParticleGenerator->m_lastUpdateTime = currentTime;
            _pParticleGenerator->m_lastUpdateRest = particlesToEmitExact - NumParticlesToEmit;
        }

        if( NumParticlesToEmit > 0 )
        {
            _pParticleGenerator->m_lastUpdateTime = currentTime;
            _pParticleGenerator->m_lastUpdateRest = particlesToEmitExact - (f32)NumParticlesToEmit;

            if (_pParticleGenerator->m_totalParticlesGenerated <= pParams->m_emitParticlesCount)
            {
                NumParticlesToEmit = Min(pParams->m_emitParticlesCount-_pParticleGenerator->m_totalParticlesGenerated,NumParticlesToEmit);

                for( u32 i = 0; i < NumParticlesToEmit; ++i )
                {
                    _pParticleGenerator->addOneNewComplexParticle();
                }
            }
        }

#ifdef ITF_X360
        for( int i = 0; i < 1024; i += 128 )
            __dcbt( i, _pParticleGenerator->m_pParticleArrayIndex );
#endif //ITF_X360

        for (u32 index = 0;index<_pParticleGenerator->m_maxParticlesNeed;++index)
        {
            pParticle = &_pParticleGenerator->m_pParticleArrayIndex[index];

#ifdef ITF_X360
            for( int i = 0; i < 4096; i += 128 )
                __dcbt( i+index*sizeof(ITF_Particle), _pParticleGenerator->m_pParticleArrayIndex  );
#endif //ITF_X360

            if (pParticle->m_bAlive)
            {
                STATS_PARTICULE_COMPLEXACTIVEINC

                /// Calculate new position
                f32 elapsedTime = currentTime - pParticle->m_initTime;

                if ( elapsedTime >= pParticle->m_phaselifeTime )
                {
                    pParticle->m_curPhase++;
                    if (pParticle->m_curPhase == nbPhase && (pParticle->m_flag & PAR_F_LOOP))
                        pParticle->m_curPhase = 0;
                    if (pParticle->m_curPhase == nbPhase)
                    {
                        _pParticleGenerator->deallocateParticle(pParticle); 
                        --_pParticleGenerator->m_activeParticlesCount;
                        continue;
                    }
                    else
                    {
                        _pParticleGenerator->getParameters()->initToNextPhase(pParticle, currentTime, _pParticleGenerator);
                        elapsedTime = currentTime - pParticle->m_initTime;
                    }
                }

                pParticle->m_vel += (pParams->getGravity() + pParams->getAcc()) * _dt;
                pParticle->m_vel *= pParams->getFriction();

                if (pParams->isCartoon())
                    pParticle->m_vel.m_z = 0.f;
                pParticle->m_pos += pParticle->m_vel * _dt;

                if (pParticle->m_flag & PAR_F_ORIENTDIR)
                {
                    /// Orient to dir.
                    Vec3d dir = pParticle->m_vel;
                    dir.normalize();
                    pParticle->m_angle.SetRadians(atan2f(dir.m_y, dir.m_x));
                    
                    if (pParams->CanFlipOrientDir() && pParams->isFlip())
                    {
                        pParticle->m_angle -= MTH_PI;
                    }
                }
                else
                {
                    //if (pParticle->m_angularSpeed != 0.0f )
                    pParticle->m_angle += pParticle->m_angularSpeed * _dt;
                }

                /// blend.
                if (pParticle->m_flag & PAR_F_BLENDPHASE)
                    updateBlend(pParticle, elapsedTime);

                /// compute pivot.
                Vec2d::Mul(&pParticle->m_pivot, &pParticle->m_curSize, &pivot);

                /// todo: collision ??.

                if (pParticle->m_flag & PAR_F_USEANIM)
                    _pParticleGenerator->updateParticleAnim(pParticle, elapsedTime);

                _pParticleGenerator->SetUVFromUVmode(pParticle);

                if (_pParticleGenerator->isComputeAABB())
                    _pParticleGenerator->updateAABB(pParticle);

                #ifdef ITF_SUPPORT_MULTITHREADED
                _pParticleGenerator->fillNormal(pParticle,pTexture,pdata, scaleUV);
                pdata += 4;
                #endif // ITF_SUPPORT_MULTITHREADED
                vertexCountToDisplay ++;
            }
        }

        _pParticleGenerator->setParticlesCountVtxToDisplay( vertexCountToDisplay);
    }

    //------------------------------------------------------------------------------------------
    
    void  JobUpdateParticle::synchronize()
    {
        m_context.m_pParticleGenerator->synchronize();
    }

    void mainJobUpdateParticle(JobUpdateParticleContext* _context,bbool _immediate)
    {
        STATS_PARTICULE_UPDATEINC

        ITF_ParticleGenerator*    pParticleGenerator = _context->m_pParticleGenerator;

        if (pParticleGenerator->getTrueAABB() || pParticleGenerator->getParameters()->isUseMatrix())
        {
            //init working bounding box with generator pos
            pParticleGenerator->getParameters()->setWorkingBoundingBox(pParticleGenerator->getParameters()->getPosition());
        }

        f32 dt = _context->m_deltaTime;
        bbool disableCreation = _context->m_disableCreation;

        switch(pParticleGenerator->getParameters()->getGeneratorMode())
        {
        case PARGEN_MODE_FOLLOW: 
            updateModeFollow(pParticleGenerator,dt, disableCreation);
            break;
        case PARGEN_MODE_MANUAL:
            updateModeManual(pParticleGenerator,dt);
            break;
        case PARGEN_MODE_COMPLEX:
        default:
            updateModeComplex(pParticleGenerator,dt, disableCreation);
            break;
        }

        if (pParticleGenerator->getParameters()->isUseMatrix())
        {
            pParticleGenerator->transformBoundingBoxWithMatrix();
        }

        if (_immediate)
            pParticleGenerator->synchronize();
    }
}
