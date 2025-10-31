#include "precompiled_engine.h"

#ifndef _ITF_PARTICULEGENERATOR_H_
#include "engine/display/particle/ParticuleGenerator.h"
#endif //_ITF_PARTICULEGENERATOR_H_

#ifndef _ITF_PARTICULEGENERATORPARAMETERS_H_
#include "engine/display/particle/ParticuleGeneratorParameters.h"
#endif //_ITF_PARTICULEGENERATORPARAMETERS_H_

namespace ITF
{
    static RandomLCG randomLCG;

    BEGIN_SERIALIZATION(ParticleGeneratorParameters)

        SERIALIZE_MEMBER("maxParticles",m_maxParticles);
        SERIALIZE_MEMBER("defaultColor",m_defaultColor);
        SERIALIZE_MEMBER("emitParticlesCount",m_emitParticlesCount);
        SERIALIZE_MEMBER("pos",m_pos);
        SERIALIZE_MEMBER("pivot",m_pivot);
        SERIALIZE_MEMBER("velNorm",m_velNorm);
        SERIALIZE_MEMBER("vel",m_vel);
        SERIALIZE_MEMBER("grav",m_grav);
        SERIALIZE_MEMBER("acc",m_acc);
        SERIALIZE_MEMBER("depth",m_depth);
        SERIALIZE_MEMBER("useZAsDepth",m_useZAsDepth);
        SERIALIZE_MEMBER("velocityVar",m_velocityVar);
        SERIALIZE_MEMBER("friction",m_friction);
        SERIALIZE_MEMBER("freq",m_freq);
        SERIALIZE_MEMBER("emitInterval",m_emitInterval);
        SERIALIZE_MEMBER("initAngle",m_initAngle);
        SERIALIZE_MEMBER("angleDelta",m_angleDelta);
        SERIALIZE_MEMBER("angularSpeed",m_angularSpeed);
        SERIALIZE_MEMBER("angularSpeedDelta",m_angularSpeedDelta);
        SERIALIZE_MEMBER("timeTarget",m_timeTarget);
        SERIALIZE_MEMBER("nbPhase",m_nbPhase);
        SERIALIZE_MEMBER("renderPrio",m_renderPrio);
        SERIALIZE_MEMBER("circleRadius",m_circleRadius);
        SERIALIZE_MEMBER("innerCircleRadius",m_innerCircleRadius);
        SERIALIZE_MEMBER("scaleShape",m_scaleShape);
        SERIALIZE_MEMBER("rotateShape",m_rotateShape);
        SERIALIZE_MEMBER("randomizeDirection",m_randomizeDirection);
        SERIALIZE_OBJECT("genBox",m_genBox);
        SERIALIZE_OBJECT("boundingBox",m_bounding);
        SERIALIZE_MEMBER("orientDir",m_orientDir);
        SERIALIZE_ENUM_BEGIN("UVmode",m_UVmode);
            SERIALIZE_ENUM_VAR(UV_Default);
            SERIALIZE_ENUM_VAR(UV_FlipX);
            SERIALIZE_ENUM_VAR(UV_FlipY);
            SERIALIZE_ENUM_VAR(UV_FlipXY);
            SERIALIZE_ENUM_VAR(UV_FlipXtoDirX);
            SERIALIZE_ENUM_VAR(UV_FlipYtoDirY);
            SERIALIZE_ENUM_VAR(UV_FlipXYtoDirXY);
        SERIALIZE_ENUM_END();
        SERIALIZE_MEMBER("uniformscale",m_uniformScale);
        SERIALIZE_MEMBER("cartoonrendering",m_useCartoon);
        SERIALIZE_MEMBER("impostorrendering",m_useImpostor);
        SERIALIZE_MEMBER("showimpostorrender",m_showImpostorRender);
        SERIALIZE_MEMBER("impostorTextureSizeX",m_impostorTextureSizeX);
        SERIALIZE_MEMBER("impostorTextureSizeY",m_impostorTextureSizeY);
        SERIALIZE_MEMBER("outlinesize",m_outlinesize);
        SERIALIZE_MEMBER("randomoutline",m_randomoutline);
        SERIALIZE_MEMBER("texturemirroru",m_textureMirrorU);
        SERIALIZE_MEMBER("texturemirrorv",m_textureMirrorV);
        SERIALIZE_MEMBER("genangmin",m_genAngMin);
        SERIALIZE_MEMBER("genangmax",m_genAngMax);

        SERIALIZE_MEMBER("canFlipAngleOffset",m_canFlipAngleOffset);
        SERIALIZE_MEMBER("canFlipInitAngle",m_canFlipInitAngle);
        SERIALIZE_MEMBER("canFlipAngularSpeed",m_canFlipAngularSpeed);
        SERIALIZE_MEMBER("canFlipPivot",m_canFlipPivot);
        SERIALIZE_MEMBER("canFlipUV",m_canFlipUV);
        SERIALIZE_MEMBER("canFlipAngleMin",m_canFlipAngleMin);
        SERIALIZE_MEMBER("canFlipAngleMax",m_canFlipAngleMax);
        SERIALIZE_MEMBER("canFlipAccel",m_canFlipAccel);
        SERIALIZE_MEMBER("canFlipOrientDir",m_canFlipOrientDir);
        SERIALIZE_MEMBER("numberSplit",m_nSplit);
        SERIALIZE_MEMBER("splitDelta",m_splitDelta);
        SERIALIZE_MEMBER("useMatrix",m_useMatrix);
        SERIALIZE_MEMBER("usePhasesColorAndSize", m_usePhasesColorAndSize);
        SERIALIZE_MEMBER("useActorTranslation", m_useActorTranslation);
        SERIALIZE_MEMBER("actorTranslationOffset", m_actorTranslationOffset);
        SERIALIZE_MEMBER("disableLight", m_disableLight);
 
        SERIALIZE_CONTAINER_OBJECT("phases",m_phaseList);

        SERIALIZE_ENUM_BEGIN("blendMode",m_blendMode);
            SERIALIZE_ENUM_VAR(GFX_BLEND_UNKNOWN);
            SERIALIZE_ENUM_VAR(GFX_BLEND_COPY);
            SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHA);
            SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHAPREMULT);
            SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHADEST);
            SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHADESTPREMULT);
            SERIALIZE_ENUM_VAR(GFX_BLEND_ADD);
            SERIALIZE_ENUM_VAR(GFX_BLEND_ADDALPHA);
            SERIALIZE_ENUM_VAR(GFX_BLEND_SUBALPHA);
            SERIALIZE_ENUM_VAR(GFX_BLEND_SUB);
            SERIALIZE_ENUM_VAR(GFX_BLEND_MUL);
            SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHAMUL);
            SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHAMUL);
            SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHA);
            SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHAPREMULT);
            SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHADEST);
            SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHADESTPREMULT);
            SERIALIZE_ENUM_VAR(GFX_BLEND_MUL2X);
            SERIALIZE_ENUM_VAR(GFX_BLEND_NUMBER);
        SERIALIZE_ENUM_END();

        SERIALIZE_ENUM_BEGIN("genGenType",m_genGenType);
            SERIALIZE_ENUM_VAR(PARGEN_GEN_POINTS );
            SERIALIZE_ENUM_VAR(PARGEN_GEN_RECTANGLE);
            SERIALIZE_ENUM_VAR(PARGEN_GEN_CIRCLE);
        SERIALIZE_ENUM_END();

        SERIALIZE_ENUM_BEGIN("genMode",m_genMode);
            SERIALIZE_ENUM_VAR(PARGEN_MODE_FOLLOW );
            SERIALIZE_ENUM_VAR(PARGEN_MODE_COMPLEX);
        SERIALIZE_ENUM_END();

    END_SERIALIZATION()

    BEGIN_SERIALIZATION(ParPhase)

        SERIALIZE_MEMBER("phaseTime",m_phaseTime);
        SERIALIZE_MEMBER("colorMin",m_colorMin);
        SERIALIZE_MEMBER("colorMax",m_colorMax);
        SERIALIZE_MEMBER("sizeMin",m_sizeMin);
        SERIALIZE_MEMBER("sizeMax",m_sizeMax);
        SERIALIZE_MEMBER("animstart",m_animStart);
        SERIALIZE_MEMBER("animend",m_animEnd);
        SERIALIZE_MEMBER("deltaphasetime",m_deltaphasetime);
        SERIALIZE_MEMBER("animstretchtime",m_animstretchtime);
        SERIALIZE_MEMBER("blendtonextphase",m_blendToNextPhase);
   
    END_SERIALIZATION()

    ParticleGeneratorParameters::ParticleGeneratorParameters()
    {
        m_maxParticles			= 0;
        m_emitParticlesCount	= 1;
        m_emitInterval		    = 1.0f;
        m_defaultColor          = Color::white();

        m_pos					= Vec3d::Zero;
        m_pivot                 = Vec2d::Zero;
        m_velNorm               = 1.f;
        m_vel			        = Vec3d::Zero;
        m_grav					= Vec3d::Zero;
        m_acc					= Vec3d::Zero;
        m_velocityVar			= 1.0f;

        m_lifeTimeMultiplier    = 1.0f;

        m_useZAsDepth           = btrue;
        m_depth                 = 0.f;

        m_freq					= 0.05f;
        m_blendMode				= GFX_BLEND_ALPHA;

        m_renderPrio            = 0.f;

        m_nbPhase               = 1;

        m_circleRadius          = 1.f;
        m_innerCircleRadius     = 0.f;
        m_scaleShape            = Vec3d::One;
        m_rotateShape           = Vec3d::Zero;

        m_orientDir             = 0;
        m_UVmode                = UV_Default;
        m_uniformScale          = bfalse;

        m_randomizeDirection    = btrue;
        m_useCartoon            = bfalse;
        m_outlinesize           = 0.09f;
        m_randomoutline         = 5;

        m_useImpostor           = bfalse;
        m_showImpostorRender    = bfalse;

        m_impostorTextureSizeX  = 128;
        m_impostorTextureSizeY  = 128;
        m_texture.invalidateResourceId();

        m_friction = 1.f;
        m_timeTarget = 0.f;
        m_genMode = PARGEN_MODE_COMPLEX;
        m_genGenType = PARGEN_GEN_POINTS;

        m_textureMirrorU = bfalse;
        m_textureMirrorV = bfalse;

        m_genAngMin = Angle(btrue, -360);
        m_genAngMax = Angle(btrue, 360);

        m_flipped   = bfalse;

        m_canFlipAngleOffset = btrue;
        m_canFlipInitAngle = btrue;
        m_canFlipAngularSpeed = btrue;
        m_canFlipPivot = btrue;
        m_canFlipUV = btrue;
        m_canFlipAngleMin = btrue;
        m_canFlipAngleMax = btrue;
        m_canFlipAccel = btrue;
        m_canFlipBoundingBox = btrue;
        m_canFlipOrientDir = bfalse;
        m_useMatrix = bfalse;
        m_useActorTranslation = bfalse;
        m_actorTranslationOffset = Vec2d::Zero;

        m_disableLight = bfalse;

        m_usePhasesColorAndSize = btrue;

        m_nSplit = 0;
        m_splitDelta = Angle(btrue, 0);
    }


    ///----------------------------------------------------------------------------//

    void ParticleGeneratorParameters::setNumberPhase( u32 _nphase )
    {
        m_nbPhase = _nphase;

        m_phaseList.clear();
        m_phaseList.reserve(m_nbPhase);

        ParPhase newphase;

        for (u32 i=0;i< m_nbPhase;i++)
            m_phaseList.push_back(newphase);
    }

    ///----------------------------------------------------------------------------//
    
    f32 ParticleGeneratorParameters::computeSumPhaseTime() const
    {
       f32 phaseSumTime = 0.0f;
       for (ITF_VECTOR<ParPhase>::const_iterator iter = m_phaseList.begin();iter!=m_phaseList.end();++iter)
           phaseSumTime+=(*iter).m_phaseTime;

       return phaseSumTime;
    }
    
    ///----------------------------------------------------------------------------//
    
    void ParticleGeneratorParameters::addPhase( )
    {
        m_nbPhase++;

        ParPhase newphase;
        m_phaseList.push_back(newphase);
    }


    ///----------------------------------------------------------------------------//
    
    void setParticleFromPhaseInfo(ITF_Particle* _par, ParPhase* _phase, f32 _lifeTimeMult)
    {
        if (_phase->m_blendToNextPhase)
            _par->m_flag |= PAR_F_BLENDPHASE;
        else
            _par->m_flag &= ~PAR_F_BLENDPHASE;

        // if phase have anim information
        if (_phase->m_animstretchtime)
            _par->m_flag |= PAR_F_ANIM_STRETCHTIME;
        else
            _par->m_flag &= ~PAR_F_ANIM_STRETCHTIME;

        if (_phase->m_animStart >= 0)
        {
            _par->m_startAnimIndex = _phase->m_animStart;
            _par->m_endAnimIndex = _phase->m_animEnd;
        }    

        _par->m_phaselifeTime = _phase->m_phaseTime + randomLCG.getRandF(-_phase->m_deltaphasetime, _phase->m_deltaphasetime);
        _par->m_phaselifeTime *= _lifeTimeMult;
    }

    ///----------------------------------------------------------------------------//

    void ParticleGeneratorParameters::getDstBlend(ITF_Particle* _par, ITF_ParticleGenerator* _gen)
    {
        /// Set blend with next phase in case there are an another one after.
        u32 nextPhase = _par->m_curPhase + 1;
        if (nextPhase >= m_nbPhase && _par->m_flag & PAR_F_LOOP)
            nextPhase = 0;

        if (nextPhase < m_nbPhase)
        {
            ParPhase* phasenext = &m_phaseList[nextPhase];

            Color randomColor = Color::colorBlend(&phasenext->m_colorMin, &phasenext->m_colorMax,randomLCG.getRandF(0.f, 1.f));
            _par->m_dstColor = _gen->applyGlobalColor(randomColor).getAsU32();

            _par->m_dstSize.m_x = randomLCG.getRandF(phasenext->m_sizeMin.m_x, phasenext->m_sizeMax.m_x);
           
            if ( _par->m_flag & PAR_F_UNIFORMSCALE )
                _par->m_dstSize.m_y = _par->m_dstSize.m_x;
            else
                _par->m_dstSize.m_y = randomLCG.getRandF(phasenext->m_sizeMin.m_y, phasenext->m_sizeMax.m_y);
        }
        /// in case that its the last phase and blending mode is on, then blend with color 0 and size 0
        else if ( _par->m_flag & PAR_F_BLENDPHASE)
        {
            Color zero(0.f,0.f,0.f,0.f);
            _par->m_dstColor = _gen->applyGlobalColor(zero).getAsU32();
            _par->m_dstSize = Vec2d::Zero;
        }
        /// else, keep the size and color.
        else
        {
            _par->m_dstColor = _par->m_initColor;
            _par->m_dstSize = _par->m_initSize;
        }    
    }

    ///----------------------------------------------------------------------------//

    void ParticleGeneratorParameters::initFromPhase(ITF_Particle* _par, f32 _time, ITF_ParticleGenerator* _owner)
    {
        _par->m_initTime = _time;
        ParPhase* phase = &m_phaseList[_par->m_curPhase];
        setParticleFromPhaseInfo(_par, phase, m_lifeTimeMultiplier);

        //if particle uses phases color and size data (bfalse for a color and size manual update)
        if( m_usePhasesColorAndSize )
        {
            _par->m_initSize.m_x = randomLCG.getRandF(phase->m_sizeMin.m_x, phase->m_sizeMax.m_x);
        
            if ( _par->m_flag & PAR_F_UNIFORMSCALE )
                _par->m_initSize.m_y = _par->m_initSize.m_x;
            else
                _par->m_initSize.m_y = randomLCG.getRandF(phase->m_sizeMin.m_y, phase->m_sizeMax.m_y);

            Color randomColor = Color::Interpolate(phase->m_colorMin, phase->m_colorMax, randomLCG.getRandF(0.f, 1.f));
            _par->m_initColor = _owner->applyGlobalColor(randomColor).getAsU32();

            getDstBlend(_par, _owner);

            _par->m_curColor = _par->m_initColor;
            _par->m_curSize = _par->m_initSize;
        }
    }

    ///----------------------------------------------------------------------------//

    void ParticleGeneratorParameters::initToNextPhase(ITF_Particle* _par, f32 _time, ITF_ParticleGenerator* _owner)
    {
        _par->m_initTime = _time;        
        ParPhase* phase = &m_phaseList[_par->m_curPhase];
        setParticleFromPhaseInfo(_par, phase, m_lifeTimeMultiplier);

        //if particle uses phases color and size data (bfalse for a color and size manual update)
        if( m_usePhasesColorAndSize )
        {
            //get color and scale from previous phase.
            _par->m_initSize = _par->m_dstSize;
            _par->m_initColor = _par->m_dstColor;
        
            getDstBlend(_par, _owner);

            _par->m_curColor = _par->m_initColor;
            _par->m_curSize = _par->m_initSize;
        }
    }

    void ParticleGeneratorParameters::setFlip( bbool _flip )
    {
        if (_flip == m_flipped)
            return;

        m_flipped = _flip;

        AABB    tmpAABB;
        Vec3d   acc = getAcc();
        
        if (canFlipAccel())
        {
            acc.m_x *= -1;
            setAcc(acc);
        }

        if (canFlipAngularSpeed())
            setAngularSpeed(-getAngularSpeed());

        //setAngleDelta(-getAngleDelta());
        //setAngularSpeedDelta(-getAngularSpeedDelta());
        if (canFlipInitAngle())
            m_initAngle = -m_initAngle;

        if (canFlipPivot())
        {
            Vec2d pivot = getPivot();
            pivot.m_x *= -1;
            setPivot(pivot);
        }

        getGenBox().FlipHorizontaly(bfalse);
    
        if (canFlipBoundingBox())
        {
            getBoundingBox().FlipHorizontaly(bfalse);
        }

        Angle maxAng = getGenAngMax();
        Angle minAng = getGenAngMin();
 
        if (canFlipAngleMin())
            setGenAngMin(Angle(btrue, -maxAng.ToDegrees()));

        if (canFlipAngleMin())
            setGenAngMax(Angle(btrue, -minAng.ToDegrees()));

        if (canFlipUV())
        {
            UVMODE uvMode = getUVmode();
            switch (uvMode)
            {
            case UV_Default:        uvMode = UV_FlipX;      break;
            case UV_FlipX:          uvMode = UV_Default;    break;
            case UV_FlipY:          uvMode = UV_FlipXY;     break;
            case UV_FlipXY:         uvMode = UV_FlipY;      break;
            case UV_FlipXtoDirX:    break; 
            case UV_FlipYtoDirY:    break;
            case UV_FlipXYtoDirXY:  break;
            }
            setUVmode(uvMode);
        }
    }
} // namespace ITF


