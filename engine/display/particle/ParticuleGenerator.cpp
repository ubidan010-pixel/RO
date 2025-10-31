#include "precompiled_engine.h"

#include <algorithm>

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_PARTICULEGENERATOR_H_
#include "engine/display/particle/ParticuleGenerator.h"
#endif //_ITF_PARTICULEGENERATOR_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_GFXADAPTER_RENDERTARGET_H_
#include "engine/AdaptersInterfaces/GFXAdapter_RenderTarget.h"
#endif //_ITF_GFXADAPTER_RENDERTARGET_H_

#ifndef _ITF_PARTICULEGENERATORPARAMETERS_H_
#include "engine/display/particle/ParticuleGeneratorParameters.h"
#endif //_ITF_PARTICULEGENERATORPARAMETERS_H_

#ifndef _ITF_JOB_H_
#include "engine/scheduler/job.h"
#endif //_ITF_JOB_H_

#ifndef _ITF_JOBUPDATEPARTICLE_H_
#include "engine/display/particle/JobUpdateParticle.h"
#endif //_ITF_JOBUPDATEPARTICLE_H_

#ifndef _ITF_SCHEDULER_H_
#include "engine/scheduler/scheduler.h"
#endif //_ITF_SCHEDULER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif // _ITF_CAMERA_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

#ifdef ITF_WII
#ifndef _ITF_WII_DRAWPRIM_H_
#include "adapters/GFXAdapter_WII/GFXAdapter_WII.h"
#endif // _ITF_WII_DRAWPRIM_H_
#endif // ITF_WII

#define USEVB
#define USE_INTERNAL_SPRITE_INDEX_VB

namespace ITF
{

///----------------------------------------------------------------------------//

    static i32 msSeed = 1376312589;

    inline i32 GenerateSeed()
    {
	    // A very, VERY crude LCG but good enough to generate
	    // a nice range of seed values
	    msSeed = (msSeed * 0x015a4e35L) + 1;
	    msSeed = (msSeed>>16)&0x7fff;
	    return msSeed;
    }   

    RandomGenerator::RandomGenerator()
    {
	    // Don't call this in the base class!!! Creates "pure virtual function call" error!
    //	SetNewSeed();
    }

    RandomGenerator::~RandomGenerator()
    {
    }
    void RandomGenerator::SetNewSeed()
    {
	    SetSeed(GenerateSeed());
    }

    const i32 RandomLCG::msQuotient  = I32_MAX / 16807L;
    const i32 RandomLCG::msRemainder = I32_MAX % 16807L;

    RandomLCG::RandomLCG()
    {
	    SetNewSeed();
    }

    RandomLCG::RandomLCG(i32 s)
    {
	    SetSeed(s);
    }

    void RandomLCG::SetSeed(i32 s)
    {
	    mSeed = s;
    }

    u32 RandomLCG::RandI() const
    {
	    if(mSeed <= msQuotient)
		    mSeed = (mSeed * 16807L) % I32_MAX;
	    else
	    {
		    const i32 high_part = mSeed / msQuotient;
		    const i32 low_part  = mSeed % msQuotient;

		    const i32 test = (16807L * low_part) - (msRemainder * high_part);

		    if(test>0)	mSeed = test;
		    else		mSeed = test + I32_MAX;
	    }
	    return mSeed; 
    }

//random generators.
//Seeder random(666);
    static RandomLCG randomLCG;

ITF_ParticleGenerator ITF_ParticleGenerator::null_object;

///----------------------------------------------------------------------------//

SafeArray<ITF_ParticleGenerator::ParticleGeneratorPoolEntry> ITF_ParticleGenerator::m_pool;
SafeArray<u32> ITF_ParticleGenerator::m_indexesToFree;
SafeArray<u32> ITF_ParticleGenerator::m_freeIndexes;
#ifdef PARTICLE_GENERATOR_USE_PLACEMENT_NEW
    static u8* m_poolMemory = NULL;
#endif

void ITF_ParticleGenerator::initPool()
{
#ifdef PARTICLE_GENERATOR_USE_PLACEMENT_NEW
    if (m_poolMemory)
        return;
    m_poolMemory = (u8*)Memory::mallocCategory(sizeof(ITF_ParticleGenerator) * m_poolSize, MemoryId::mId_Particle);
#endif //PARTICLE_GENERATOR_USE_PLACEMENT_NEW

    for (u32 i = 0; i < m_poolSize; i++)
    {
        ParticleGeneratorPoolEntry entry;
        #ifdef PARTICLE_GENERATOR_USE_PLACEMENT_NEW
            entry.m_gen = (ITF_ParticleGenerator*)&m_poolMemory[i * sizeof(ITF_ParticleGenerator)];
        #else
            entry.m_gen = NULL;
        #endif
        entry.m_unavailableCounter = ParticleGeneratorPoolEntry::counter_available;
        m_pool.push_back(entry);
        m_freeIndexes.push_back(i);
    }
}

void ITF_ParticleGenerator::freePool()
{
    while(m_indexesToFree.size())
        FlushGenerators();
#ifdef PARTICLE_GENERATOR_USE_PLACEMENT_NEW
    if (m_poolMemory)
    {
        Memory::free(m_poolMemory);
        m_poolMemory = NULL;
    }
#endif
}

ITF_ParticleGenerator* ITF_ParticleGenerator::getGenerator(i32& _entryIndex)
{
    if (m_freeIndexes.size())
    {
        i32 index = (i32)m_freeIndexes.back();
        m_freeIndexes.pop_back();
        ITF_ASSERT(ParticleGeneratorPoolEntry::counter_available == m_pool[index].m_unavailableCounter);
        m_pool[index].m_unavailableCounter = ParticleGeneratorPoolEntry::counter_allocated;
        _entryIndex = index;
        #ifdef PARTICLE_GENERATOR_USE_PLACEMENT_NEW
            new((void*)m_pool[index].m_gen) ITF_ParticleGenerator(); // call placement new
            m_pool[index].m_gen->construct();
        #else
            if (!m_pool[index].m_gen)
                m_pool[index].m_gen = newAlloc(mId_System, ITF_ParticleGenerator());
        #endif
        #ifdef DEBUG_PARTICLE_POOL
            LOG("[PARTICLE POOL] Alloc index %d ptr %d at frame %d", index, m_pool[index].m_gen, CURRENTFRAME);
        #endif
        return m_pool[index].m_gen;
    }
    _entryIndex = -1;
    ITF_ASSERT_MSG(bfalse, "please increase m_poolSize");
    return NULL;
}

void ITF_ParticleGenerator::freeGenerator(ITF_ParticleGenerator* _gen, i32 _entryIndex)
{
    i32 foundIndex = -1;
    if (_entryIndex >= 0)
    {
        if (m_pool[_entryIndex].m_gen == _gen)
        {
            foundIndex = _entryIndex;
        }
    }
    else
    {
        for (u32 i = 0; i < m_poolSize; i++)
        {
            if (m_pool[i].m_gen == _gen)
            {
                foundIndex = (i32)i;
                break;
            }
        }
    }
    ITF_ASSERT_MSG(foundIndex >= 0, "pool not found");
    if (foundIndex >= 0)
    {
        m_pool[foundIndex].m_unavailableCounter = 3;
        m_indexesToFree.push_back(foundIndex);
        #ifdef DEBUG_PARTICLE_POOL
            LOG("[PARTICLE POOL] set to be freed index %d ptr %d at frame %d", foundIndex, m_pool[foundIndex].m_gen, CURRENTFRAME);
        #endif
    }
}


void ITF_ParticleGenerator::FlushGenerators()
{    
    for (i32 i=(i32)m_indexesToFree.size()-1 ; i>=0 ; --i)
    {
        u32 index = m_indexesToFree[i];
        if (m_pool[index].m_unavailableCounter != ParticleGeneratorPoolEntry::counter_available && m_pool[index].m_unavailableCounter != ParticleGeneratorPoolEntry::counter_allocated)
        {
            m_pool[index].m_unavailableCounter--;
            if (m_pool[index].m_unavailableCounter == ParticleGeneratorPoolEntry::counter_available)
            {
                #ifdef PARTICLE_GENERATOR_USE_PLACEMENT_NEW
                    m_pool[index].m_gen->~ITF_ParticleGenerator();                    
                #else
                    SF_DEL(m_pool[index].m_gen);
                #endif
                m_indexesToFree.eraseNoOrder(i);
                m_freeIndexes.push_back(index);
//                LOG("[PARTICLE POOL] really freed (cleared) index %d at frame %d", index, CURRENTFRAME);
            }
        }
    }
#ifdef DEBUG_PARTICLE_POOL
    u32 count = 0;
    for (u32 i = 0; i < m_poolSize; i++)
    {
        if (ParticleGeneratorPoolEntry::counter_available == m_pool[i].m_unavailableCounter)
        {
            ITF_ASSERT(m_freeIndexes.find(i) >= 0);
        }
        else
        {
            ITF_ASSERT(m_freeIndexes.find(i) < 0);
            count++;
        }
    }
    ITF_ASSERT( m_poolSize - count == m_freeIndexes.size());
#endif
}

///----------------------------------------------------------------------------//

#ifdef PARTICLE_GENERATOR_USE_PLACEMENT_NEW
    void* ITF_ParticleGenerator::operator new(size_t _size, void* _adrs)
    {
        // just fill vf table
        return _adrs;
    }
#endif

ITF_ParticleGenerator::ITF_ParticleGenerator()
{
    construct();
}

void ITF_ParticleGenerator::construct()
{
    m_template = NULL;
    m_params.setGeneratorMode(PARGEN_MODE_COMPLEX);
    m_params.setGeneratorGenType(PARGEN_GEN_POINTS);
    m_particlesCountVtx     = 0;
    m_particlesCountVtxToDisplay = 0;
    m_pActiveList			= 0;
    m_particleArray         = 0;
	m_activeParticlesCount  = 0;
	m_currentTime			= 0.0f;
	m_lastUpdateTime			= 0.0f;
    m_lastUpdateRest        = 0.0f;
    m_mesh.m_vtxBuffer		= 0;
    m_params.setNumberPhase(1);
    m_maxParticlesNeed      = 0;

#ifdef ITF_SUPPORT_MULTITHREADED
    m_cacheMemory           = NULL;
#endif // ITF_SUPPORT_MULTITHREADED

    m_params.setBoxMin(Vec2d(-10,-10));
    m_params.setBoxMax(Vec2d(10,10));

    m_ambColor          = Color::white();
    m_globalColor       = Color::white();
    m_factorColor       = Color::white();
    m_alpha             = 1.f;

    m_params.setAABBMin(Vec2d(1.f, 1.f));
    m_params.setAABBMax(Vec2d(1.f, 1.f));

    m_generationStarted = btrue;

    m_totalParticlesGenerated = 0;

    m_renderTargetImpostor = NULL;

    m_particlesZOrdering = NULL;
    
    m_isRender = btrue;
    m_jobParticle = newAlloc(mId_Particle,JobUpdateParticle());
    m_jobParticle->setAsManaged(btrue);
    m_pParticleArrayIndex = NULL;
    m_checkValidity = bfalse;    

    m_aabbNeedCompute = btrue;

    m_spliterAngle = NULL;
    m_SplitCurPos = 0;

    m_LocalRotationZ = 0.f;
    m_LocalScale = Vec2d::One;
    m_transformMatrix = GMatrixIdentity;
    m_renderInTarget = 0;
}
///----------------------------------------------------------------------------//

ITF_ParticleGenerator::~ITF_ParticleGenerator()
{
    destruct();
}

void ITF_ParticleGenerator::destruct()
{
    clear();

    Job::destroy(m_jobParticle); 
    m_jobParticle = NULL;
}


///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::clear()
{
    //LOG("ITF_ParticleGenerator::clear %d", this);
    SF_DEL_ARRAY(m_pParticleArrayIndex);

#ifdef ITF_SUPPORT_MULTITHREADED
    if (m_cacheMemory)
        delete[] m_cacheMemory;
#endif // ITF_SUPPORT_MULTITHREADED

#ifndef VBMANAGER_USE_DYNAMICRING_VB
    GFX_ADAPTER->removeVertexBufferDelay(m_mesh.m_vtxBufferBuffered[0],1);
    GFX_ADAPTER->removeVertexBufferDelay(m_mesh.m_vtxBufferBuffered[1],1);
    m_mesh.m_vtxBufferBuffered[0] = NULL;
    m_mesh.m_vtxBufferBuffered[1] = NULL;
#else
    GFX_ADAPTER->removeVertexBuffer(m_mesh.m_vtxBuffer);
    m_mesh.m_vtxBuffer = NULL;
#endif // VBMANAGER_USE_DYNAMICRING_VB    

#ifndef USE_INTERNAL_SPRITE_INDEX_VB
    for (u32 i = 0; i < m_mesh.m_ElementList.size(); i++)
    {
        if(m_mesh.m_ElementList[i].m_indexBuffer) 
            GFX_ADAPTER->removeIndexBufferDelay(m_mesh.m_ElementList[i].m_indexBuffer,1);
    }
#endif

    if (m_renderTargetImpostor)
        GFX_ADAPTER->deleteRenderTarget(m_renderTargetImpostor);

    SF_DEL(m_particlesZOrdering);
    SF_DEL_ARRAY(m_spliterAngle);

    if ( m_particleArray )
    {
        delete []m_particleArray;
    }
    else
    {
        while( m_pActiveList )
        {
            ITF_Particle* pParticle = m_pActiveList;
            m_pActiveList = pParticle->m_pNext;
            delete pParticle;
        }
    }

    m_particleArray = NULL;
    m_pActiveList = NULL;

    m_checkValidity = bfalse;
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::setMaxParticles( u32 _maxParticles )
{
    if (!GFX_ADAPTER)
        return;

//    LOG("ITF_ParticleGenerator::setMaxParticles %d", this);
    u32 replicate = 1;

    SF_DEL_ARRAY(m_pParticleArrayIndex);
    m_pFreeParticleIndex.clear();

    if (_maxParticles == 0)
        _maxParticles = 1;

#ifdef ITF_WII
    m_params.setMaxParticles(std::min(_maxParticles, (u32)1000));
#else // ITF_WII
    m_params.setMaxParticles(std::min(_maxParticles, (u32)10000));
#endif // ITF_WII

    if (m_params.getFrequency()>0.000001f)
    {
        m_maxParticlesNeed = (u32)ceil( m_params.computeSumPhaseTime() * m_params.getLifeTimeMultiplier() / m_params.getFrequency() );
        m_maxParticlesNeed = std::min(m_maxParticlesNeed, m_params.getMaxParticles());
    }
    else
    {
        m_maxParticlesNeed = m_params.getMaxParticles();
    }
    ITF_ASSERT(m_maxParticlesNeed > 0);

    m_pParticleArrayIndex = newAlloc(mId_Particle,ITF_Particle[m_maxParticlesNeed]);
#ifndef ITF_FINAL
    if (!m_pParticleArrayIndex)
    {
        ITF_FATAL_ERROR("out memory for particle %d %f",m_maxParticlesNeed,m_params.getFrequency());
    }
#endif //ITF_FINAL

    if (m_params.isCartoon())
    {
        if (m_particlesZOrdering)
            delete m_particlesZOrdering;
        m_particlesZOrdering = newAlloc(mId_Particle, PAR_Zlist);
        m_particlesZOrdering->setSize(_maxParticles*2);
        replicate = 2;
    }

    /// Destroy VB.
#ifndef VBMANAGER_USE_DYNAMICRING_VB
    GFX_ADAPTER->removeVertexBuffer(m_mesh.m_vtxBufferBuffered[0]);
    GFX_ADAPTER->removeVertexBuffer(m_mesh.m_vtxBufferBuffered[1]);
#else    
    GFX_ADAPTER->removeVertexBuffer(m_mesh.m_vtxBuffer);
    m_mesh.m_vtxBuffer = NULL;
#endif // VBMANAGER_USE_DYNAMICRING_VB

    /// Create VB.
    m_particlesCountVtx = m_maxParticlesNeed * 4 * replicate;

#ifdef ITF_SUPPORT_MULTITHREADED
    if (m_cacheMemory)
        delete[] m_cacheMemory;

    m_cacheMemory = newAlloc(mId_Particle, u8[m_particlesCountVtx*sizeof(VertexPCT)]);//TODO use a pool  
#endif // ITF_SUPPORT_MULTITHREADED

#ifndef VBMANAGER_USE_DYNAMICRING_VB
	m_mesh.m_vtxBufferBuffered[0] = GFX_ADAPTER->createVertexBuffer(m_particlesCountVtx , VertexFormat_PCT, sizeof(VertexPCT), 1, VB_T_PARTICLE);
    ITF_ASSERT(m_mesh.m_vtxBufferBuffered[0]);
    m_mesh.m_vtxBufferBuffered[1] = GFX_ADAPTER->createVertexBuffer(m_particlesCountVtx , VertexFormat_PCT, sizeof(VertexPCT), 1, VB_T_PARTICLE);
    ITF_ASSERT(m_mesh.m_vtxBufferBuffered[1]);
#else
    m_mesh.m_vtxBuffer = GFX_ADAPTER->createVertexBuffer(m_particlesCountVtx , VertexFormat_PCT, sizeof(VertexPCT), 1, VB_T_PARTICLE);
#endif // VBMANAGER_USE_DYNAMICRING_VB


    /// Create Element.
    if (!m_mesh.m_ElementList.size())
    {
        ITF_MeshElement elem;
		elem.m_startIndex = 0;
		elem.m_count = 0;
		m_mesh.m_ElementList.push_back(elem);
    }

#ifdef USE_INTERNAL_SPRITE_INDEX_VB
    m_mesh.m_ElementList[0].m_indexBuffer = GFX_ADAPTER->needSpriteIndexBuffer(m_maxParticlesNeed * 6 * replicate);
#else
    if(m_mesh.m_ElementList[0].m_indexBuffer) 
    GFX_ADAPTER->removeIndexBuffer(m_mesh.m_ElementList[0].m_indexBuffer);
    
    m_mesh.m_ElementList[0].m_indexBuffer = GFX_ADAPTER->createIndexBuffer(m_maxParticlesNeed * 6 * replicate , bfalse);
    
    /// Fill Element index Buffer.
    u32 i;
	u16* index;

	m_mesh.m_ElementList[0].m_indexBuffer->Lock((void**)&index);
	u16 vertex = 0;

    for (i = 0; i < m_maxParticlesNeed * replicate ; i++)
	{
		index[i * 6 + 0] = vertex + 0;
		index[i * 6 + 1] = vertex + 1;
		index[i * 6 + 2] = vertex + 2;
		index[i * 6 + 3] = vertex + 2;
		index[i * 6 + 4] = vertex + 3;
		index[i * 6 + 5] = vertex + 0;
		vertex += 4;
	}

	m_mesh.m_ElementList[0].m_indexBuffer->Unlock();
#endif
 
    reInit();
}


///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::init( const ITF_ParticleGenerator_Template* _template )
{
    m_template = _template;
    m_useAnim = m_template->getUseAnim();
    m_computeAABB = m_template->getComputeAABB();
    
    if (m_computeAABB)
        getParameters()->setCanFlipBoundingBox(bfalse);

    BinaryClone(const_cast<ParticleGeneratorParameters*>(&m_template->getParameters()),&m_params);

    u32 nbphase = m_params.m_phaseList.size();
    if (m_params.m_nbPhase > nbphase)
        m_params.m_nbPhase = nbphase;

    setMaxParticles(m_template->getParameters().getMaxParticles());

    Texture* defaultTex = (Texture*)RESOURCE_MANAGER->getTextureAt(0);
    if (defaultTex)
        m_params.setTexture(defaultTex->getID());
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::reInit()
{
    m_pFreeParticleIndex.clear();
    for (i32 index = m_maxParticlesNeed-1;index>=0 ;index--)
    {
        m_pFreeParticleIndex.push_back(index);
    }

    for (u32 index = 0;index<m_maxParticlesNeed;++index)
    {
        ITF_Particle  *pParticle = &m_pParticleArrayIndex[index];
        pParticle->dead();
    }
    
    m_activeParticlesCount = 0;

    m_globalColor = m_params.getDefaultColor();
}

///----------------------------------------------------------------------------//

void Quad_ToVB(VertexPCT* __restrict  data,Angle _angle,u32 _color,const Vec3d& _pos,const Vec2d& _currentSize, const UVdata& m_uvData, Vec2d& _scaleUV)
{
/*  original code
    f32 rayonx = _currentSize.m_x * 0.5f;
    f32 rayony = _currentSize.m_y * 0.5f;
    f32 c,s;
    f32_CosSin(_angle.Radians(), &c, &s);

    f32 rayonxc = rayonx * c;
    f32 rayonys = rayony * s;
    f32 rayonxs = rayonx * s;
    f32 rayonyc = rayony * c;

    data[0].m_pos.set( -rayonxc - rayonys + _pos.m_x, -rayonxs + rayonyc + _pos.m_y, _pos.m_z);
    data[0].m_color = _color;
    data[0].m_uv = m_uvData.getUV0() * _scaleUV;

    data[1].m_pos.set( -rayonxc + rayonys  + _pos.m_x, -rayonxs - rayonyc + _pos.m_y, _pos.m_z);
    data[1].m_color = _color;
    data[1].m_uv.set(m_uvData.getUV0().m_x * _scaleUV.m_x, m_uvData.getUV1().m_y * _scaleUV.m_y);

    data[2].m_pos.set( rayonxc + rayonys + _pos.m_x, rayonxs - rayonyc + _pos.m_y, _pos.m_z);
    data[2].m_color = _color;
    data[2].m_uv = m_uvData.getUV1() * _scaleUV;

    data[3].m_pos.set( rayonxc - rayonys + _pos.m_x, rayonxs + rayonyc + _pos.m_y , _pos.m_z);
    data[3].m_color = _color;
    data[3].m_uv.set(m_uvData.getUV1().m_x * _scaleUV.m_x, m_uvData.getUV0().m_y * _scaleUV.m_y);
*/
    Vec2d cs;
    Vec2d::CosSinOpt(&cs, _angle.Radians());

    Vec2d rayon    = _currentSize * 0.5f;
    Vec2d rayonxcs = Vec2d::ScaleX(&cs, &rayon);
    Vec2d sc       = -cs;
          sc       = Vec2d::mergeYX(&sc, &cs);
    Vec2d rayonysc = Vec2d::ScaleY(&sc, &rayon);

    Vec2d posXY;
    posXY.truncateVec3d(_pos);

    Vec2d pos = rayonysc - rayonxcs + posXY;

#ifdef USE_WGP
    Vec2d uv = m_uvData.getUV0() * _scaleUV;
    ITF_VertexBuffer *vb = reinterpret_cast<ITF_VertexBuffer*>(data);
    ITF_ASSERT(vb->bIsDynamic());

    vb->wgp_write(_color, _pos.m_z, pos, uv);

    pos = -rayonysc - rayonxcs + posXY;
    uv  = Vec2d::mergeXY(&m_uvData.getUV0(), &m_uvData.getUV1()) * _scaleUV;
    vb->wgp_write(_color, _pos.m_z, pos, uv);

    pos = -rayonysc + rayonxcs + posXY;
    uv  = m_uvData.getUV1() * _scaleUV;
    vb->wgp_write(_color, _pos.m_z, pos, uv);

    pos = rayonysc + rayonxcs + posXY;
    uv  = Vec2d::mergeXY(&m_uvData.getUV1(), &m_uvData.getUV0()) * _scaleUV;
    vb->wgp_write(_color, _pos.m_z, pos, uv);
#else
    Vec2d::to3d(&data[0].m_pos, &pos, _pos.m_z);
    data[0].m_color = _color;
    data[0].m_uv = m_uvData.getUV0() * _scaleUV;

    pos = -rayonysc - rayonxcs + posXY;
    Vec2d::to3d(&data[1].m_pos, &pos, _pos.m_z);
    data[1].m_color = _color;
    data[1].m_uv = Vec2d::mergeXY(&m_uvData.getUV0(), &m_uvData.getUV1()) * _scaleUV;

    pos = -rayonysc + rayonxcs + posXY;
    Vec2d::to3d(&data[2].m_pos, &pos, _pos.m_z);
    data[2].m_color = _color;
    data[2].m_uv = m_uvData.getUV1() * _scaleUV;

    pos = rayonysc + rayonxcs + posXY;
    Vec2d::to3d(&data[3].m_pos, &pos, _pos.m_z);
    data[3].m_color = _color;
    data[3].m_uv = Vec2d::mergeXY(&m_uvData.getUV1(), &m_uvData.getUV0()) * _scaleUV;
#endif
}

///----------------------------------------------------------------------------//

void Quad_ToVBPivot( VertexPCT* __restrict data, const ITF_Particle* __restrict _pParticle,const UVdata& _uvData, Vec2d& _scaleUV)
{
/*  original code
    Vec2d rayon, cs, sc, p1, p3, p1cs, p1sc, p3cs, p3sc;
    Vec2d::Scale(&rayon, &_pParticle->m_curSize, 0.5f);
    
    Vec2d::SinCosOpt(&sc, _pParticle->m_angle.Radians());
    cs.m_x = sc.m_y; cs.m_y = sc.m_x;
    u32 color = _pParticle->m_curColor.getAsU32();

    Vec2d::Sub(&p1, &_pParticle->m_pivot, &rayon);
    Vec2d::Add(&p3, &_pParticle->m_pivot, &rayon);   

    Vec2d::Mul(&p1cs, &p1, &cs);
    Vec2d::Mul(&p3cs, &p3, &cs);
    Vec2d::Mul(&p1sc, &p1, &sc);
    Vec2d::Mul(&p3sc, &p3, &sc);    

    data[0].m_pos = Vec3d(p1cs.m_x - p1cs.m_y + _pParticle->m_pos.m_x, p1sc.m_x + p1sc.m_y + _pParticle->m_pos.m_y, _pParticle->m_pos.m_z);
    data[0].m_color = color;
    data[0].m_uv = _uvData.getUV0() * _scaleUV;

    data[1].m_pos = Vec3d(p1cs.m_x - p3cs.m_y + _pParticle->m_pos.m_x, p1sc.m_x + p3sc.m_y + _pParticle->m_pos.m_y, _pParticle->m_pos.m_z);
    data[1].m_color = color;
    data[1].m_uv = Vec2d(_uvData.getUV0().m_x * _scaleUV.m_x, _uvData.getUV1().m_y * _scaleUV.m_y);

    data[2].m_pos = Vec3d(p3cs.m_x - p3cs.m_y + _pParticle->m_pos.m_x, p3sc.m_x + p3sc.m_y + _pParticle->m_pos.m_y, _pParticle->m_pos.m_z);
    data[2].m_color = color;
    data[2].m_uv = _uvData.getUV1() * _scaleUV;

    data[3].m_pos = Vec3d(p3cs.m_x - p1cs.m_y + _pParticle->m_pos.m_x, p3sc.m_x + p1sc.m_y + _pParticle->m_pos.m_y, _pParticle->m_pos.m_z);
    data[3].m_color = color;
    data[3].m_uv = Vec2d(_uvData.getUV1().m_x * _scaleUV.m_x, _uvData.getUV0().m_y * _scaleUV.m_y);
*/
    // Rewritten to get faster code on Wii
    Vec2d cs;
    Vec2d::CosSinOpt(&cs, _pParticle->m_angle.Radians());
    Vec2d rayon = _pParticle->m_curSize * 0.5f;

    Vec2d p1 = _pParticle->m_pivot - rayon;
    Vec2d p3 = _pParticle->m_pivot + rayon;
    Vec2d sc = -cs;
          sc = Vec2d::mergeYX(&sc, &cs);

    u32 color = _pParticle->m_curColor.getAsU32();
    Vec2d posXY;
    posXY.truncateVec3d(_pParticle->m_pos);
    f32 posZ = _pParticle->m_pos.m_z;

    Vec2d pos = Vec2d::ScaleX(&cs, &p1) + Vec2d::ScaleY(&sc, &p1) + posXY;
    Vec2d uv  = _uvData.getUV0() * _scaleUV;

#ifdef USE_WGP
    ITF_VertexBuffer *vb = reinterpret_cast<ITF_VertexBuffer*>(data);
    ITF_ASSERT(vb->bIsDynamic());

    vb->wgp_write(color, posZ, pos, uv);

    pos = Vec2d::ScaleX(&cs, &p1) + Vec2d::ScaleY(&sc, &p3) + posXY;
    uv  = Vec2d::mergeXY(&_uvData.getUV0(), &_uvData.getUV1()) * _scaleUV;
    vb->wgp_write(color, posZ, pos, uv);

    pos = Vec2d::ScaleX(&cs, &p3) + Vec2d::ScaleY(&sc, &p3) + posXY;
    uv  = _uvData.getUV1() * _scaleUV;
    vb->wgp_write(color, posZ, pos, uv);

    pos = Vec2d::ScaleX(&cs, &p3) + Vec2d::ScaleY(&sc, &p1) + posXY;
    uv  = Vec2d::mergeXY(&_uvData.getUV1(), &_uvData.getUV0()) * _scaleUV;
    vb->wgp_write(color, posZ, pos, uv);
#else
    Vec2d::to3d(&data[0].m_pos, &pos, posZ);
    data[0].m_color = color;
    data[0].m_uv = _uvData.getUV0() * _scaleUV;

    pos = Vec2d::ScaleX(&cs, &p1) + Vec2d::ScaleY(&sc, &p3) + posXY;
    uv  = Vec2d::mergeXY(&_uvData.getUV0(), &_uvData.getUV1()) * _scaleUV;
    Vec2d::to3d(&data[1].m_pos, &pos, posZ);
    data[1].m_color = color;
    data[1].m_uv = uv;

    pos = Vec2d::ScaleX(&cs, &p3) + Vec2d::ScaleY(&sc, &p3) + posXY;
    uv  = _uvData.getUV1() * _scaleUV;
    Vec2d::to3d(&data[2].m_pos, &pos, posZ);
    data[2].m_color = color;
    data[2].m_uv = uv;

    pos = Vec2d::ScaleX(&cs, &p3) + Vec2d::ScaleY(&sc, &p1) + posXY;
    uv  = Vec2d::mergeXY(&_uvData.getUV1(), &_uvData.getUV0()) * _scaleUV;
    Vec2d::to3d(&data[3].m_pos, &pos, posZ);
    data[3].m_color = color;
    data[3].m_uv = uv;
#endif
}

///----------------------------------------------------------------------------//

void Quad_ToVBOrient( VertexPCT* __restrict _data,const ITF_Particle* __restrict _pParticle,const Vec2d& _pivot, const UVdata& _uvData, Vec2d& _scaleUV)
{
    Vec3d   vertex[4];

    /// orient dir to cam.
    const GMatrix44* mview = GFX_ADAPTER->getViewMatrix();
    Vec3d SZ = Vec3d( mview->_m13, mview->_m23, mview->_m33);
    Vec3d SY = _pParticle->m_vel;
    Vec3d SX;
    SY.normalize();
    SY.crossProduct(SZ, SX);
    SX.normalize();

    //SX = Vec3d(1.f,0.f,0.f);//Vec3d( mview->_11, mview->_21, mview->_31);
    //SY = Vec3d(0.f,1.f,0.f);//Vec3d( mview->_12, mview->_22, mview->_32);
    //SZ = Vec3d(0.f,1.f,0.f);//Vec3d( mview->_13, mview->_23, mview->_33);

    SX *= (_pParticle->m_curSize.m_x * 0.5f);
    SY *= (_pParticle->m_curSize.m_y * 0.5f);

    Vec3d normal0 = SX + SY;
    Vec3d normal1 = SX - SY;
    u32 color = _pParticle->m_curColor.getAsU32();

	if (!_pivot.IsEqual(Vec2d::Zero,MTH_EPSILON))
    {
        Vec2d v_Size = _pParticle->m_curSize * 0.5f;
        Vec2d v_off  = _pivot * _pParticle->m_curSize;

		SX.normalize();
		SY.normalize();

		Vec3d nSX = -SX;
		Vec3d nSY = -SY;

        SX *= (v_Size.m_x - v_off.m_x);
        SY *= (v_Size.m_y - v_off.m_y);
        nSX *= (v_Size.m_x + v_off.m_x);
        nSY *= (v_Size.m_y + v_off.m_y);
     
        //SZ *= _quad->m_pivot.m_z;
        //v += SZ;
        SZ = Vec3d(Vec3d::Zero);
		
        vertex[0] = _pParticle->m_pos + nSX + nSY;
		vertex[1] = _pParticle->m_pos + nSX + SY;
		vertex[2] = _pParticle->m_pos + SX + SY;
		vertex[3] = _pParticle->m_pos + SX + nSY;
	}
    else
    {
	    vertex[0] = _pParticle->m_pos - normal0;
	    vertex[1] = _pParticle->m_pos - normal1;
	    vertex[2] = _pParticle->m_pos + normal0;
	    vertex[3] = _pParticle->m_pos + normal1;
    }

    Vec2d uv0 = _uvData.getUV0() * _scaleUV;
    Vec2d uv1 = Vec2d::mergeXY(&_uvData.getUV1(), &_uvData.getUV0()) * _scaleUV;
    Vec2d uv2 = Vec2d::mergeXY(&_uvData.getUV0(), &_uvData.getUV1()) * _scaleUV;
    Vec2d uv3 = _uvData.getUV1() * _scaleUV;

#ifdef USE_WGP
    ITF_VertexBuffer *vb = reinterpret_cast<ITF_VertexBuffer*>(_data);
    ITF_ASSERT(vb->bIsDynamic());
    vb->wgp_write(color, vertex[1].m_z, vertex[1].truncateTo2D(), uv0);
    vb->wgp_write(color, vertex[0].m_z, vertex[0].truncateTo2D(), uv1);
    vb->wgp_write(color, vertex[2].m_z, vertex[2].truncateTo2D(), uv2);
    vb->wgp_write(color, vertex[3].m_z, vertex[3].truncateTo2D(), uv3);
#else
    _data[0].m_pos = vertex[1];
    _data[0].m_color = color;
    _data[0].m_uv = uv0;

    _data[3].m_pos = vertex[0];
    _data[3].m_color = color;
    _data[3].m_uv = uv1;

    _data[1].m_pos = vertex[2];
    _data[1].m_color = color;
    _data[1].m_uv = uv2;

    _data[2].m_pos = vertex[3];
    _data[2].m_color = color;
    _data[2].m_uv = uv3;
#endif
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::fillNormal(const ITF_Particle * __restrict pParticle, const Texture* __restrict pTexture, VertexPCT* pdata, Vec2d _scaleUV)
{
    UVdata uvData;

    if (pTexture->getUVAtlas() && pParticle->m_indexUV >= 0 )
        uvData = pTexture->getUVAtlas()->getUVDatabyIndex(pParticle->m_indexUV);

    if (pParticle->m_flag & PAR_F_UVINVERT_X)
    {
        Vec2d uv0   = uvData.getUV0();
        Vec2d uv1   = uvData.getUV1();
        f32   uv0X  = uv0.m_x;
        uv0.m_x     = uv1.m_x;
        uv1.m_x     = uv0X;
        uvData.setUVdata(uv0, uv1);
    }

    if (pParticle->m_flag & PAR_F_UVINVERT_Y)
    {
        Vec2d uv0   = uvData.getUV0();
        Vec2d uv1   = uvData.getUV1();
        f32   uv0Y  = uv0.m_y;
        uv0.m_y     = uv1.m_y;
        uv1.m_y     = uv0Y;
        uvData.setUVdata(uv0, uv1);
    }
    
#ifdef USEVB		
    if (pParticle->m_pivot.IsEqual(Vec2d::Zero,MTH_EPSILON))
    {
        if (m_params.m_orientDir == 2)
            Quad_ToVBOrient(pdata,pParticle,pParticle->m_pivot,uvData, _scaleUV);
        else
        {
            u32 color = pParticle->m_curColor.getAsU32();
            Quad_ToVB(pdata, pParticle->m_angle,color,pParticle->m_pos,pParticle->m_curSize,uvData, _scaleUV);
        }
    }
    else
    {
        if (m_params.m_orientDir == 2)
        {
            Quad_ToVBOrient(pdata,pParticle,m_params.m_pivot,uvData, _scaleUV);
        }
        else
        {
            Quad_ToVBPivot(pdata, pParticle,uvData, _scaleUV);
        }
    }
#else
    ITF_ASSERT(0);
    /*GFX_ADAPTER->SetTexture(0, pTexture);
    GFX_ADAPTER->draw3DQuad(&quad);*/
#endif
}

///----------------------------------------------------------------------------//

u32 ITF_ParticleGenerator::FillZsort(Texture* pTexture, VertexPCT* pdata)
{
    //TOBE IMPLEMENTED
    ITF_ASSERT(0);
    u32 count = 0;
    /*
    GFX_3DQUAD quad;

    ITF_ParticleOrder parorderMask;
    ITF_ParticleOrder parorder;

    ITF_Particle *pParticle = m_pActiveList;
	

    /// reorder particles and mask.
    m_particlesZOrdering->reset();

    while( pParticle )
    {
        parorderMask.m_part = pParticle;
        parorderMask.m_zsort = pParticle->m_zsort;
        parorderMask.m_mask = 1;

        parorder.m_part = pParticle;
        parorder.m_zsort = pParticle->m_pos.m_z;
        parorder.m_mask = 0;
        
        m_particlesZOrdering->AddParticle(&parorderMask);
        m_particlesZOrdering->AddParticle(&parorder);

        pParticle = pParticle->m_pNext;
    }


    m_particlesZOrdering->sort();

    ITF_ParticleOrder* pCurNode = NULL;
    ITF_Particle* pPar;

    for (u32 i = 0; i < m_particlesZOrdering->getNumberEntry(); i++)
    {   
        pCurNode = m_particlesZOrdering->getOrderNodeAt(i);

        pPar = pCurNode->m_part;
        
        quad.m_angle = pPar->m_angle;

        
        if (pTexture->getUVAtlas() && (pPar->m_useAnim || m_useUVRandom) )
            quad.m_uvData = pTexture->getUVAtlas()->getUVDatabyIndex(pPar->m_indexUV);
 
        if (pCurNode->m_mask)
        {
            quad.m_color = COLOR_BLACK;
            quad.m_size.m_x = pPar->m_curSize.m_x + m_params.m_outlinesize;
            quad.m_size.m_y = pPar->m_curSize.m_y + m_params.m_outlinesize;
        }
        else
        {
            quad.m_color = pPar->m_curColor.getAsU32();
            quad.m_size = pPar->m_curSize;
        }

        quad.m_pos = pPar->m_pos;
        quad.m_pos.m_z = pCurNode->m_zsort;
 
        if (pPar->m_pivot.IsEqual(Vec2d::Zero,MTH_EPSILON))
        {
            Quad_ToVB(pdata, &quad, scaleUV);
            pdata += 4;
        }
        
        count++;
    }*/

    return count;

}

bbool ITF_ParticleGenerator::render()
{
    if (m_params.isImpostor())
        impostorRender();
    else
        drawParticles();
    
    
    m_lights.clear();
    return 1;
}

///----------------------------------------------------------------------------//
bbool ITF_ParticleGenerator::checkGeneratorValidity()
{
    if (m_checkValidity) return btrue;

    /// check atlas and phase.
    Texture* pTexture = static_cast<Texture*>(m_params.getTexture().getResource());
    if ((!pTexture) || (!pTexture->isPhysicallyLoaded()))
        return bfalse;

//    ITF_ASSERT_MSG(!pTexture->getPath().getString().strstr(Resource::getTracedResourceName()), "ITF_ParticleGenerator client of traced resource");


    const UVAtlas* atlas = pTexture->getUVAtlas();
    if (atlas)
    {
        u32 atlasmaxindex = atlas->getNumberUV();
        
        if (getStartAnimIndex() >= 0 && getEndAnimIndex() >= 0)
        {
            if ((u32)getStartAnimIndex() >= atlasmaxindex || (u32)getEndAnimIndex() >= atlasmaxindex)
            {
                m_validityMessage.setTextFormat("global anim index > max atlas index from texture: %s", m_params.getTexture().getDebugString().cStr());
                m_checkValidity = btrue;
                return bfalse;
            }

            u32 nbphase = m_params.getNumberPhase();
            for (u32 i =0 ; i < nbphase; i++)
            {
                if (m_params.m_phaseList[i].m_animStart >= 0 && m_params.m_phaseList[i].m_animEnd >= 0)
                {
                    if ((u32)m_params.m_phaseList[i].m_animStart >= atlasmaxindex || (u32)m_params.m_phaseList[i].m_animEnd >= atlasmaxindex)
                    {
                        m_validityMessage.setTextFormat("phase anim index > max atlas index from texture: %s", m_params.getTexture().getDebugString().cStr());
                        m_checkValidity = btrue;
                        return bfalse;
                    }
                }
           
            }
        }
    }

    m_checkValidity = btrue;
    m_validityMessage.clear();
    return btrue;
}

void ITF_ParticleGenerator::addlight(const LightInfo &_light)
{
    if (!m_params.getDisableLight())
        m_lights.push_back(_light);
}

void ITF_ParticleGenerator::lightProcess()
{
    if (!m_params.getDisableLight())
    {
        u32 numberLight = m_lights.size();

        Color m_LightColorComputed = Color::zero();
        f32 nl = 0.f;
        if (numberLight)
        {
            for (u32 i = 0; i < numberLight; i++)
            {
                LightInfo* light = &m_lights[i];
                
                f32 sqDist;

                if (light->m_useBV)
                {
                    f32 distx = m_params.getBoundingBox().getDistMinPosAABB(light->m_position.truncateTo2D());
                    f32 disty = getParameters()->getPosition().m_z - light->m_position.m_z;
                    sqDist = f32_Abs(Vec2d(distx, disty).norm());
                }
                else
                {
                    sqDist = f32_Abs(Vec3d(light->m_position - m_params.m_pos).norm());
                }

                if (sqDist < light->m_radFar)
                {
                    switch (light->m_type)
                    {
                    case LightInfo::AMBIENT:
                        m_LightColorComputed += light->m_color;
                        break;
                     case LightInfo::LINEAR:
                            if (sqDist < light->m_radNear)
                            {
                                m_LightColorComputed += light->m_color;
                                nl = 1.f;
                            }
                            else
                            {
                                f32 lerp =  1.f - (sqDist - light->m_radNear) / (light->m_radFar - light->m_radNear);
                                nl += lerp;
                                m_LightColorComputed += light->m_color * lerp;
                            }
                        break;
                   }
                }
            }

            if (nl > 1.f)
                nl = 1.f;

            //lerp final
            Color one(1.f, 1.f, 1.f, 1.f);
            Color co = Color::colorBlend(&m_LightColorComputed, &one, nl);
            m_LightColorComputed = co;
        }
        else
             m_LightColorComputed = Color::white();


        m_LightColorComputed.setAlpha(1.f);
        m_LightColorComputed.ClampColor();
        applyAmbiantColor(m_LightColorComputed);
    }
}

#ifndef ITF_SUPPORT_MULTITHREADED
void ITF_ParticleGenerator::fillActiveList(ITF_VertexBuffer *_VB)
{
    if (!m_particlesCountVtxToDisplay)
        return;

    Texture* pTexture = static_cast<Texture*>(getParameters()->getTexture().getResource());
    ITF_ASSERT(pTexture && pTexture->isPhysicallyLoaded());

    const ParticleGeneratorParameters* pParams = getParametersConst(); 
    const Vec2d scaleUV(pParams->isUseTextureMirrorU() ? 2.f : 1.f, pParams->isUseTextureMirrorV() ? 2.f : 1.f);

    for(ITF_Particle* pParticle = m_pActiveList; pParticle ; pParticle = pParticle->m_pNext)
    {
        if(pParticle->m_bAlive)
        {
            fillNormal(pParticle,pTexture,reinterpret_cast<VertexPCT*>(_VB), scaleUV);
        }
    }
}

//------------------------------------------------------------------------------------------

void ITF_ParticleGenerator::fillParticuleArray(ITF_VertexBuffer *_VB)
{
    if (!m_particlesCountVtxToDisplay)
        return;

    Texture* pTexture = static_cast<Texture*>(getParameters()->getTexture().getResource());
    ITF_ASSERT(pTexture && pTexture->isPhysicallyLoaded());

    const ParticleGeneratorParameters* pParams = getParametersConst(); 
    const Vec2d scaleUV(pParams->isUseTextureMirrorU() ? 2.f : 1.f, pParams->isUseTextureMirrorV() ? 2.f : 1.f);

    for (u32 index = 0;index<m_maxParticlesNeed;++index)
    {
        if (m_pParticleArrayIndex[index].m_bAlive)
        {
            fillNormal(&m_pParticleArrayIndex[index],pTexture,reinterpret_cast<VertexPCT*>(_VB),scaleUV);
        }
    }
}

//------------------------------------------------------------------------------------------

void ITF_ParticleGenerator::fillParticles(ITF_VertexBuffer *_VB)
{
    switch(getParameters()->getGeneratorMode())
    {
    case PARGEN_MODE_MANUAL:
        fillActiveList(_VB);
        break;
    case PARGEN_MODE_FOLLOW: 
    case PARGEN_MODE_COMPLEX:
    default:
        fillParticuleArray(_VB);
        break;
    }
}
#endif // ITF_SUPPORT_MULTITHREADED


bbool ITF_ParticleGenerator::drawParticles(bbool _direct)
{
//    LOG("ITF_ParticleGenerator::drawParticles %d", this);

    STATS_PARTICULE_DRAWINC
    if (!m_isRender) 
        return bfalse;

    Texture* pTexture = static_cast<Texture*>(m_params.getTexture().getResource());
    if ((!pTexture) || (!pTexture->isPhysicallyLoaded()))
        return bfalse;

   if (m_activeParticlesCount == 0)
        return bfalse;

    lightProcess();

#ifdef USEVB
    VertexPCT* pdata;
    #ifdef ITF_SUPPORT_MULTITHREADED
	    pdata = (VertexPCT*)m_cacheMemory;
    #else // ITF_SUPPORT_MULTITHREADED
        ITF_VertexBuffer * pVB = NULL;
        #ifdef VBMANAGER_USE_DYNAMICRING_VB
            // Set real vtx number to avoid extra lock
            if(m_mesh.m_vtxBuffer->bIsDynamic())
                m_mesh.m_vtxBuffer->m_nVertex = min(m_particlesCountVtxToDisplay, m_maxParticlesNeed) * 4;
            if(!m_mesh.m_vtxBuffer->m_nVertex)
                return bfalse;
        #endif // VBMANAGER_USE_DYNAMICRING_VB
        pdata = LockVB(&pVB);
        fillParticles(pVB);
    #endif // ITF_SUPPORT_MULTITHREADED
#else // USEVB
    /// setup Matrix to identity.
    GFX_ADAPTER->setMatrixTransform(GFX_MWORLD, &GMatrixIdentity);
 	GFX_ADAPTER->setAlphaBlend(m_params.m_blendMode);
#endif // USEVB

    if (m_params.isCartoon())
        m_particlesCountVtxToDisplay = FillZsort(pTexture, pdata);
    /*
    else
        count = FillNormal(pTexture, pdata);*/

    if (!m_particlesCountVtxToDisplay)
    {
        #ifndef ITF_SUPPORT_MULTITHREADED
        UnlockVB(pVB);
        #endif // ITF_SUPPORT_MULTITHREADED
        return bfalse;
    }
    
#ifdef USEVB
#ifdef ITF_SUPPORT_MULTITHREADED
    ITF_VertexBuffer * pVB = NULL;
    VertexPCT* data = LockVB(&pVB);
    if(data != NULL)
#endif // ITF_SUPPORT_MULTITHREADED
    {
#ifdef ITF_SUPPORT_MULTITHREADED
        #ifdef USE_WGP
            ITF_ASSERT(pVB->bIsDynamic());
            const VertexPCT* pVtx = (VertexPCT*)m_cacheMemory;
            for(int i = 0; i < m_particlesCountVtxToDisplay*4; ++i, ++pVtx)
            {
                pVB->wgp_write(pVtx->m_pos);
                pVB->wgp_write(pVtx->m_color);
                pVB->wgp_write(pVtx->m_uv);
            }
        #else // USE_WGP
            ITF_MemcpyWriteCombined(data,m_cacheMemory,sizeof(VertexPCT)*m_particlesCountVtxToDisplay*4);
        #endif // USE_WGP
#endif // ITF_SUPPORT_MULTITHREADED
        UnlockVB(pVB);

        ITF_MeshElement* _elem = &m_mesh.m_ElementList[0];
        _elem->m_count = m_particlesCountVtxToDisplay * 6;
        _elem->m_material.m_textureDiffuse = m_params.getTexture();
        _elem->m_material.m_blendMode = m_params.getBlendMode();

        _elem->m_material.m_TexAdressingModeU = m_params.isUseTextureMirrorU() ? GFXTADDRESS_MIRROR : GFXTADDRESS_CLAMP;
        _elem->m_material.m_TexAdressingModeV = m_params.isUseTextureMirrorV() ? GFXTADDRESS_MIRROR : GFXTADDRESS_CLAMP;

        if (!m_params.getDisableLight())
        {
            m_mesh.m_globalColor.m_r = m_ambColor.m_r;
            m_mesh.m_globalColor.m_g = m_ambColor.m_g;
            m_mesh.m_globalColor.m_b = m_ambColor.m_b;
            m_mesh.m_globalColor.m_a = 1.0f;
        } 
        else
        {
            m_mesh.m_globalColor     = Color::white();
        }
        m_mesh.m_globalColor = m_mesh.m_globalColor * m_factorColor;
        m_mesh.m_globalColor.m_a *= getAlpha();
        
        if (_direct)
        {
            GFX_ADAPTER->drawMesh(m_mesh, bfalse); 

            #ifdef ITF_WII
            // Pass for separate alpha
            if (m_params.isImpostor())
            {
                GFX_ADAPTER_WII->startImpostorAlphaRendering(m_renderTargetImpostor);
                GFX_ADAPTER->drawMesh(m_mesh, bfalse); 
            }
            #endif // ITF_WII
        }
        else
        {
            if (m_params.m_useMatrix || m_params.useActorTranslation())
                m_mesh.setMatrix(m_transformMatrix);
            else
                m_mesh.setMatrix((GMatrix44&)GMatrixIdentity);

            f32 zr = 0.f;
            if (m_params.m_useZAsDepth)
                zr = m_params.m_pos.m_z;
            else
                zr = m_params.m_depth;

            zr+= m_params.m_renderPrio;
            
            if (getTemplate()->getIs2D())
            {    
                f32 screenWidth = (f32)GFX_ADAPTER->getScreenWidth();
                Vec2d scale(screenWidth,-screenWidth);
                GFX_ADAPTER->addPrimitive2d( (Vec2d&)m_params.m_pos, scale, 0, GFX_ITF_MESH, &m_mesh);
            }
            else
            {
                GFX_ADAPTER->addPrimitiveInZList((void*)&m_mesh, GFX_ITF_MESH, NULL, zr, m_renderInTarget,ObjectRef::InvalidRef);
            }
        }
    }
#ifdef ITF_SUPPORT_MULTITHREADED
    else
        return bfalse;
#endif // ITF_SUPPORT_MULTITHREADED
#endif // USEVB

    return btrue;
}

///----------------------------------------------------------------------------//

ITF_INLINE Texture* ITF_ParticleGenerator::getTextureFromImpostor()
{
    if (m_renderTargetImpostor)
        return m_renderTargetImpostor->getTexture();
    else return NULL;
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::impostorRender()
{
    if (!m_renderTargetImpostor)
    {
        m_renderTargetImpostor = GFX_ADAPTER->getNewRenderTarget();
        m_renderTargetImpostor->create((f32)m_params.m_impostorTextureSizeX, (f32)m_params.m_impostorTextureSizeY);
        m_renderTargetImpostor->setBackGroundColor(0);
    }

    f32 minx = -1.f;
    f32 maxx = 1.f;
    f32 miny = -1.f;
    f32 maxy = 1.f;

    Vec2d ViewPos;

    /// From AABB
    f32 w = m_params.getBoundingBox().getWidth();
    f32 h = m_params.getBoundingBox().getHeight();
    Vec2d max = m_params.getBoundingBox().getMax();
    ViewPos = m_params.getBoundingBox().getCenter();
    
    minx = ViewPos.m_x;
    miny = ViewPos.m_y;
    minx -= w * 0.5f;
    miny -= h * 0.5f;
    maxx = max.m_x;
    maxy = max.m_y;

    if (!m_computeAABB)
    {
        minx += m_params.m_pos.m_x;
        maxx += m_params.m_pos.m_x;
        miny += m_params.m_pos.m_y;
        maxy += m_params.m_pos.m_y;
    }

    /*
    Vec2d Size = Vec2d(1.f, 1.f);
    Vec2d Scale = Vec2d(4.f,0.7f);
    Size *= Scale;
    ViewPos = Vec2d(-Size.m_x, -Size.m_y);
    Vec2d Pos = Vec2d(4.f,0.f);
    ViewPos += Pos;
    minx = m_params.m_pos.m_x + ViewPos.m_x;
    miny = m_params.m_pos.m_y + ViewPos.m_y;
    maxx = m_params.m_pos.m_x + Size.m_x;
    maxy = m_params.m_pos.m_y + Size.m_y;
    */
   
    GFX_ADAPTER->startImpostorRendering(m_renderTargetImpostor, minx, miny, maxx, maxy);
    drawParticles(btrue);
    GFX_ADAPTER->stopImpostorRendering(m_renderTargetImpostor);

    if (m_params.isShowImpostorRender())
    {
		m_3dQuad.m_color = COLOR_WHITE;
		m_3dQuad.m_size.m_x = (maxx - minx);
        m_3dQuad.m_size.m_y = (maxy - miny);
        m_3dQuad.m_pos.m_x = m_params.m_pos.m_x + m_3dQuad.m_size.m_x * 0.5f;
        m_3dQuad.m_pos.m_y = m_params.m_pos.m_y;
    
        f32 zr = m_params.m_pos.m_z + m_params.m_renderPrio;
        GFX_ADAPTER->addPrimitiveInZList((void*)&m_3dQuad, GFX_ITF_3DQUAD, getTextureFromImpostor(), zr,0,ObjectRef::InvalidRef);
    }
}

///----------------------------------------------------------------------------//

Vec3d getRandomVector()
{
    Vec3d v;
    v.m_x = randomLCG.getRandF( -1.0f, 1.0f );
    v.m_y = randomLCG.getRandF( -1.0f, 1.0f );
    v.m_z = randomLCG.getRandF( -1.0f, 1.0f );
    v.normalize();
    return v;
}
///----------------------------------------------------------------------------//

void   ITF_ParticleGenerator::deallocateParticle(ITF_Particle* pParticle)
{
    //compute the index
    u32 index = u32(pParticle - m_pParticleArrayIndex);
    pParticle->dead();

    m_pFreeParticleIndex.push_back(index);
}
///----------------------------------------------------------------------------//

ITF_Particle*   ITF_ParticleGenerator::allocateParticle()
{
    const  u32 size = m_pFreeParticleIndex.size();
    ITF_ASSERT(size != 0);

    const u32 index = size-1;

    ITF_Particle* pParticle = &m_pParticleArrayIndex[m_pFreeParticleIndex[index]];
    m_pFreeParticleIndex.eraseNoOrder(index);

    pParticle->live();
    return pParticle;
}
///----------------------------------------------------------------------------//

Vec3d getRandomVectorInter(f32 _ang1 , f32 _ang2, f32 _zinter1, f32 _zinter2)
{
	Vec3d v;
    v.m_z = randomLCG.getRandF( _zinter1, _zinter2 );
    
    f32 r = (float)f32_Sqrt(1 - v.m_z * v.m_z);
    
    f32 t = randomLCG.getRandF( _ang1, _ang2 );

    f32_CosSin(t, &v.m_x, &v.m_y);
    v.m_x *= r;
    v.m_y *= r;

	return v;
}
///----------------------------------------------------------------------------//

ITF_INLINE Vec3d getRandomVector2d(f32 _min, f32 _max)
{
    f32 t = randomLCG.getRandF( _min, _max );
    Vec3d v;
    //f32_CosSin(t, &v.m_x, &v.m_y);
    f32_CosSin(t, &v.m_y, &v.m_x);
    v.m_z = 0.f;
    return v;
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::update(f32 _dt, bbool _bDisableCreation)
{
//    LOG("ITF_ParticleGenerator::update %d", this);

    m_isRender = btrue;
 
    if (m_aabbNeedCompute && getTrueAABB())
    {
        ParticleGeneratorParameters* parameter = getParameters();
        parameter->setWorkingBoundingBox(parameter->getPosition());
        parameter->synchronizeBoundingBox();
        m_aabbNeedCompute = bfalse;
    }
    
    if (m_maxParticlesNeed>0)
    {
        // We only disable creation for infinite particles
        bbool forceDisableCreation = _bDisableCreation && getParameters()->m_emitParticlesCount == U32_INVALID;
        
        JobUpdateParticleContext context(this,_dt, forceDisableCreation);
        ITF_ASSERT(m_jobParticle);
        m_jobParticle->setContext(context);
        Scheduler::get()->pushJob(m_jobParticle);
    }

    if (getParameters()->isUseMatrix() || getParameters()->useActorTranslation())
        computeTransformMatrix();
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::addOneNewComplexParticle()
{
    ITF_ASSERT(m_params.m_genMode!=PARGEN_MODE_MANUAL);

    if( m_activeParticlesCount < m_maxParticlesNeed )
    {
        STATS_PARTICULE_COMPLEXNEWINC

        ITF_Particle  *pParticle = allocateParticle(); //pParticle = newAlloc(mId_Particle,ITF_Particle);

        initNewParticleComplex(pParticle);

        ++m_activeParticlesCount;

        ITF_ASSERT(m_activeParticlesCount<=m_maxParticlesNeed);
        ++m_totalParticlesGenerated;
    }
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::spawnNParticle( u32 _numParticles, f32 maxTimeRange )
{
    SF_DEL(m_particleArray);
    m_particleArray = newAlloc(mId_Particle, ITF_Particle[_numParticles]);

    m_pActiveList = NULL;
    for ( u32 i = 0; i < _numParticles; i++ )
    {
        m_particleArray[i].m_pNext = m_pActiveList;
        m_pActiveList = &m_particleArray[i];
        initNewParticleComplex(&m_particleArray[i]);
        // add random time
        m_particleArray[i].m_initTime += randomLCG.getRandF( 0.f, maxTimeRange );
    }

    // force data to _numParticles
    m_activeParticlesCount      = _numParticles;
    m_totalParticlesGenerated   = _numParticles;
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::startManualUpdate( const Vec3d& _pos )
{
    m_params.setBoundingBox(AABB(_pos,_pos));
    getParameters()->setPosition(_pos);
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::updateManualParticle( u32 _index, const Vec3d& _pos, const Vec3d& _speed, Angle _rot )
{
    ITF_ASSERT(m_params.m_genMode == PARGEN_MODE_MANUAL && m_particleArray);
    ITF_ASSERT(_index < m_totalParticlesGenerated);

    if ( !m_particleArray )
    {
        return;
    }

    ITF_Particle* particle = &m_particleArray[_index];
    particle->m_pos = _pos;
    particle->m_vel = _speed;
    particle->m_angle = _rot;
    
    m_params.getBoundingBox().grow(_pos);
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::changeManualParticleState( u32 _index, bbool _isAlive )
{
    ITF_ASSERT(m_params.m_genMode == PARGEN_MODE_MANUAL && m_particleArray);
    ITF_ASSERT(_index < m_totalParticlesGenerated);

    if ( !m_particleArray )
    {
        return;
    }

    ITF_Particle* particle = &m_particleArray[_index];
    particle->m_bAlive = _isAlive;
}


///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::changeManualParticleFlip( u32 _index, bbool _isFlip )
{
    ITF_ASSERT(m_params.m_genMode == PARGEN_MODE_MANUAL && m_particleArray);
    ITF_ASSERT(_index < m_totalParticlesGenerated);

    if ( !m_particleArray )
    {
        return;
    }

    ITF_Particle* particle = &m_particleArray[_index];
    if (_isFlip)
        particle->m_flag |= PAR_F_UVINVERT_X;
    else
        particle->m_flag &= ~PAR_F_UVINVERT_X;
}


///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::changeManualParticleSize(const u32 _index, const f32 _size)
{
    ITF_ASSERT(m_params.m_genMode == PARGEN_MODE_MANUAL && m_particleArray);
    ITF_ASSERT(_index < m_totalParticlesGenerated);

    if ( !m_particleArray )
    {
        return;
    }

    ITF_Particle* particle = &m_particleArray[_index];
	particle->m_dstSize.m_x = _size;
	particle->m_dstSize.m_y = _size;
	particle->m_initSize.m_x = _size;
	particle->m_initSize.m_y = _size;
	particle->m_curSize.m_x = _size;
	particle->m_curSize.m_y = _size;
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::changeManualParticleColor(const u32 _index, const Color &_color)
{
    ITF_ASSERT(m_params.m_genMode == PARGEN_MODE_MANUAL && m_particleArray);
    ITF_ASSERT(_index < m_totalParticlesGenerated);

    if ( !m_particleArray )
    {
        return;
    }

    ITF_Particle* particle = &m_particleArray[_index];
    particle->m_initColor = _color.getAsU32();
	particle->m_dstColor = _color.getAsU32();
	particle->m_curColor = _color.getAsU32();
}


///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::changeManualParticleAlpha(const u32 _index, const u8 _alpha)
{
    ITF_ASSERT(m_params.m_genMode == PARGEN_MODE_MANUAL && m_particleArray);
    ITF_ASSERT(_index < m_totalParticlesGenerated);

    if ( !m_particleArray )
    {
        return;
    }

    ITF_Particle* particle = &m_particleArray[_index];
	particle->m_initColor.m_rgba.m_a = _alpha;
	particle->m_dstColor.m_rgba.m_a = _alpha;
	particle->m_curColor.m_rgba.m_a = _alpha;
}


///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::changeManualAllParticlesColor(const Color &_color)
{
    for(u32 index = 0; index < m_totalParticlesGenerated; ++index)
    {
        changeManualParticleColor(index, _color);
    }
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::changeManualAllParticlesSize(const f32 _size)
{
    for(u32 index = 0; index < m_totalParticlesGenerated; ++index)
    {
        changeManualParticleSize(index, _size);
    }
}


///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::updateParticleAnim(ITF_Particle* _particle, f32 _time)
{
    if (_particle->m_flag & PAR_F_ANIM_STRETCHTIME)
    {
        f32 f = _time/_particle->m_phaselifeTime;
        if (_particle->m_endAnimIndex < _particle->m_startAnimIndex)
        {
            _particle->m_indexUV = (i32)f32_Lerp((f32)_particle->m_endAnimIndex, (f32)(_particle->m_startAnimIndex + 1), f);
            _particle->m_indexUV = Clamp(_particle->m_indexUV, _particle->m_endAnimIndex, _particle->m_startAnimIndex);
        } else
        {
            _particle->m_indexUV = (i32)f32_Lerp((f32)_particle->m_startAnimIndex, (f32)(_particle->m_endAnimIndex + 1), f);
            _particle->m_indexUV = Clamp(_particle->m_indexUV, _particle->m_startAnimIndex, _particle->m_endAnimIndex);
        }
    } else
    {
        if (_particle->m_endAnimIndex < _particle->m_startAnimIndex)
        {
            u32 mod = (_particle->m_startAnimIndex - _particle->m_endAnimIndex) + 1;
            _particle->m_indexUV = _particle->m_startAnimIndex - (((i32)(_time * m_template->getAnimUVFreq() )) % mod);
        } else
        {
            u32 mod = (_particle->m_endAnimIndex - _particle->m_startAnimIndex) + 1;
            _particle->m_indexUV = _particle->m_startAnimIndex + (((i32)(_time * m_template->getAnimUVFreq() )) % mod);
        }
   }
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::updateAABB(ITF_Particle* _particle)
{
    //check if we use actor relative space
    if( m_params.useActorTranslation() )
    {
        //update AABB with gen position + relative position
        m_params.getBoundingBoxWorking().grow( m_params.m_pos + _particle->m_pos );
    }
    else
    {
        //else grow AABB with particle position
        m_params.getBoundingBoxWorking().grow(_particle->m_pos);
    }
}

void ITF_ParticleGenerator::computeTransformMatrix()
{
    Matrix44 out = MatrixIdentity;

    if( m_params.isUseMatrix() )
    {
        Matrix44 rotScale;

        M44_setMatrixRotationZ(&rotScale, m_LocalRotationZ);
        Vec3d scale(m_LocalScale.m_x, m_LocalScale.m_y, 1.f);
        rotScale.mulScale(scale);
        if( !m_params.useActorTranslation() )
        {
            Matrix44 invtrans;
            Matrix44 mtransform;
            Matrix44 translation;
            M44_setMatrixTranslation(&invtrans, -m_params.m_pos.m_x, -m_params.m_pos.m_y, -m_params.m_pos.m_z);
            M44_mul4x4Out(&mtransform, &invtrans, &rotScale);
            M44_setMatrixTranslation(&translation, m_params.m_pos.m_x, m_params.m_pos.m_y, m_params.m_pos.m_z);
            M44_mul4x4Out(&out, &mtransform, &translation);
        }
        else
        {
             out = rotScale;
        }
    }

    if( m_params.useActorTranslation() )
    {
        Matrix44 currentTransformation = out;
        Matrix44 translation;
        //get offset and scale it
        const Vec2d offset = Vec2d(
            m_params.getActorTranslationOffset().m_x * m_LocalScale.m_x,
            m_params.getActorTranslationOffset().m_y * m_LocalScale.m_y );
        //compute translation matrix
        M44_setMatrixTranslation(&translation, m_params.m_pos.m_x + offset.m_x, m_params.m_pos.m_y + offset.m_y, m_params.m_pos.m_z);
        //apply translation matrix to transform matrix
        M44_mul4x4Out(&out, &currentTransformation, &translation);
    }

    m_transformMatrix = out;
}

void ITF_ParticleGenerator::transformBoundingBoxWithMatrix()
{
    AABB& aabb = m_params.getBoundingBoxWorking();
    Vec2d min = aabb.getMin();
    Vec2d max = aabb.getMax();
    if (m_params.useActorTranslation())
    {
        min -= m_params.m_pos.truncateTo2D();
        max -= m_params.m_pos.truncateTo2D();
    }

    Float4 v;
    Float4 vt1;
    Float4 vt2;
    Float4 vt3;
    Float4 vt4;

    f32 depth = m_params.m_pos.m_z;
    Matrix44 mTrans = (Matrix44)m_transformMatrix;

    setFloat4(v, min.m_x, min.m_y, depth, 1);
    M44_matrixCoordTransform(&vt1, &mTrans, &v);
    setFloat4(v, max.m_x, max.m_y, depth, 1);
    M44_matrixCoordTransform(&vt2, &mTrans, &v);
    setFloat4(v, min.m_x, max.m_y, depth, 1);
    M44_matrixCoordTransform(&vt3, &mTrans, &v);
    setFloat4(v, max.m_x, min.m_y, depth, 1);
    M44_matrixCoordTransform(&vt4, &mTrans, &v);
    Float4 vmin = min4(min4(vt1, vt2), min4(vt3,vt4));
    Float4 vmax = max4(max4(vt1, vt2), max4(vt3,vt4));

    Vec2d &vMin2 = (Vec2d&)vmin;
    Vec2d &vMax2 = (Vec2d&)vmax;

    m_params.getBoundingBoxWorking().setMin(vMin2);
    m_params.getBoundingBoxWorking().setMax(vMax2);
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::initNewParticleFollow(ITF_Particle* _par)
{
    _par->m_flag = 0;

    if (m_params.m_orientDir == 1)
        _par->m_flag |= PAR_F_ORIENTDIR;
    if (m_params.m_uniformScale)
        _par->m_flag |= PAR_F_UNIFORMSCALE;

    Vec3d randomVec = getRandomVector();
    _par->m_vel = randomVec * m_params.m_velocityVar;
    randomVec = getRandomVector();
    _par->m_velTarget = randomVec * m_params.m_velocityVar;
    f32 angularSpeedDelta = f32_Abs(m_params.m_angularSpeedDelta.Radians());
    _par->m_angularSpeed.SetRadians(randomLCG.getRandF(m_params.m_angularSpeed.Radians() - angularSpeedDelta, m_params.m_angularSpeed.Radians() + angularSpeedDelta));
    _par->m_angle.SetRadians(0.0f);

    SetUVFromUVmode(_par);

    //_par->m_initTime = m_currentTime;
    
    _par->m_pos.m_z = m_params.m_pos.m_z;
    /// TopLeft, TopRight, BottomLeft, BottomRight;
    Vec2d V[4];
    CAMERA->getScreenCornersAtZ(_par->m_pos.m_z, V);

    f32 xmin = m_params.m_pos.m_x + m_params.m_genBox.getMin().m_x;
    f32 xmax = m_params.m_pos.m_x + m_params.m_genBox.getMax().m_x;
    f32 ymax = m_params.m_pos.m_y + m_params.m_genBox.getMax().m_y;
    f32 ymin = m_params.m_pos.m_y + m_params.m_genBox.getMin().m_y;

    _par->m_pos.m_x = randomLCG.getRandF(xmin, xmax);
    _par->m_pos.m_y = randomLCG.getRandF(ymin, ymax);

    _par->m_timeTarget = m_params.m_timeTarget;
    _par->m_curtimeTarget = _par->m_timeTarget;
    _par->m_curPhase = 0;

    _par->m_indexUV = m_template->getStartAnimIndex();
    if (m_template->getStartAnimIndex() >= 0 && !m_template->getUseUVRandom())
    {
        _par->m_startAnimIndex = m_template->getStartAnimIndex();
        _par->m_endAnimIndex = m_template->getEndAnimIndex();
        _par->m_flag |= PAR_F_USEANIM;
        m_useAnim = btrue;
    }
    else
    {
        _par->m_flag &= ~PAR_F_USEANIM;
        m_useAnim = bfalse;
    }
    if (m_template->getLoop())
        _par->m_flag |= PAR_F_LOOP;
    else
        _par->m_flag &= ~PAR_F_LOOP;

    if ( m_template->getUseUVRandom() )
    {
        _par->m_indexUV = randomLCG.getRandI(m_template->getStartAnimIndex(), m_template->getEndAnimIndex());
    }

    m_params.initFromPhase(_par, m_currentTime, this);
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::synchronize()
{
    if (getTrueAABB())
        getParameters()->synchronizeBoundingBox();
}

Color ITF_ParticleGenerator::applyGlobalColor( const Color& _color ) const
{
    return _color * m_globalColor;
}

void ITF_ParticleGenerator::applyDefaultColor( const Color& _color )
{
    m_params.setDefaultColor(_color);
    m_globalColor.m_r = m_params.getDefaultColor().m_r;
    m_globalColor.m_g = m_params.getDefaultColor().m_g;
    m_globalColor.m_b = m_params.getDefaultColor().m_b;
    m_globalColor.m_a = m_params.getDefaultColor().m_a;
}

void ITF_ParticleGenerator::applyAmbiantColor( const Color& _color )
{
    m_ambColor = _color;
}

void ITF_ParticleGenerator::setFog(const Color& _color)
{
    m_mesh.m_colorFog = _color;
}

void ITF_ParticleGenerator::setRenderInTarget(u32 _val)
{
     m_renderInTarget = _val;
}

void ITF_ParticleGenerator::setColorFactor(const Color& _color)
{
    m_factorColor = _color;
}



///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::initNewParticleComplex(ITF_Particle* _par)
{
    Vec3d dir(0.f, 1.f, 0.f);
    _par->m_flag = 0;

    Vec3d GenPos = m_params.m_pos;
    if (getTemplate()->getIs2D() || m_params.useActorTranslation() )
    {
        GenPos = Vec3d::Zero;
    }

    if (m_params.m_orientDir == 1)
        _par->m_flag |= PAR_F_ORIENTDIR;
    if (m_params.m_uniformScale)
        _par->m_flag |= PAR_F_UNIFORMSCALE;

    switch (m_params.m_genGenType)
    {
    case PARGEN_GEN_CIRCLE:
    {
        if (m_params.m_nSplit > 1 && m_params.m_nSplit < 90)
        {
            if (!m_spliterAngle)
            {
                m_spliterAngle = newAlloc(mId_Particle, Angle[m_params.m_nSplit]);
                f32 max = m_params.getGenAngMin().ToRadians() + m_params.getAngle().ToRadians();
                f32 min = m_params.getGenAngMax().ToRadians() + m_params.getAngle().ToRadians();
                if (min < 0)
                    min = MTH_2PI - f32_Abs(min);

                u32 div = m_params.m_nSplit - 1;
                
                f32 dist = (max - min);
                
                if (f32_Abs(dist) > MTH_2PI)
                    dist = MTH_2PI;
                if (f32_Abs(dist) == MTH_2PI || dist == 0.f)
                    div++;

                f32 inc = dist / div;

                for (u32 i = 0; i < m_params.m_nSplit; i++)
                    m_spliterAngle[i].SetRadians(min + i*inc);
            }

            //generate
            dir = getRandomVector2d(m_spliterAngle[m_SplitCurPos].ToRadians() - m_params.m_splitDelta.ToRadians(), m_spliterAngle[m_SplitCurPos].ToRadians() + m_params.m_splitDelta.ToRadians());

            m_SplitCurPos++;
            if (m_SplitCurPos >= m_params.m_nSplit)
                m_SplitCurPos = 0;
        }
        else 
            dir = getRandomVector2d(m_params.getGenAngMin().ToRadians() - m_params.getAngle().ToRadians(), m_params.getGenAngMax().ToRadians() - m_params.getAngle().ToRadians());
        
        const f32 radius = randomLCG.getRandF(m_params.getInnerCircleRadius(), m_params.getCircleRadius());
        const Vec3d& scale = m_params.getScaleShape();
        dir.m_x *= scale.m_x;
        dir.m_y *= scale.m_y;
        // NO SCALE Z for circle --- > dir.m_z *= scale.m_z;

        const Vec3d& vrot = m_params.getRotateShape();

        if (!vrot.IsEqual(Vec3d::Zero,MTH_EPSILON))
        {
            //reorient dir
            Matrix44 rotX;
            Matrix44 rotY;
            Matrix44 mat;
            M44_setMatrixRotationX(&rotX, vrot.m_x);
            M44_setMatrixRotationY(&rotY, vrot.m_y);
            mat = rotX * rotY;
            M44_matrixVec3dTransform(&dir, &mat, &dir);
        }
        _par->m_pos = GenPos + dir * radius;

        if (m_params.m_randomizeDirection)
        {
            _par->m_vel = dir * (m_params.m_velNorm + randomLCG.getRandF(0.f, m_params.m_velocityVar));
        }
        else
        {
            _par->m_vel = m_params.m_vel * (m_params.m_velNorm + randomLCG.getRandF(0.f, m_params.m_velocityVar));
        }
        break;
    }

    case PARGEN_GEN_RECTANGLE:
    {
        dir = getRandomVector2d(m_params.getGenAngMin().ToRadians() - m_params.getAngle().ToRadians(), m_params.getGenAngMax().ToRadians() - m_params.getAngle().ToRadians());
        Vec2d localPos;
        localPos.m_x = randomLCG.getRandF(m_params.m_genBox.getMin().m_x, m_params.m_genBox.getMax().m_x);
        localPos.m_y = randomLCG.getRandF(m_params.m_genBox.getMin().m_y, m_params.m_genBox.getMax().m_y);
        const f32 angle = m_params.getAngle().ToRadians();
        _par->m_pos = GenPos + localPos.Rotate(angle).to3d();

        if (m_params.m_randomizeDirection)
        {
            _par->m_vel = _par->m_pos - GenPos;
            _par->m_vel = _par->m_vel.normalize() * (m_params.m_velNorm + randomLCG.getRandF(0.f, m_params.m_velocityVar)); 
        }
        else
        {
            _par->m_vel = m_params.m_vel * (m_params.m_velNorm + randomLCG.getRandF(0.f, m_params.m_velocityVar));
        }
        break;
    }

    case PARGEN_GEN_SPHERE:
    {
        if (m_params.m_randomizeDirection)
        {
            dir = getRandomVector();
            _par->m_vel = dir * (m_params.m_velNorm + randomLCG.getRandF(0.f, m_params.m_velocityVar)); 
        }
        else
        {
            _par->m_vel = m_params.m_vel * (m_params.m_velNorm + randomLCG.getRandF(0.f, m_params.m_velocityVar));
        }
        _par->m_pos = GenPos;
        break;
     }

    case PARGEN_GEN_HEMISPHERE:
    {
        dir = getRandomVectorInter(-MTH_PI, MTH_PI, 0.f, 1.f);
        dir.normalize();
        f32 radius = randomLCG.getRandF(m_params.getInnerCircleRadius(), m_params.getCircleRadius());

        _par->m_pos = GenPos + dir * radius;

        if (m_params.m_randomizeDirection)
        {
            _par->m_vel = dir * (m_params.m_velNorm + randomLCG.getRandF(0.f, m_params.m_velocityVar)); 
        }
        else
        {
            _par->m_vel = m_params.m_vel * (m_params.m_velNorm + randomLCG.getRandF(0.f, m_params.m_velocityVar));
        }
        break;
    }

    case PARGEN_GEN_POINTS:
    default:
        dir = getRandomVector();
        _par->m_vel = m_params.m_vel*m_params.m_velNorm;
        _par->m_vel += dir  * m_params.m_velocityVar;
        _par->m_pos = GenPos;
        break;
    }
    
    if (m_params.isCartoon())
    {
        // special z for cartoon.
        _par->m_pos.m_z = GenPos.m_z + (m_totalParticlesGenerated % m_params.m_maxParticles) * 0.0001f;
        _par->m_zsort = _par->m_pos.m_z - randomLCG.getRandF(0.00015f, 0.00015f * m_params.m_randomoutline);
    }

    /// Orient to dir.
    if (_par->m_flag & PAR_F_ORIENTDIR)
        _par->m_angle.SetRadians(MTH_RADTODEG * atan2f(dir.m_y, dir.m_x));
    else
    {
        f32 angleDelta = f32_Abs(m_params.m_angleDelta.Radians());
        _par->m_angle.SetRadians(randomLCG.getRandF(m_params.m_initAngle.Radians() - angleDelta, m_params.m_initAngle.Radians() + angleDelta));

        f32 angularSpeedDelta = f32_Abs(m_params.m_angularSpeedDelta.Radians());
        _par->m_angularSpeed.SetRadians(randomLCG.getRandF(m_params.m_angularSpeed.Radians() - angularSpeedDelta, m_params.m_angularSpeed.Radians() + angularSpeedDelta));
    }

    SetUVFromUVmode(_par);

    _par->m_initTime  = m_currentTime;
    _par->m_curPhase = 0;
    _par->m_indexUV = m_template->getStartAnimIndex();

    /// Anim.
    if (m_template->getStartAnimIndex() >= 0 && !m_template->getUseUVRandom())
    {
        _par->m_startAnimIndex = m_template->getStartAnimIndex();
        _par->m_endAnimIndex = m_template->getEndAnimIndex();
        _par->m_flag |= PAR_F_USEANIM;
        m_useAnim = btrue;
    }
    else
    {
        _par->m_flag &= ~PAR_F_USEANIM;
        m_useAnim = bfalse;
    }

    if (m_template->getLoop())
        _par->m_flag |= PAR_F_LOOP;
    else
        _par->m_flag &= ~PAR_F_LOOP;

    if ( m_template->getUseUVRandom() )
    {
        //_par->m_indexUV = (i32)random.GetU32(m_startAnimIndex*100, (m_endAnimIndex+1)*100) / 100;
        _par->m_indexUV = randomLCG.getRandI(m_template->getStartAnimIndex(), m_template->getEndAnimIndex());
    }

    m_params.initFromPhase(_par, m_currentTime, this);
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::spawnOneParticle()
{
    addOneNewComplexParticle();
}

void ITF_ParticleGenerator::SetUVFromUVmode(ITF_Particle* _par)
{
    switch (m_params.m_UVmode)
    {
    default:
    case UV_Default:
        break;
    case UV_FlipX:
        _par->m_flag |= PAR_F_UVINVERT_X;
        break;
    case UV_FlipY:
        _par->m_flag |= PAR_F_UVINVERT_Y;
        break;
    case UV_FlipXY:
        _par->m_flag |= PAR_F_UVINVERT_X;
        _par->m_flag |= PAR_F_UVINVERT_Y;
        break;
    case UV_FlipXtoDirX:
        if (_par->m_vel.m_x < 0.f)
            _par->m_flag |= PAR_F_UVINVERT_X;
        break;
    case UV_FlipYtoDirY:
        if (_par->m_vel.m_y < 0.f)
            _par->m_flag |= PAR_F_UVINVERT_Y;
        break;
    case UV_FlipXYtoDirXY:
        if (_par->m_vel.m_x < 0.f)
            _par->m_flag |= PAR_F_UVINVERT_X;
        if (_par->m_vel.m_y < 0.f)
            _par->m_flag |= PAR_F_UVINVERT_Y;
        break;
    }
}

///----------------------------------------------------------------------------//

BEGIN_SERIALIZATION(ITF_ParticleGenerator_Template)
    SERIALIZE_MEMBER("computeAABB",m_computeAABB);
    SERIALIZE_MEMBER("useAnim",m_useAnim);
    SERIALIZE_MEMBER("loop", m_loop);
    SERIALIZE_MEMBER("useUVRandom",m_useUVRandom);
    SERIALIZE_MEMBER("animstart",m_startAnimIndex);
    SERIALIZE_MEMBER("animend",m_endAnimIndex);
    SERIALIZE_MEMBER("AnimUVfreq",m_AnimUVfreq);
    SERIALIZE_OBJECT("params",m_params);
    SERIALIZE_MEMBER("is2D", m_is2D);
END_SERIALIZATION()

IMPLEMENT_OBJECT_RTTI(ITF_ParticleGenerator_Template)

ITF_ParticleGenerator_Template::ITF_ParticleGenerator_Template()
{
    m_computeAABB       = 0;
    m_useAnim           = 0;
    m_loop              = 0;
    m_startAnimIndex    = -1;
    m_endAnimIndex      = -1;
    m_AnimUVfreq        = 1.f;
    m_useUVRandom       = 0;
    m_is2D              = bfalse;
}

///----------------------------------------------------------------------------//
/// Particles Z list.
///----------------------------------------------------------------------------//

void PAR_Zlist::AddParticle(ITF_ParticleOrder* m_part)
{
    ITF_ParticleOrder* pNode;
    pNode = AddNode();

    if (!pNode) return;

    *pNode = *m_part;
}

///----------------------------------------------------------------------------//

ITF_ParticleOrder* PAR_Zlist::AddNode()
{
    ITF_ParticleOrder* pNewNode = &m_BufferNode[m_NumberEntry];
    m_reorderingTable[m_NumberEntry] = pNewNode;

    m_NumberEntry++;

    return pNewNode;
}

///----------------------------------------------------------------------------//

void PAR_Zlist::reset()
{
    m_NumberEntry = 0;
}

///----------------------------------------------------------------------------//

void PAR_Zlist::init()
{
}

///----------------------------------------------------------------------------//

struct ParticleOrderPtrZLess
{
    bool operator()(ITF_ParticleOrder * _r, ITF_ParticleOrder * _l)
    {
        return _r->m_zsort < _l->m_zsort;
    }
};

int parsorting(const void* n1, const void* n2)
{
    ITF_ParticleOrder* a = *(ITF_ParticleOrder**) n1;
    ITF_ParticleOrder* b = *(ITF_ParticleOrder**) n2;
       
    if( a->m_zsort < b->m_zsort ) return -1;
    else if( a->m_zsort == b->m_zsort ) return 0;
    else return 1;
}

///----------------------------------------------------------------------------//

void PAR_Zlist::sort()
{
    std::sort(m_reorderingTable, m_reorderingTable+m_NumberEntry, ParticleOrderPtrZLess());
}

///----------------------------------------------------------------------------//

void PAR_Zlist::setSize(u32 _number)
{
    reset();
    m_BufferNode.resize(_number);

    if (m_reorderingTable)
        delete[] m_reorderingTable;

    m_reorderingTable = newAlloc(mId_Particle, ITF_ParticleOrder*[_number]);
    for (u32 i = 0; i < _number;i++)
    {
        m_reorderingTable[i] = &m_BufferNode[i];
    }
}

} // namespace ITF


