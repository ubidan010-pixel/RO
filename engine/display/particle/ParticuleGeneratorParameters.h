#ifndef _ITF_PARTICULEGENERATORPARAMETERS_H_
#define _ITF_PARTICULEGENERATORPARAMETERS_H_

namespace ITF
{
    struct ITF_Particle;

    enum PARGEN_GEN_TYPE
    {
        PARGEN_GEN_POINTS = 0,
        PARGEN_GEN_RECTANGLE,
        PARGEN_GEN_CIRCLE,
        PARGEN_GEN_SPHERE,
        PARGEN_GEN_HEMISPHERE,
        ENUM_FORCE_SIZE_32(PARGEN_GEN_TYPE)
    };

    enum PARGEN_MODE
    {
        PARGEN_MODE_FOLLOW = 0,
        PARGEN_MODE_COMPLEX,
        PARGEN_MODE_MANUAL,
        ENUM_FORCE_SIZE_32(PARGEN_MODE)
    };

    enum UVMODE : i32
    {
        UV_Default = 0,
        UV_FlipX,
        UV_FlipY,
        UV_FlipXY,
        UV_FlipXtoDirX,
        UV_FlipYtoDirY,
        UV_FlipXYtoDirXY,
    };

    struct ParPhase
    {
        DECLARE_SERIALIZE()

        ParPhase() 
        {
            m_phaseTime = 1.f;
            m_colorMin = Color::white();
            m_colorMax = Color::white();
            m_sizeMin = Vec2d(1.f, 1.f);
            m_sizeMax = Vec2d(1.f, 1.f);
            m_animStart = -1;
            m_animEnd = -1;
            m_deltaphasetime = 0.f;
            m_animstretchtime = bfalse;
            m_blendToNextPhase = btrue;
        };

        f32     m_phaseTime;
        f32     m_deltaphasetime;
        Color   m_colorMin;
        Color   m_colorMax;
        Vec2d   m_sizeMin;
        Vec2d   m_sizeMax;
        i32     m_animStart;
        i32     m_animEnd;
        bbool   m_animstretchtime;
        bbool   m_blendToNextPhase;
    };

    class ParticleGeneratorParameters
    {
    private:

        AABB                m_bounding;
        AABB                m_boundingWorking;//the bounding box used by thread

    public:        
        DECLARE_SERIALIZE()
        ParticleGeneratorParameters();

        void setMaxParticles( u32 _maxParticles ) { m_maxParticles = _maxParticles; }
        ITF_INLINE u32 getMaxParticles(	) const { return m_maxParticles; }

        ITF_INLINE void setNumToEmit( u32 _emitParticlesCount ) { m_emitParticlesCount = _emitParticlesCount; }
        ITF_INLINE u32 getNumToEmit(	) const { return m_emitParticlesCount; }

        ITF_INLINE void setEmitInterval( f32 _emitInterval ) { m_emitInterval = _emitInterval; }
        ITF_INLINE f32 getEmitInterval(	) const { return m_emitInterval; }

        ITF_INLINE void setFrequency( f32 _freq ) { m_freq = _freq; }
        ITF_INLINE f32 getFrequency(	) const { return m_freq; }

        ITF_INLINE void setPosition( Vec3d _pos ) { m_pos = _pos; }
        ITF_INLINE const Vec3d& getPosition(	) const { return m_pos; }

        ITF_INLINE void setDepth( f32 _depth ) { m_depth = _depth; }
        ITF_INLINE f32  getDepth(	) const { return m_depth; }
        ITF_INLINE void useDepth( bbool _useDepth ) { m_useZAsDepth = !_useDepth; }

        ITF_INLINE void         setVelocity( Vec3d _vel ) { m_vel = _vel; }
        ITF_INLINE Vec3d        getVelocity(	) const { return m_vel; }

        ITF_INLINE void         setVelocityNorm( f32 _velNorm ) { m_velNorm = _velNorm; }
        ITF_INLINE f32          getVelocityNorm(	) const { return m_velNorm; }

        ITF_INLINE void         setFriction( f32 _friction) { m_friction = _friction; }
        ITF_INLINE f32          getFriction(	) const { return m_friction; }

        ITF_INLINE void         setGravity( Vec3d _grav ) { m_grav = _grav; }
        ITF_INLINE const Vec3d& getGravity(	) const { return m_grav; }

        ITF_INLINE void         setAcc( Vec3d _acc ) { m_acc = _acc; }
        ITF_INLINE const Vec3d& getAcc(	) const { return m_acc; }

        ITF_INLINE void         setTexture( ResourceID _texture ) { m_texture = _texture; }
        ITF_INLINE ResourceID&  getTexture(	) { return m_texture; }

        ITF_INLINE void     setAngularSpeed( Angle _angSpeed ) { m_angularSpeed = _angSpeed; }
        ITF_INLINE Angle    getAngularSpeed(	) const { return m_angularSpeed; }

        ITF_INLINE void     setAngularSpeedDelta( Angle _angSpeedDt ) { m_angularSpeedDelta = _angSpeedDt; }
        ITF_INLINE Angle    getAngularSpeedDelta(	) const { return m_angularSpeedDelta; }

        ITF_INLINE void     setAngle( Angle _min ) { m_initAngle = _min; }
        ITF_INLINE Angle    getAngle(	) const { return m_initAngle; }

        ITF_INLINE void     setAngleDelta( Angle _delta ) { m_angleDelta = _delta; }
        ITF_INLINE Angle    getAngleDelta(	) const { return m_angleDelta; }

        ITF_INLINE void     setTimeTarget( f32 _time ) { m_timeTarget = _time; }
        ITF_INLINE f32      getTimeTarget(	) const { return m_timeTarget; }

        ITF_INLINE void     setVelocityVar( f32 _velocityVar ) { m_velocityVar = _velocityVar; }
        ITF_INLINE f32      getVelocityVar() const { return m_velocityVar; }

        ITF_INLINE void             setGeneratorGenType( PARGEN_GEN_TYPE _genType ) { m_genGenType = _genType; }
        ITF_INLINE PARGEN_GEN_TYPE  getGeneratorGenType() const { return m_genGenType; }

        ITF_INLINE void             setGeneratorMode( PARGEN_MODE _genMode ) { m_genMode = _genMode; }
        ITF_INLINE PARGEN_MODE      getGeneratorMode() const { return m_genMode; }

        ITF_INLINE void             setRenderPrio(f32 _renderPrio) { m_renderPrio = _renderPrio; }
        ITF_INLINE f32              getRenderPrio() const { return m_renderPrio; }
        
        ITF_INLINE void             setBlendMode(GFX_BLENDMODE _mode) { m_blendMode = _mode;}
        ITF_INLINE GFX_BLENDMODE    getBlendMode() const { return m_blendMode;}

        ITF_INLINE void         setBoxMin( Vec2d _vMin ) { m_genBox.setMin(_vMin); }
        ITF_INLINE const Vec2d& getBoxMin() const { return m_genBox.getMin(); }

        ITF_INLINE void         setBoxMax( Vec2d _vMax ) { m_genBox.setMax(_vMax); }
        ITF_INLINE const Vec2d& getBoxMax() const { return m_genBox.getMax(); }

        ITF_INLINE void         setAABBMin( Vec2d _vMin ) { m_bounding.setMin(_vMin); }
        ITF_INLINE const Vec2d& getAABBMin() const { return m_bounding.getMin(); }

        ITF_INLINE void         setAABBMax( Vec2d _vMax ) { m_bounding.setMax(_vMax); }
        ITF_INLINE const Vec2d& getAABBMax() const { return m_bounding.getMax(); }

        ITF_INLINE void     setCircleRadius(f32 _rad) {m_circleRadius = _rad;}
        ITF_INLINE f32      getCircleRadius() const {return m_circleRadius;}

        ITF_INLINE void     setInnerCircleRadius(f32 _rad) {m_innerCircleRadius = _rad;}
        ITF_INLINE f32      getInnerCircleRadius()const  {return m_innerCircleRadius;}

        ITF_INLINE void         setScaleShape(Vec3d _scale) {m_scaleShape = _scale;}
        ITF_INLINE const Vec3d& getScaleShape()const  {return m_scaleShape;}

        ITF_INLINE void         setRotateShape(Vec3d _rot) {m_rotateShape = _rot;}
        ITF_INLINE const Vec3d& getRotateShape()const  {return m_rotateShape;}

        ITF_INLINE void     setOrientToDir(u32 _val) { m_orientDir = _val;}
        ITF_INLINE void     setUniformScale(u32 _val) { m_uniformScale = _val;}

        ITF_INLINE void     setUVmode(UVMODE _mode) { m_UVmode = _mode;}
        ITF_INLINE UVMODE   getUVmode() { return m_UVmode; }
 
        ITF_INLINE void     useCartoon(bbool _val) { m_useCartoon = _val;}
        ITF_INLINE bbool    isCartoon() const  { return m_useCartoon;}

        ITF_INLINE void     setOutLineSize(f32 _size) {m_outlinesize = _size;}
        ITF_INLINE f32      getOutLineSize()const  {return m_outlinesize;}

        ITF_INLINE void     setRandomOutLine(u32 _rnd) {m_randomoutline = _rnd;}
        ITF_INLINE u32      getRandomOutLine()const  {return m_randomoutline;}

        ITF_INLINE void             setPivot( const Vec2d& _pivot ) { m_pivot = _pivot; }
        ITF_INLINE const Vec2d&     getPivot() const { return m_pivot; }

        ITF_INLINE bbool    isImpostor() { return m_useImpostor;}
        ITF_INLINE bbool    isShowImpostorRender() { return m_showImpostorRender;}

        ITF_INLINE bbool    isUseTextureMirrorU() const  { return m_textureMirrorU;}
        ITF_INLINE bbool    isUseTextureMirrorV() const  { return m_textureMirrorV;}

        ITF_INLINE void     setGenAngMin( Angle _ang ) { m_genAngMin = _ang; }
        ITF_INLINE Angle    getGenAngMin(	) const { return m_genAngMin; }

        ITF_INLINE void     setGenAngMax( Angle _ang ) { m_genAngMax = _ang; }
        ITF_INLINE Angle    getGenAngMax(	) const { return m_genAngMax; }

        ITF_INLINE void     setDefaultColor( Color _color ) { m_defaultColor = _color; }
        ITF_INLINE Color    getDefaultColor(	) const { return m_defaultColor; }

        bbool getDisableLight() const       { return m_disableLight; }
		void				setDisableLight(bbool _v) {m_disableLight = _v;}

        AABB& getGenBox()                   {return m_genBox;}
        AABB& getBoundingBox()              {return m_bounding;}
        AABB& getBoundingBoxWorking()       {return m_boundingWorking;}


        void setWorkingBoundingBox(const AABB& _aabb) {m_boundingWorking = _aabb;}

        void synchronizeBoundingBox() {m_bounding = m_boundingWorking;}

        void setBoundingBox(const AABB& _aabb) {m_bounding = _aabb;}

        f32 computeSumPhaseTime() const;
        /// Phases.
        void setNumberPhase( u32 _nphase );
        void addPhase( );

        ITF_INLINE u32 getNumberPhase( ) const {return m_nbPhase;} 
        void initFromPhase(ITF_Particle* _par, f32 _time, class ITF_ParticleGenerator* _owner);
        void initToNextPhase(ITF_Particle* _par, f32 _time, class ITF_ParticleGenerator* _owner);
        void getDstBlend(ITF_Particle* _par, class ITF_ParticleGenerator* _gen);

        ITF_INLINE void     setPhaseTime( u32 _nphase, f32 _time ) { m_phaseList[_nphase].m_phaseTime = _time; }
        ITF_INLINE f32      getPhaseTime(    u32 _nphase	) const { return m_phaseList[_nphase].m_phaseTime; }

        ITF_INLINE void     setDeltaPhaseTime( u32 _nphase, f32 _time ) { m_phaseList[_nphase].m_deltaphasetime = _time; }
        ITF_INLINE f32      getDeltaPhaseTime(    u32 _nphase	) const { return m_phaseList[_nphase].m_deltaphasetime; }

        ITF_INLINE void     setPhaseAnimStart( u32 _nphase, i32 _v ) { m_phaseList[_nphase].m_animStart = _v; }
        ITF_INLINE i32      getPhaseAnimStart(    u32 _nphase	) const { return m_phaseList[_nphase].m_animStart; }

        ITF_INLINE void     setPhaseAnimEnd( u32 _nphase, i32 _v ) { m_phaseList[_nphase].m_animEnd = _v; }
        ITF_INLINE i32      getPhaseAnimEnd(    u32 _nphase	) const { return m_phaseList[_nphase].m_animEnd; }

        ITF_INLINE void         setPhaseSizeMin( u32 _nphase, Vec2d _sizemin ) { m_phaseList[_nphase].m_sizeMin = _sizemin; }
        ITF_INLINE const Vec2d& getPhaseSizeMin(	u32 _nphase)const  { return m_phaseList[_nphase].m_sizeMin; }

        ITF_INLINE void         setPhaseSizeMax( u32 _nphase, Vec2d _sizemax ) { m_phaseList[_nphase].m_sizeMax = _sizemax; }
        ITF_INLINE const Vec2d& getPhaseSizeMax(	u32 _nphase) const { return m_phaseList[_nphase].m_sizeMax; }

        ITF_INLINE void         setPhaseColorMin( u32 _nphase, const Color& _colormin ) { m_phaseList[_nphase].m_colorMin = _colormin; }
        ITF_INLINE const Color& getPhaseColorMin( u32 _nphase) const { return m_phaseList[_nphase].m_colorMin; }

        ITF_INLINE void         setPhaseColorMax( u32 _nphase, const Color& _colormax ) { m_phaseList[_nphase].m_colorMax = _colormax; }
        ITF_INLINE const Color& getPhaseColorMax( u32 _nphase) const { return m_phaseList[_nphase].m_colorMax; }

        ITF_INLINE void         setPhase(u32 _index, ParPhase & _phase)
        {
            setPhaseTime(_index,_phase.m_phaseTime);
            setPhaseSizeMin(_index,_phase.m_sizeMin);
            setPhaseSizeMax(_index,_phase.m_sizeMax);
            setPhaseColorMin(_index,_phase.m_colorMin);
            setPhaseColorMax(_index,_phase.m_colorMax);
        }

        ITF_INLINE void     setLifeTimeMultiplier( f32 _mult ) { m_lifeTimeMultiplier = (_mult >= 0.0f) ? _mult : 1.0f ;}
        ITF_INLINE f32      getLifeTimeMultiplier() const { return m_lifeTimeMultiplier;}

        ITF_INLINE bbool    getRandomizeDirection() const { return m_randomizeDirection; }
        ITF_INLINE void     setRandomizeDirection(bbool _val) { m_randomizeDirection = _val; }
        
        void                setFlip(bbool _flip);
        ITF_INLINE bbool    isFlip()const  {return m_flipped;}

        ITF_INLINE void   setCanFlipAccel(bbool _opt) {m_canFlipAccel = _opt;}
        ITF_INLINE bbool  canFlipAccel()const  {return m_canFlipAccel;}

        ITF_INLINE void   setCanFlipAngleOffset(bbool _opt) {m_canFlipAngleOffset = _opt;}
        ITF_INLINE bbool  canFlipAngleOffset()const  {return m_canFlipAngleOffset;}

        ITF_INLINE void   setCanFlipInitAngle(bbool _opt) {m_canFlipInitAngle = _opt;}
        ITF_INLINE bbool  canFlipInitAngle()const  {return m_canFlipInitAngle;}

        ITF_INLINE void   setCanFlipAngularSpeed(bbool _opt) {m_canFlipAngularSpeed = _opt;}
        ITF_INLINE bbool  canFlipAngularSpeed()const  {return m_canFlipAngularSpeed;}

        ITF_INLINE void   setCanFlipPivot(bbool _opt) {m_canFlipPivot = _opt;}
        ITF_INLINE bbool  canFlipPivot()const  {return m_canFlipPivot;}

        ITF_INLINE void   setCanFlipUV(bbool _opt) {m_canFlipUV = _opt;}
        ITF_INLINE bbool  canFlipUV()const  {return m_canFlipUV;}

        ITF_INLINE void   setCanFlipAngleMin(bbool _opt) {m_canFlipAngleMin = _opt;}
        ITF_INLINE bbool  canFlipAngleMin()const  {return m_canFlipAngleMin;}

        ITF_INLINE void   setCanFlipAngleMax(bbool _opt) {m_canFlipAngleMax = _opt;}
        ITF_INLINE bbool  canFlipAngleMax()const  {return m_canFlipAngleMax;}

        ITF_INLINE void   setCanFlipBoundingBox(bbool _opt) {m_canFlipBoundingBox = _opt;}
        ITF_INLINE bbool  canFlipBoundingBox()const  {return m_canFlipBoundingBox;}

        ITF_INLINE void   setUseMatrix(bbool _opt) {m_useMatrix = _opt;}
        ITF_INLINE bbool  isUseMatrix()const  {return m_useMatrix;}

        ITF_INLINE void   setUseActorTranslation(bbool _opt) {m_useActorTranslation = _opt;}
        ITF_INLINE bbool  useActorTranslation()const  {return m_useActorTranslation;}

        ITF_INLINE void   setCanFlipOrientDir(bbool _opt) {m_canFlipOrientDir = _opt;}
        ITF_INLINE bbool  CanFlipOrientDir()const  {return m_canFlipOrientDir;}

        ITF_INLINE const Vec2d & getActorTranslationOffset() const { return m_actorTranslationOffset; }

    public:   
        Color           m_defaultColor;
        u32				m_maxParticles;
        u32				m_emitParticlesCount;

        Vec3d			m_pos;				// position.
        Vec2d           m_pivot;			// pivot.
        f32 			m_velNorm;  		// velocity.
        Vec3d			m_vel;				// velocity.
        Vec3d			m_grav;				// gravitation.
        Vec3d			m_acc;				// wind/acceleration.
        f32				m_velocityVar;      // velocity variations
        f32				m_friction;

        f32             m_depth;            // depth
        bbool           m_useZAsDepth;

        f32				m_freq;

        f32				m_emitInterval;

        Angle   		m_initAngle;
        Angle		    m_angleDelta;

        Angle	     	m_angularSpeed;
        Angle   		m_angularSpeedDelta;
        f32             m_timeTarget;

        u32             m_nbPhase;

        f32             m_renderPrio;

        // Internal modifier
        f32             m_lifeTimeMultiplier;

        /// Generator.
        GFX_BLENDMODE       m_blendMode;
        ResourceID		    m_texture;

        PARGEN_GEN_TYPE     m_genGenType;
        PARGEN_MODE         m_genMode;

        ITF_VECTOR<ParPhase> m_phaseList;

        /// params for gen Type:
        /// circle:
        f32                 m_circleRadius;
        f32                 m_innerCircleRadius;
        Vec3d               m_scaleShape;
        Vec3d               m_rotateShape;
        /// rectangle:
        AABB                m_genBox;

        /// options:
        u32                 m_orientDir;
        UVMODE              m_UVmode;
        bbool               m_uniformScale;
        bbool               m_randomizeDirection;

        /// cartoon:
        bbool               m_useCartoon;
        f32                 m_outlinesize;
        u32                 m_randomoutline;

        /// impostor
        bbool               m_useImpostor;
        bbool               m_showImpostorRender;
        u32                 m_impostorTextureSizeX;
        u32                 m_impostorTextureSizeY;

        ///
        bbool               m_textureMirrorU;
        bbool               m_textureMirrorV;

        Angle               m_genAngMin;
        Angle               m_genAngMax;

        bbool               m_canFlipAngleOffset;
        bbool               m_canFlipInitAngle;
        bbool               m_canFlipAngularSpeed;
        bbool               m_canFlipPivot;
        bbool               m_canFlipUV;
        bbool               m_canFlipAngleMin;
        bbool               m_canFlipAngleMax;
        bbool               m_canFlipAccel;
        bbool               m_canFlipBoundingBox;
        bbool               m_canFlipOrientDir;
        bbool               m_disableLight;
        bbool               m_useMatrix;
        bbool               m_usePhasesColorAndSize;
        bbool               m_useActorTranslation;
        Vec2d               m_actorTranslationOffset;
        
        /// splitmode.
        u32                 m_nSplit;
        Angle               m_splitDelta;

    private:
        bbool               m_flipped;
    };

} // namespace ITF

#endif // _ITF_PARTICULEGENERATORPARAMETERS_H_
