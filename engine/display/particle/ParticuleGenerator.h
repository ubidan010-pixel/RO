#ifndef _ITF_PARTICULEGENERATOR_H_
#define _ITF_PARTICULEGENERATOR_H_

#include "core/types.h"

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_BV_AABB_H_
#include "core/boundingvolume/AABB.h"
#endif //_ITF_BV_AABB_H_

#ifndef _ITF_PARTICULEGENERATORPARAMETERS_H_
#include "engine/display/particle/ParticuleGeneratorParameters.h"
#endif //_ITF_PARTICULEGENERATORPARAMETERS_H_

#ifndef _ITF_COLORINTEGER_H_
#include "core/ColorInteger.h"
#endif //_ITF_COLORINTEGER_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

//#define PARTICLE_GENERATOR_USE_PLACEMENT_NEW

namespace ITF
{
    class RandomGenerator
	{
		protected:

							RandomGenerator();
		virtual				~RandomGenerator();

		mutable	i32			mSeed;
		public:

				void		SetNewSeed();
		inline	i32			GetSeed()		const		{ return mSeed; }

		virtual	void		SetSeed(i32 s) = 0;

		virtual	u32			RandI()			const	= 0;	// 0..2^31
		virtual	float		RandF()			const			// 0.0 .. 1.0 float generator
							{
								// default: multiply by 1/(2^31)
								return float(RandI()) * float(1.0f/2147483647.0f);
							}

				i32		getRandI(i32 i, i32 n)		// i..n integer generator
							{
								//ASSERT(i<=n);
								return i + (RandI() % (n - i + 1));
							}   

				float		getRandF(float i, float n)			// i..n float generator
							{
								//ASSERT(i<=n);
								return (i + (n - i) * RandF());   
							}   
	};

    // Linear Congruential Method
    class RandomLCG : public RandomGenerator
	{
		public:
							RandomLCG();
							RandomLCG(i32 s);

		virtual        void		SetSeed(i32 s);
		virtual        u32		RandI() const;

		protected:
		static	const i32 msQuotient;
		static	const i32 msRemainder;
	};

class renderTarget;


/// flags:

const int PAR_F_ORIENTDIR           = 0x00000001; // Orient angle in particle direction.
const int PAR_F_UNIFORMSCALE        = 0x00000002; // Uniform scale scale y = scale x.
const int PAR_F_USEANIM             = 0x00000004; // Uniform scale scale y = scale x.
const int PAR_F_ANIM_STRETCHTIME    = 0x00000008;
const int PAR_F_ANIM_BLEND          = 0x00000010;
const int PAR_F_BLENDPHASE          = 0x00000020;
const int PAR_F_LOOP                = 0x00000040; // allow particle loop
const int PAR_F_UVINVERT_X          = 0x00000080; // invert uv x
const int PAR_F_UVINVERT_Y          = 0x00000100; // invert uv y

struct ITF_Particle
{
    ITF_Particle():m_pivot(Vec2d::Zero),m_curSize(Vec2d::Zero),m_initSize(Vec2d::Zero),m_dstSize(Vec2d::Zero)
    {
        m_indexUV = 0;
        m_bAlive = bfalse;
    }

    ITF_INLINE void live()  {m_bAlive = btrue;}
    ITF_INLINE void dead()  {m_bAlive = bfalse;}
    
    u32             m_flag:31;
    u32             m_bAlive:1;
    i32             m_startAnimIndex;
    i32             m_endAnimIndex;
    f32             m_zsort;

    u32             m_curPhase;

    f32				m_initTime;
    f32             m_timeTarget;
    f32             m_curtimeTarget;
    f32             m_phaselifeTime;
    ITF_Particle	*m_pNext;

    i32             m_indexUV;

    Angle			m_angle;
    Angle			m_angularSpeed;

    Vec2d			m_curSize;
    Vec2d			m_initSize;
    Vec2d			m_dstSize;

    Vec3d			m_pos;
    Vec2d           m_pivot;
	Vec3d			m_vel;
    Vec3d			m_velTarget;
	
	ColorInteger    m_curColor;
	ColorInteger    m_initColor;
    ColorInteger	m_dstColor;
};

struct ITF_ParticleOrder
{
    ITF_Particle* m_part;
    u32 m_mask;
    f32 m_zsort;
};

class PAR_Zlist
{
public:
    PAR_Zlist()
    {
        m_NumberEntry = 0;
        m_reorderingTable = NULL;
        init();
    }

    ~PAR_Zlist()
    {
        if (m_reorderingTable)
            delete[] m_reorderingTable;
    }

    void AddParticle(ITF_ParticleOrder* m_part);

    ITF_INLINE u32 getNumberEntry() {return m_NumberEntry;};

    void sort();
    ITF_INLINE ITF_ParticleOrder* getOrderNodeAt(u32 _val) {return m_reorderingTable[_val];};
    void reset();
    void setSize(u32 _number);

protected:

    void init();

    ITF_ParticleOrder* AddNode();

    SafeArray<ITF_ParticleOrder> m_BufferNode;
    ITF_ParticleOrder** m_reorderingTable;

    u32 m_NumberEntry;
};

#if 0
	ITF_Particle Generate()
	{
		ITF_Particle a;

		a.m_pos = m_pos;
		a.m_vel = m_vel;
		Vector orthogonal(a.m_vel);
		
		Vec_Rotate(&orthogonal, 23.f);
		Vec_Cross(&orthogonal, &a.m_vel, &orthogonal);		
		f32 rn = rndf(0.f,1.f);

		Vec_ArbitraryRotate(&a.m_vel, m_angle_of_cut_out+(m_angle_of_scatter - m_angle_of_cut_out)*rn, orthogonal);
		Vec_ArbitraryRotate(&a.m_vel, rndf(0.f, 360.f), m_vel);

		a.m_color = E3D_Color_RND(m_ColorMin, m_ColorMax);
		
		a.m_grav = m_grav;
		a.m_size = m_size;
		a.m_lifetime = m_lifetime - 0.2f * m_lifetime * rndf(0.f,1.f);
		a.m_t = 0;
		m_time_to_add_particle += m_lifetime/(f32)m_particlesMax;
		return a;
	}
};
#endif

class JobUpdateParticle;
class ITF_ParticleGenerator
{

public:
    #ifdef PARTICLE_GENERATOR_USE_PLACEMENT_NEW
        void* operator new(size_t, void*);
    #endif
    ITF_ParticleGenerator();

	~ITF_ParticleGenerator();

	void init( const class ITF_ParticleGenerator_Template* _template );
    void clear();
	void reInit();
    void construct();
    void destruct();
	void update(f32 _dt, bbool _bDisableCreation = bfalse);

    bbool render();
    bbool drawParticles(bbool _direct = bfalse);
    void lightProcess();
    void addlight(const LightInfo &_light);
    void impostorRender();

    u32 FillZsort(Texture* pTexture, VertexPCT* pdata);
    void fillNormal(const ITF_Particle *__restrict pParticle,const Texture* __restrict pTexture, VertexPCT* pdata, Vec2d _scaleUV);

    void applyDefaultColor(const Color& _col);
    void applyAmbiantColor(const Color& _col);
    ITF_INLINE Color getAmbiantColor() {return m_ambColor;}

    void    setFog(const Color& _color);
    void    setColorFactor(const Color& _color);
    f32     getAlpha() const { return m_alpha; }
	void    setAlpha(f32 _alpha) { m_alpha = _alpha; }
    void    setRenderInTarget(u32 _val);

    void updateParticleAnim(ITF_Particle* _particle, f32 _time);
    void SetUVFromUVmode(ITF_Particle* _par);

    void updateAABB(ITF_Particle* _particle);
    void transformBoundingBoxWithMatrix();
    void computeTransformMatrix();

    void synchronize();

    static ITF_ParticleGenerator* getGenerator(i32& _entryIndex);
    static void freeGenerator(ITF_ParticleGenerator* _gen, i32 _entryIndex = -1);
    static void FlushGenerators();

    //This operator is forbidden
    const ITF_ParticleGenerator& operator = (const ITF_ParticleGenerator& src)
    {
        ITF_ASSERT(0);
        return ITF_ParticleGenerator::null_object;
    }

    static ITF_ParticleGenerator null_object;

    void setMaxParticles( u32 _maxParticles );
    ITF_INLINE ParticleGeneratorParameters * getParameters() { return & m_params;}
    ITF_INLINE const ParticleGeneratorParameters * getParametersConst() const { return &m_params; }
    ITF_INLINE bbool getActive( ) { return m_activeParticlesCount > 0 ; }

    ITF_INLINE bbool isUseAnim() { return m_useAnim; }
    ITF_INLINE void setTrueAABB(u32 _val) { m_computeAABB = _val;};
    ITF_INLINE u32  getTrueAABB() const { return m_computeAABB; }
 
    ITF_INLINE void setAnimUVfreq(f32 _freq);

    void        spawnOneParticle();
    void        spawnNParticle( u32 _numParticles, f32 maxTimeRange );
    ITF_INLINE void        startGeneration() {m_generationStarted = btrue;}
    ITF_INLINE void        stopGeneration() {m_generationStarted = bfalse;}
    ITF_INLINE bbool        isGenerationStarted() {return m_generationStarted;}
    ITF_INLINE u32  getNumGeneratedParticles() const { return m_totalParticlesGenerated; }
    ITF_INLINE u32  getActiveParticlesCount() const { return m_activeParticlesCount; }
    ITF_INLINE u32  getMaxParticlesCount() const { return m_maxParticlesNeed; }
    Color applyGlobalColor( const Color& _color) const;

    void startManualUpdate( const Vec3d& _pos );
    void updateManualParticle( u32 _index, const Vec3d& _pos, const Vec3d& _speed, Angle _rot );
    void changeManualParticleState( u32 _index, bbool _isAlive);
    void changeManualParticleFlip( u32 _index, bbool _isFlip);
	void changeManualParticleSize(const u32 _index, const f32 _size);
    void changeManualParticleColor(const u32 _index, const Color &_color);
	void changeManualParticleAlpha(const u32 _index, const u8 _alpha);
    void changeManualAllParticlesColor(const Color & _color);
    void changeManualAllParticlesSize(const f32 _size);

    ITF_INLINE Texture* getTextureFromImpostor();

    ITF_INLINE i32 getStartAnimIndex() const;
    ITF_INLINE i32 getEndAnimIndex() const;


    ITF_INLINE f32 getCurrentTime() const {return m_currentTime;}
    ITF_INLINE void incCurrentTime(f32 _dt)  {m_currentTime += _dt;}
    ITF_INLINE u32  isComputeAABB()  const   {return m_computeAABB;}
    ITF_INLINE void setParticlesCountVtxToDisplay(u32 _particlesCountVtxToDisplay) {m_particlesCountVtxToDisplay = _particlesCountVtxToDisplay;}
#ifdef ITF_SUPPORT_MULTITHREADED
    ITF_INLINE u8* getCachedMemory() {return m_cacheMemory;}
#endif // ITF_SUPPORT_MULTITHREADED
    //TODO public until I refactor
    ITF_Particle*       m_pActiveList;

    u32				    m_activeParticlesCount;
    f32				    m_lastUpdateTime;
    f32				    m_lastUpdateRest;   // fraction of a particle that should have been emitted last frame, but got rounded off
    bbool               m_generationStarted;

    u32                 m_totalParticlesGenerated;

    ITF_Particle*   allocateParticle();
    void deallocateParticle(ITF_Particle* pParticle);

    void initNewParticleFollow(ITF_Particle* _par);
    void addOneNewComplexParticle();
    bbool checkGeneratorValidity();
    const String& getValidityMessage() const {return m_validityMessage;}

    void                setLocalRotationZ(f32 _az) {m_LocalRotationZ = _az;}
    void                setLocalScale(Vec2d _scale) {m_LocalScale = _scale;}

    u32                 m_maxParticlesNeed;

    ITF_Particle*       m_pParticleArrayIndex;

    SafeArray<LightInfo,2>  m_lights;

    const class ITF_ParticleGenerator_Template* getTemplate() const { return m_template; }

    ITF_INLINE VertexPCT* LockVB(ITF_VertexBuffer **_pVB)
    {
        VertexPCT* data = NULL;

#ifndef VBMANAGER_USE_DYNAMICRING_VB
        m_mesh.m_userBuffer = !m_mesh.m_userBuffer;
        u32 currentBuffer = m_mesh.m_userBuffer;
        *_pVB = m_mesh.m_vtxBufferBuffered[currentBuffer];
#else
        *_pVB = m_mesh.m_vtxBuffer;    
#endif // VBMANAGER_USE_DYNAMICRING_VB

        GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(*_pVB,(void**)&data);
        ITF_ASSERT_MSG(data, "Unable to lock a vertex buffer");

        return data;
    }

    ITF_INLINE void UnlockVB(ITF_VertexBuffer *_pVB)
    {
        ITF_ASSERT(_pVB);
        GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(_pVB);
    }


    static void    initPool();
    static void    freePool();

private:
    struct ParticleGeneratorPoolEntry
    {
        static const u32 counter_available = 0;
        static const u32 counter_allocated = 0xffffffff;
        ParticleGeneratorPoolEntry()
        {
            m_gen = NULL;
            m_unavailableCounter = counter_allocated;
        }
        ITF_ParticleGenerator*  m_gen;
        u32                     m_unavailableCounter;
    };

    static const u32 m_poolSize = 512;
    static SafeArray<ParticleGeneratorPoolEntry> m_pool;
    static SafeArray<u32>       m_indexesToFree;
    static SafeArray<u32>       m_freeIndexes;

#ifndef ITF_SUPPORT_MULTITHREADED
    void        fillActiveList(ITF_VertexBuffer *_VB);
    void        fillParticuleArray(ITF_VertexBuffer *_VB);
    void        fillParticles(ITF_VertexBuffer *_VB);
#endif // ITF_SUPPORT_MULTITHREADED

private:

    const class ITF_ParticleGenerator_Template* m_template;
    SafeArray<u32>      m_pFreeParticleIndex;

    renderTarget*       m_renderTargetImpostor;
    GFX_3DQUAD          m_3dQuad;

 
    void initNewParticleComplex(ITF_Particle* _par);

    ParticleGeneratorParameters m_params;

#ifdef ITF_SUPPORT_MULTITHREADED
    u8*                 m_cacheMemory;//TODO move it using a pool
#endif // ITF_SUPPORT_MULTITHREADED
    u32                 m_particlesCountVtx;
    u32                 m_particlesCountVtxToDisplay;
    u32                 m_computeAABB;

    JobUpdateParticle*  m_jobParticle;

	/// Engine.
	f32				    m_currentTime;
	
    bbool               m_isRender;

    // When in manual mode we spawn an array of particles which we can access through an index
    ITF_Particle*       m_particleArray;

    /// Mesh.
    ITF_Mesh            m_mesh;

    Color               m_globalColor;
    Color               m_ambColor;
    Color               m_factorColor;
    f32                 m_alpha;
    bbool               m_renderInTarget;

    /// z sorting.
    PAR_Zlist*          m_particlesZOrdering;

    bbool               m_checkValidity;
    String              m_validityMessage;

    bbool               m_aabbNeedCompute;
    bbool               m_useAnim;

    u32                 m_SplitCurPos;
    Angle*              m_spliterAngle;

    // for matrix orientation and scale:
    f32                 m_LocalRotationZ;
    Vec2d               m_LocalScale;
    GMatrix44           m_transformMatrix;
};

class ITF_ParticleGenerator_Template : public TemplateObj
{
    DECLARE_OBJECT_CHILD_RTTI(ITF_ParticleGenerator_Template, TemplateObj,1063724644);
    DECLARE_SERIALIZE()

public:

    ITF_ParticleGenerator_Template();
    ~ITF_ParticleGenerator_Template() {}

    u32                                         getComputeAABB() const { return m_computeAABB; }
    u32                                         getUseAnim() const { return m_useAnim; }
    u32                                         getLoop() const { return m_loop; }
    u32                                         getUseUVRandom() const { return m_useUVRandom; }
    i32                                         getStartAnimIndex() const { return m_startAnimIndex; }
    i32                                         getEndAnimIndex() const { return m_endAnimIndex; }
    f32                                         getAnimUVFreq() const { return m_AnimUVfreq; }
    const ParticleGeneratorParameters&          getParameters() const { return m_params; }
    bbool                                       getIs2D() const {return m_is2D;}

private:

    u32                                         m_computeAABB;
    u32                                         m_useAnim;
    u32                                         m_loop;
    u32                                         m_useUVRandom;
    i32                                         m_startAnimIndex;
    i32                                         m_endAnimIndex;
    f32                                         m_AnimUVfreq;
    ParticleGeneratorParameters                 m_params;
    bbool                                       m_is2D;
};

ITF_INLINE i32 ITF_ParticleGenerator::getStartAnimIndex() const { return m_template->getStartAnimIndex(); }
ITF_INLINE i32 ITF_ParticleGenerator::getEndAnimIndex() const { return m_template->getEndAnimIndex(); }

} // namespace ITF

#endif // _ITF_PARTICULEGENERATOR_H_
