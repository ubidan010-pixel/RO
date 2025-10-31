#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_FLUIDFALLAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_FluidFallAIComponent.h"
#endif //_ITF_RAY_FLUIDFALLAICOMPONENT_H_

#ifndef _ITF_CURVECOMPONENT_H_
#include "gameplay/Components/Misc/CurveComponent.h"
#endif //_ITF_CURVECOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#define STATE_CLOSED                    0
#define STATE_ANNUNCIATION              1
#define STATE_OPENING                   2
#define STATE_OPEN                      3
#define STATE_CLOSING                   4

#define STATE_GLOBAL_OFF                0
#define STATE_GLOBAL_ON                 1

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_FluidFallAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_FluidFallAIComponent)
        SERIALIZE_UVMODE("uvMode",m_uvMode);
        
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("startOpen",   m_startOpen);
            SERIALIZE_MEMBER("widthStart",  m_widthStart);
            SERIALIZE_MEMBER("widthBase",   m_widthBase);
            SERIALIZE_MEMBER("speed",       m_speed);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_FluidFallAIComponent)
        VALIDATE_COMPONENT_PARAM("", m_curveComponent,  "Ray_FluidFallAIComponent requires an CurveComponent.");
    END_VALIDATE_COMPONENT()

    //*****************************************************************************

    Ray_FluidFallAIComponent::Ray_FluidFallAIComponent()
    : m_curveComponent(NULL)
    , m_fxControllerComponent(NULL)
    , m_stateGlobal(STATE_GLOBAL_ON)
    , m_startOpen(btrue)
    , m_widthStart(2.0f)
    , m_widthBase(5.0f)
    , m_speed(10.0f)
    , m_uvMode(UV_MODE_OPTIMUM)
    , m_registered(bfalse)
    , m_curIndexAtlas(0)
    , m_countTrame(0)
    , m_useAtlas(bfalse)
    , m_countAtlasUv(0)
    {
        // none
    }

    //*****************************************************************************

    Ray_FluidFallAIComponent::~Ray_FluidFallAIComponent()
    {
    }

    //*****************************************************************************

    void Ray_FluidFallAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Register Events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventGeneric,4063838687), this);

        // Get components
        m_curveComponent = m_actor->GetComponent<CurveComponent>(); // Mandatory
        m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();

        ITF_ASSERT(m_curveComponent);

        // Init resources
        if ( getTemplate()->getGameMaterial().getStringID() != GameMaterial_Template::nullMatName.getStringID() )
        {
            World::addGameMaterialFromFile(getTemplate()->getGameMaterial(), m_actor->getResourceGroup());
        }

        if (!getTemplate()->getTextureFile().isEmpty())
        {
            m_texture = m_actor->addResource(Resource::ResourceType_Texture, getTemplate()->getTextureFile());

            // Atlas or not Atlas ?
            if (m_texture.isValidResourceId())
            {
                Texture * texture = static_cast<Texture*>(m_texture.getResource());

                if(texture)
                {
                    const UVAtlas* uvAtlas = texture->getUVAtlas();

                    if (uvAtlas)
                    {
                        m_useAtlas = btrue;
                        m_countAtlasUv = uvAtlas->getNumberUV();
                    }
                }
            }           
            
        }


        // Init Compo
        init();
 
    }

    //*****************************************************************************

    void Ray_FluidFallAIComponent::init(bbool _isReinit /*=bfalse*/)
    {
        // Global
        if (m_startOpen)
        {
            forceOpen(m_startOpen);
        }
        else
        {
            m_stateGlobal = STATE_GLOBAL_OFF;
        }
    }

    //*****************************************************************************

    void Ray_FluidFallAIComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event);

        if(EventTrigger* trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
        {
            if (!getTemplate()->getIgnoreEventTrigger())
            {
                if (m_stateGlobal == STATE_GLOBAL_OFF)
                {
                    receiveEvent(btrue);
                }
                else
                {
                    receiveEvent(bfalse);
                }
            }
        }
        else if ( EventGeneric* eventGeneric = _event->DynamicCast<EventGeneric>(ITF_GET_STRINGID_CRC(EventGeneric,4063838687)) )
        {
            const StringID s_Open = ITF_GET_STRINGID_CRC(Open,812432275);
            const StringID s_ForceOpen = ITF_GET_STRINGID_CRC(ForceOpen,410328037);
            const StringID s_Close = ITF_GET_STRINGID_CRC(Close,3883599349);

            if (eventGeneric->getId() == s_Open)
            {
               receiveEvent(btrue);
            }
            else if (eventGeneric->getId() == s_Close)
            {
                receiveEvent(bfalse);
            }
            else if (eventGeneric->getId() == s_ForceOpen)
            {
                forceOpen(btrue);
            }
        }



        if (getTemplate()->getUsePolyline())
        {
            for(u32 i = 0; i < m_listFluidProgress.size(); i++)
            {
                FluidFallProgress& ffp = *m_listFluidProgress[i];
                ffp.m_polyline.onEvent(_event);
            }

        }
    }

    //*****************************************************************************

    void Ray_FluidFallAIComponent::receiveEvent(bbool _open)
    {
        if (_open && m_stateGlobal == STATE_GLOBAL_OFF)
        {
            m_listFluidProgress.push_back(newAlloc(mId_Gameplay,FluidFallProgress()));
            FluidFallProgress& ffp = *m_listFluidProgress.back();
            ffp.init(m_curveComponent, bfalse, m_widthStart, m_widthBase);
            if (getTemplate()->getUsePolyline())
            {
                ffp.m_polyline.init(m_actor,this);
                ffp.m_polyline.setGameMaterial(getTemplate()->getGameMaterial());
            }
            
            if(getTemplate()->getUseAnnunciatorFx())
            {
                startAnnonce(ffp);
            }
            else
            {
                open(ffp);
            }
            

            m_stateGlobal = STATE_GLOBAL_ON;
        }
        else if(!_open && m_stateGlobal == STATE_GLOBAL_ON)
        {
            for(u32 i = 0; i < m_listFluidProgress.size(); i++)
            {
                FluidFallProgress& ffp =* m_listFluidProgress[i];

                switch (ffp.m_state)
                {
                case STATE_OPEN:
                case STATE_OPENING:
                case STATE_ANNUNCIATION:
                    close(ffp);
                    break;
                }

            }

            m_stateGlobal = STATE_GLOBAL_OFF;
        }
    }

    //*****************************************************************************

    void Ray_FluidFallAIComponent::startAnnonce(FluidFallProgress& _ffp)
    {
        _ffp.m_state = STATE_ANNUNCIATION;
        _ffp.m_timeAnnunciationFx = getTemplate()->getDurationAnnunciationFx();
        _ffp.m_deployCurDistAnnunFX = _ffp.m_deployCurDist;

        // Start fx annunciator
        StringID fxNameAnnunciator = getTemplate()->getFxNameAnnunciator();
        if (fxNameAnnunciator.isValid() && _ffp.m_handleAnnunciatorFx == U32_INVALID)
        {
            _ffp.m_handleAnnunciatorFx = playFx(fxNameAnnunciator, m_actor->getPos());
        }

        // Start sound annunciator
        StringID soundNameAnnunciator = getTemplate()->getSoundNameAnnunciatorFFP();
        if (soundNameAnnunciator.isValid() && _ffp.m_handleAnnunciatorSound == U32_INVALID)
        {
            _ffp.m_handleAnnunciatorSound = playFx(soundNameAnnunciator, m_actor->getPos());
        }

    }

    //*****************************************************************************

    void Ray_FluidFallAIComponent::open(FluidFallProgress& _ffp)
    {
        _ffp.m_state = STATE_OPENING;
        _ffp.m_deployCurDistFluid = _ffp.m_deployCurDist;

        // Start : Fx start
        StringID fxNameStart = getTemplate()->getFxNameStartFFP();
        if (fxNameStart.isValid() && _ffp.m_handleStartFx == U32_INVALID)
        {
            _ffp.m_handleStartFx = playFx(fxNameStart, m_actor->getPos());
        }

        // Start : Sound start
        StringID soundNameStart = getTemplate()->getSoundNameStartFFP();
        if (soundNameStart.isValid() && _ffp.m_handleStartSound == U32_INVALID)
        {
            _ffp.m_handleStartSound = playFx(soundNameStart, m_actor->getPos());
        }

    }
    //*****************************************************************************

    void Ray_FluidFallAIComponent::forceOpen(bbool _startOpen)
    {
        m_stateGlobal = STATE_GLOBAL_ON;
        m_listFluidProgress.push_back(newAlloc(mId_Gameplay, FluidFallProgress()));
        FluidFallProgress* ffp = m_listFluidProgress.back();
        ffp->init(m_curveComponent, _startOpen, m_widthStart, m_widthBase);

        if (getTemplate()->getUsePolyline())
        {
            ffp->m_polyline.init(m_actor,this);
            ffp->m_polyline.setGameMaterial(getTemplate()->getGameMaterial());
        }
    }

    //*****************************************************************************

    void Ray_FluidFallAIComponent::close(FluidFallProgress& _ffp)
    {
        _ffp.m_state = STATE_CLOSING;

        _ffp.m_timer = 0.0f;
        if (_ffp.m_currentTotalLength < getTemplate()->getAlphaLength())
            _ffp.m_timer = (getTemplate()->getAlphaLength() - _ffp.m_currentTotalLength )/ m_speed;

        _ffp.m_timer += getTemplate()->getAlphaLength() / m_speed;
    }

    //*****************************************************************************

    void Ray_FluidFallAIComponent::onBecomeActive()
    {
        Super::onBecomeActive();
        registerObjects();
    }

    //*****************************************************************************

    void Ray_FluidFallAIComponent::Update( f32 _dt ) 
    {
        Super::Update(_dt);

        growAABB();

        // If texture use atlas
        if (m_useAtlas)
        {
            m_countTrame++;

            u32 modTile = getTemplate()->getModTileTexture();

            if (modTile > 0)
            {
                u32 mod = m_countTrame%modTile;

                if (mod == 0)
                {
                    if (m_countAtlasUv > 0)
                    {
                        m_curIndexAtlas++;
                        m_curIndexAtlas = m_curIndexAtlas%m_countAtlasUv;
                    }
                }
            }
        }


        for (ITF_VECTOR<FluidFallProgress*>::iterator it = m_listFluidProgress.begin(); it != m_listFluidProgress.end();)
        {
            FluidFallProgress& ffp = **it;

            updateDeploy(ffp, _dt); 
            updateStim(ffp);
            updatePolyline(ffp, _dt);

            if(ffp.m_state == STATE_CLOSED)
            {
                delete *it;
                it = m_listFluidProgress.erase(it);
            }
            else
            {
                ++it;
            }
        }

    }

    //*****************************************************************************

    void Ray_FluidFallAIComponent::updateDeploy( FluidFallProgress& _ffp, f32 _dt )
    {        

        f32 dist = m_speed * _dt;
        _ffp.m_deployCurDistFluid += dist;


        if (_ffp.m_state == STATE_ANNUNCIATION)
        {
            _ffp.m_deployCurDistAnnunFX += dist;
            _ffp.m_timeAnnunciationFx -= _dt;

            if ( _ffp.m_timeAnnunciationFx <= 0.0f ||
                _ffp.m_deployCurDistAnnunFX > m_curveComponent->getDistCurveTotal())
            {
                open(_ffp);
            }
        }
        else if (_ffp.m_state == STATE_OPENING)
        {
            _ffp.m_deployCurDist += dist;
            _ffp.m_deployCurDistAnnunFX += dist;
            _ffp.m_patchClosing = bfalse;

            if (getTemplate()->getUseAnnunciatorFx() && _ffp.m_deployCurDistAnnunFX > m_curveComponent->getDistCurveTotal())
            {
                // Stop annunciator FX
                if (_ffp.m_handleAnnunciatorFx != U32_INVALID)
                {
                    stopFX(_ffp.m_handleAnnunciatorFx);
                    _ffp.m_handleAnnunciatorFx = U32_INVALID;
                }

                // Stop annunciator Sound
                if (_ffp.m_handleAnnunciatorSound != U32_INVALID)
                {
                    stopFX(_ffp.m_handleAnnunciatorSound);
                    _ffp.m_handleAnnunciatorSound = U32_INVALID;
                }

                
            }

            if (_ffp.m_deployCurDist > m_curveComponent->getDistCurveTotal())
            {
                _ffp.m_deployCurDist = m_curveComponent->getDistCurveTotal();
                _ffp.m_deployCurOrigin = 0.0f;
                _ffp.m_state = STATE_OPEN;

                // Stop : Fx Start
                if (_ffp.m_handleStartFx != U32_INVALID)
                {
                    stopFX(_ffp.m_handleStartFx);
                    _ffp.m_handleStartFx = U32_INVALID;
                }

            }

        }
        else if(_ffp.m_state == STATE_CLOSING)
        {
            _ffp.m_deployCurDist += dist;
            _ffp.m_deployCurDistAnnunFX += dist;

            if (getTemplate()->getUseAnnunciatorFx() && _ffp.m_deployCurDistAnnunFX > m_curveComponent->getDistCurveTotal())
            {
                // Stop annunciator FX
                if (_ffp.m_handleAnnunciatorFx != U32_INVALID)
                {
                    stopFX(_ffp.m_handleAnnunciatorFx);
                    _ffp.m_handleAnnunciatorFx = U32_INVALID;
                }

                // Stop annunciator sound
                if (_ffp.m_handleAnnunciatorSound != U32_INVALID)
                {
                    stopFX(_ffp.m_handleAnnunciatorSound);
                    _ffp.m_handleAnnunciatorSound = U32_INVALID;
                }

            }

            if (_ffp.m_timer == 0.0f)
                _ffp.m_deployCurOrigin += dist;

            _ffp.m_patchClosing = _ffp.m_timer < (getTemplate()->getAlphaLength() /m_speed);

            _ffp.m_timer = Max(0.0f,_ffp.m_timer - _dt);

            if (_ffp.m_deployCurDist > m_curveComponent->getDistCurveTotal())
            {
                _ffp.m_deployCurDist = m_curveComponent->getDistCurveTotal();
            
                // Stop : Fx Start
                if (_ffp.m_handleStartFx != U32_INVALID)
                {
                    stopFX(_ffp.m_handleStartFx);
                    _ffp.m_handleStartFx = U32_INVALID;
                }
            
            }

            if (_ffp.m_deployCurOrigin > m_curveComponent->getDistCurveTotal())
            {
                _ffp.m_deployCurDist = 0.0f;
                _ffp.m_deployCurOrigin = 0.0f;
                _ffp.m_pointsList.clear();
                _ffp.m_state = STATE_CLOSED;

                // Stop : Sound Start
                if (_ffp.m_handleStartSound != U32_INVALID)
                {
                    stopFX(_ffp.m_handleStartSound);
                    _ffp.m_handleStartSound = U32_INVALID;
                }
            }
        }
    }


    //*****************************************************************************

    void Ray_FluidFallAIComponent::growAABB()
    {
        CurveComponent::TrajectoryArray& nodeArray = m_curveComponent->getNodeArrayRef();

        AABB aabb(m_actor->getPos());

        for (u32 i = 0; i < nodeArray.size(); i++)
        {
            CurveComponent::TrajectoryNode& node = nodeArray[i];
            aabb.grow(m_curveComponent->actorTransform(node.m_pos)); 
            aabb.grow(m_curveComponent->actorTransform(node.m_tangA)); 
            aabb.grow(m_curveComponent->actorTransform(node.m_tangB)); 
        } 

        m_actor->growRelativeAABBFromAbsolute(aabb);
    }

    //*****************************************************************************

    void Ray_FluidFallAIComponent::Draw()
    {
        Super::Draw();

        for(u32 i = 0; i < m_listFluidProgress.size(); i++)
        {
            FluidFallProgress& ffp = *m_listFluidProgress[i];

            if (ffp.m_state == STATE_CLOSED)
            {
                return;
            }
            else if (ffp.m_state == STATE_OPEN)
            {
                DrawOpen(ffp);
            }
            else
            {
                DrawDeploy(ffp);        
            }
        }
    }
   
    //*****************************************************************************

    void Ray_FluidFallAIComponent::DrawOpen( FluidFallProgress& _ffp )
    {
        CurveComponent::TrajectoryArray& nodeArray = m_curveComponent->getNodeArrayRef();
        Vec2d actorPos = m_actor->get2DPos();

        // ### Fluid flow
        f32 curTile = -(_ffp.m_deployCurDistFluid / getTemplate()->getTileLength());

        _ffp.m_currentTotalLength = 0.0f;

        ITF_VECTOR<PatchData> paramsArray;
        paramsArray.reserve(_ffp.m_listSegment.size());

        for (u32 i = 0; i < _ffp.m_listSegment.size(); i++) 
        {
            FluidFallProgress::SegmentCurve& segment = _ffp.m_listSegment[i];

            paramsArray.push_back(PatchData());
            PatchData & patchData = paramsArray[i];
            BezierPatchParams &params = patchData.params;
            patchData.index = i;

            CurveComponent::TrajectoryNode& nodeSource  = nodeArray[i];
            CurveComponent::TrajectoryNode& nodeDest    = nodeArray[i+1];

            patchData.length = nodeSource.m_lengthSeg;

            _ffp.m_currentTotalLength += nodeSource.m_lengthSeg;

            params.m_p0 = m_curveComponent->actorTransform(nodeSource.m_pos).truncateTo2D();
            params.m_p1 = m_curveComponent->actorTransform(nodeSource.m_tangB).truncateTo2D();
            params.m_p2 = m_curveComponent->actorTransform(nodeDest.m_tangA).truncateTo2D();
            params.m_p3 = m_curveComponent->actorTransform(nodeDest.m_pos).truncateTo2D();    


            f32 countTileSeg = nodeSource.m_lengthSeg / getTemplate()->getTileLength();
            params.m_startUV.m_x = curTile;
            params.m_endUV.m_x = curTile + countTileSeg;

            // Get UV for Y
            if (m_useAtlas)
            {
                Vec2d uv0, uv1;
                getUVDatabyIndex(m_curIndexAtlas, uv0, uv1);
                params.m_startUV.m_y = uv0.m_y;
                params.m_endUV.m_y = uv1.m_y;
            }

            params.m_startWidth = segment.m_widthStart;
            params.m_endWidth = segment.m_widthEnd;

            params.m_z = m_actor->getDepth() + getTemplate()->getZOffset();
            params.m_texture = (Texture*)m_texture.getResource();
            params.m_uvMode = m_uvMode;

            params.m_startColor = params.m_endColor = getColorFactor();
            params.m_fogColor = getColorFog();

            curTile += countTileSeg; 
        }
        calculateAlphaAndDraw(_ffp, paramsArray,_ffp.m_currentTotalLength);
        if (getTemplate()->getUseStim())
            calculateStimShape(_ffp, paramsArray,_ffp.m_currentTotalLength);
        if (getTemplate()->getUsePolyline())
            calculatePolyline(_ffp, paramsArray,_ffp.m_currentTotalLength);
    }

    //*****************************************************************************

    void Ray_FluidFallAIComponent::getUVDatabyIndex(u32 _index, Vec2d& _uv0, Vec2d& _uv1)
    {
        if (m_texture.isValidResourceId())
        {
            Texture * texture = static_cast<Texture*>(m_texture.getResource());

            if(texture)
            {
                const UVAtlas* uvAtlas = texture->getUVAtlas();

                if (uvAtlas)
                {
                    ITF_ASSERT(_index < m_countAtlasUv);

                    const UVdata& uvData = uvAtlas->getUVDatabyIndex(_index);

                    _uv0 = uvData.getUV0();
                    _uv1 = uvData.getUV1();

                    //params.m_startUV.m_y = uvData.getUV0().m_y;
                    //params.m_endUV.m_y = uvData.getUV1().m_y;

                }
            }
        }
    }

    //*****************************************************************************

    f32 Ray_FluidFallAIComponent::getPatchAlpha(f32 _pos, f32 _totalLength, bbool _isOpening, f32 _startLength)
    {
        const f32 alphaLength = getTemplate()->getAlphaLength();
        if (_isOpening)
        {
            //fade one end only
            return Min((_totalLength - _pos ) / alphaLength,1.0f);
        }
        else
        {
            if (_totalLength  > _startLength)
            {
                if (_pos < _startLength)
                    return (alphaLength - _startLength + _pos)/alphaLength;
                else 
                    return Min((_totalLength - _pos ) / alphaLength,1.0f);
            }
            else
            {
                f32 a = Clamp(_pos/alphaLength,0.0f,1.0f);
                f32 b = Clamp((_totalLength-_pos)/alphaLength,0.f,1.0f);

                return Min(a,b);
            }
        }
    }



    void Ray_FluidFallAIComponent::DrawDeploy( FluidFallProgress& _ffp )
    {
        // ### Deploy fluid

        if (_ffp.m_listSegment.size() == 0)
        {
            return;
        }


        if (getTemplate()->getUseAnnunciatorFx())
        {
            // Annunciator FX position
            Vec3d deployCurAnnunFxPos, deployCurAnnunFxTgt;
            u32 deployCurAnnunFxSegment = 0;  
            f32 AnnunFxK = 0.0f;
            m_curveComponent->getPosAndIndexSegAtDist(_ffp.m_deployCurDistAnnunFX, deployCurAnnunFxPos, deployCurAnnunFxTgt, deployCurAnnunFxSegment, AnnunFxK);
            
            setFxPos(_ffp.m_handleAnnunciatorFx, deployCurAnnunFxPos);
            setFxPos(_ffp.m_handleAnnunciatorSound, deployCurAnnunFxPos);

            if (_ffp.m_state == STATE_ANNUNCIATION)
            {
                // If ffp is in annunciation state, we update only the FX.
                return;
            }
        }

        CurveComponent::TrajectoryArray& nodeArray = m_curveComponent->getNodeArrayRef();
        Vec2d actorPos = m_actor->get2DPos();

        // Head position
        Vec3d deployCurPos, deployCurTgt;
        u32 deployCurSegment = 0;  
        f32 k = 0.0f;
        f32 distAlreadyDeploy = 0.0f;
        m_curveComponent->getPosAndIndexSegAtDist(_ffp.m_deployCurDist, deployCurPos, deployCurTgt, deployCurSegment, k);

        // Tail position
        Vec3d deployCurOriginPos, deployCurOriginTgt;
        u32 deployCurOriginSegment = 0;  
        f32 originK = 0.0f;
        m_curveComponent->getPosAndIndexSegAtDist(_ffp.m_deployCurOrigin, deployCurOriginPos, deployCurOriginTgt, deployCurOriginSegment, originK);

        // Fx Start
        if (_ffp.m_handleStartFx != U32_INVALID)
        {
            setFxPos(_ffp.m_handleStartFx, deployCurPos);
        }

        // Sound Start
        if (_ffp.m_handleStartSound != U32_INVALID)
        {
            setFxPos(_ffp.m_handleStartSound, deployCurPos);
        }


        f32 curTile;

        if (getTemplate()->getDrawUvFromRoot())
        {
            // deploy from root
            curTile = -(_ffp.m_deployCurDistFluid)/ getTemplate()->getTileLength();
        }
        else
        {
            // deploy from head
            curTile = 0.0f;
        }

        ITF_VECTOR<PatchData> paramsArray;
        i32 cnt = deployCurSegment-deployCurOriginSegment+1;
        if(cnt > 0)
            paramsArray.reserve((u32)cnt);


        u32 count = 0;
        f32 totalLength = 0.0f;
        for (u32 i = deployCurOriginSegment; i <= deployCurSegment; i++) 
        {
            paramsArray.push_back(PatchData());
            PatchData & tmpStruct = paramsArray[count];
            count++;
            tmpStruct.index = i;

            FluidFallProgress::SegmentCurve& segment = _ffp.m_listSegment[i];
            BezierPatchParams & params = tmpStruct.params;

            CurveComponent::TrajectoryNode& nodeSource  = nodeArray[i];
            CurveComponent::TrajectoryNode& nodeDest    = nodeArray[i+1];

            const Vec2d p0 = m_curveComponent->actorTransform(nodeSource.m_pos).truncateTo2D();
            const Vec2d p1 = m_curveComponent->actorTransform(nodeSource.m_tangB).truncateTo2D();
            const Vec2d p2 = m_curveComponent->actorTransform(nodeDest.m_tangA).truncateTo2D();
            const Vec2d p3 = m_curveComponent->actorTransform(nodeDest.m_pos).truncateTo2D();    

            Bezier4<Vec2d> bezier(p0,p1,p2,p3);


             params.m_z = m_actor->getDepth() + getTemplate()->getZOffset();
            params.m_texture = (Texture*)m_texture.getResource();
            params.m_uvMode = m_uvMode;

            params.m_startColor = params.m_endColor = getColorFactor();
            params.m_fogColor = getColorFog();


            // Get UV for Y
            if (m_useAtlas)
            {
                Vec2d uv0, uv1;
                getUVDatabyIndex(m_curIndexAtlas, uv0, uv1);
                params.m_startUV.m_y = uv0.m_y;
                params.m_endUV.m_y = uv1.m_y;
            }


            // only one segment, special case
            if (deployCurSegment == deployCurOriginSegment)
            {
                params.m_startUV.m_x =  curTile + _ffp.m_deployCurOrigin / getTemplate()->getTileLength();
                params.m_endUV.m_x =  params.m_startUV.m_x + (_ffp.m_deployCurDist - _ffp.m_deployCurOrigin) / getTemplate()->getTileLength();

                tmpStruct.length = _ffp.m_deployCurDist - _ffp.m_deployCurOrigin;
                totalLength += tmpStruct.length;
                Bezier4<Vec2d> newSegment;
                bezier.getSegment(originK,k,newSegment);

                params.m_p0 = newSegment.m_p0;
                params.m_p1 = newSegment.m_p1;                       
                params.m_p2 = newSegment.m_p2;
                params.m_p3 = newSegment.m_p3; 

                params.m_startWidth = segment.m_widthStart+ ((segment.m_widthEnd-segment.m_widthStart) * originK);
                params.m_endWidth = segment.m_widthStart+ ((segment.m_widthEnd-segment.m_widthStart) * k);

            }
            else
            {
                //current segment is first segment
                if (i == deployCurOriginSegment)
                {
                    params.m_startUV.m_x = curTile + _ffp.m_deployCurOrigin / getTemplate()->getTileLength();
                    f32 length = 0.0f;
                    for (u32 j = 0; j <= i; ++j)
                    {
                        length += nodeArray[j].m_lengthSeg;
                    }
                    const f32 segmentLength = (length - _ffp.m_deployCurOrigin);
                    params.m_endUV.m_x = params.m_startUV.m_x +  segmentLength / getTemplate()->getTileLength();

                    tmpStruct.length = segmentLength;
                    totalLength += tmpStruct.length;

                    Bezier4<Vec2d> left,right;

                    bezier.split(originK,left,right);
                    params.m_p0 = right.m_p0;
                    params.m_p1 = right.m_p1;                       
                    params.m_p2 = right.m_p2;
                    params.m_p3 = right.m_p3; 

                    params.m_startWidth = segment.m_widthStart+ ((segment.m_widthEnd-segment.m_widthStart) * originK);
                    params.m_endWidth = segment.m_widthEnd;

                    distAlreadyDeploy += nodeSource.m_lengthSeg;   

              
                }
                //current segment is last segment
                else if (i == deployCurSegment)
                {
                    f32 length = 0.0f;
                    for (u32 j = 0; j < i; ++j)
                    {
                        length += nodeArray[j].m_lengthSeg;
                    }
                    const f32 segmentLength = (_ffp.m_deployCurDist - length);
                    params.m_startUV.m_x = curTile + length / getTemplate()->getTileLength();
                    params.m_endUV.m_x = params.m_startUV.m_x +  segmentLength / getTemplate()->getTileLength();

                    tmpStruct.length = segmentLength;
                    totalLength+=tmpStruct.length;

                    Bezier4<Vec2d> left,right;

                    bezier.split(k,left,right);
                    params.m_p0 = left.m_p0;
                    params.m_p1 = left.m_p1;                       
                    params.m_p2 = left.m_p2;
                    params.m_p3 = left.m_p3; 

                    params.m_startWidth = segment.m_widthStart;
                    params.m_endWidth = segment.m_widthStart + ((segment.m_widthEnd-segment.m_widthStart) * k);

                }
                //current segment is an intermediate segment
                else
                {
                    //draw the full segment

                    f32 length = 0.0f;
                    for (u32 j = 0; j < i; ++j)
                    {
                        length += nodeArray[j].m_lengthSeg;
                    }
                    params.m_startUV.m_x = curTile + length / getTemplate()->getTileLength();
                    params.m_endUV.m_x = params.m_startUV.m_x + nodeSource.m_lengthSeg / getTemplate()->getTileLength();

                    tmpStruct.length = nodeSource.m_lengthSeg;
                    totalLength+=tmpStruct.length;

                    params.m_p0 = p0;
                    params.m_p1 = p1;
                    params.m_p2 = p2;
                    params.m_p3 = p3;

                    params.m_startWidth = segment.m_widthStart;
                    params.m_endWidth = segment.m_widthEnd;

                    distAlreadyDeploy += nodeSource.m_lengthSeg;   
                }
            }

        }

        calculateAlphaAndDraw(_ffp, paramsArray, totalLength);
        if (getTemplate()->getUseStim())
            calculateStimShape(_ffp, paramsArray, totalLength);
        if (getTemplate()->getUsePolyline())
            calculatePolyline(_ffp, paramsArray,_ffp.m_currentTotalLength);

    }


    //*****************************************************************************

    u32 Ray_FluidFallAIComponent::playFx(StringID _name, Vec3d _pos)
    {
        if (m_fxControllerComponent)
        {
            u32 fxHandle = m_fxControllerComponent->playFX(_name);

            m_fxControllerComponent->setFXPos(fxHandle,_pos); 
            return fxHandle; 
        }

        return U32_INVALID; 
    }

    //*****************************************************************************

    void Ray_FluidFallAIComponent::setFxPos(u32 _handle, Vec3d _pos)
    {
        if (m_fxControllerComponent)
        {
            m_fxControllerComponent->setFXPos(_handle,_pos);
        }
    }

    //*****************************************************************************

    void Ray_FluidFallAIComponent::setFxAngle(u32 _handle, f32 _angle)
    {
        if (m_fxControllerComponent)
        {
            m_fxControllerComponent->setFXAngle(_handle, _angle);
        }
    }

    //*****************************************************************************

    void Ray_FluidFallAIComponent::stopFX(u32 _handle)
    {
        if (m_fxControllerComponent)
        {
            m_fxControllerComponent->stopFX(_handle);
        }
    }


    //*****************************************************************************

    void Ray_FluidFallAIComponent::calculateAlphaAndDraw(FluidFallProgress& _ffp, ITF_VECTOR<PatchData> &paramsArray, f32 totalLength )
    {
        f32 pos = 0.0f;

        const f32 lenAlphaZone = getTemplate()->getAlphaLength();

        _ffp.m_currentTotalLength = 0.0f;
        f32 startLength = lenAlphaZone - Clamp(_ffp.m_timer * m_speed,0.0f,lenAlphaZone); 
        for (u32 i = 0; i < paramsArray.size(); ++i) 
        {

            PatchData & tmpStruct = paramsArray[i];
            _ffp.m_currentTotalLength += tmpStruct.length;

            if (_ffp.m_patchClosing && pos < lenAlphaZone && pos + tmpStruct.length > totalLength - lenAlphaZone)
            {
                //start
                tmpStruct.params.m_alphaValueA = getPatchAlpha(pos,totalLength, !_ffp.m_patchClosing,startLength);

                //interm
                tmpStruct.params.m_alphaLimitA = (lenAlphaZone - pos) / tmpStruct.length;;
                tmpStruct.params.m_startColor.m_a = getPatchAlpha(lenAlphaZone,totalLength,!_ffp.m_patchClosing,startLength);

                tmpStruct.params.m_alphaLimitB = (totalLength - lenAlphaZone - pos) / tmpStruct.length;

                tmpStruct.params.m_endColor.m_a = getPatchAlpha(lenAlphaZone,totalLength,!_ffp.m_patchClosing,startLength);


                if (tmpStruct.params.m_startColor.m_a == tmpStruct.params.m_endColor.m_a
                    && tmpStruct.params.m_alphaLimitB < tmpStruct.params.m_alphaLimitA)
                {
                    tmpStruct.params.m_alphaLimitA = tmpStruct.params.m_alphaLimitB;
                }

                pos += tmpStruct.length;

                //end
                tmpStruct.params.m_alphaValueB   = getPatchAlpha(pos,totalLength,!_ffp.m_patchClosing,startLength);
            }
            else if (pos  < totalLength - lenAlphaZone && pos + tmpStruct.length  > totalLength - lenAlphaZone)
            {
                //start
                tmpStruct.params.m_alphaLimitA = 0.0f;
                f32 alphaStart = getPatchAlpha(pos,totalLength, !_ffp.m_patchClosing,startLength);
                tmpStruct.params.m_alphaValueA = alphaStart;
                tmpStruct.params.m_startColor.m_a = alphaStart;

                //interm
                tmpStruct.params.m_alphaLimitB = (totalLength - lenAlphaZone - pos) / tmpStruct.length;
                tmpStruct.params.m_endColor.m_a = getPatchAlpha(totalLength - lenAlphaZone,totalLength,!_ffp.m_patchClosing,startLength);

                pos += tmpStruct.length;

                //end
                tmpStruct.params.m_alphaValueB   = getPatchAlpha(pos,totalLength,!_ffp.m_patchClosing,startLength);
            }
            else if (pos < startLength && pos + tmpStruct.length > startLength)
            {
                //start
                tmpStruct.params.m_alphaLimitA = 0.0f;
                f32 alphaStart = getPatchAlpha(pos,totalLength, !_ffp.m_patchClosing,startLength);
                tmpStruct.params.m_alphaValueA = alphaStart;
                tmpStruct.params.m_startColor.m_a = alphaStart;

                //interm
                tmpStruct.params.m_alphaLimitB = /*(lenAlphaZone - pos)*/startLength / tmpStruct.length;
                tmpStruct.params.m_endColor.m_a = getPatchAlpha(lenAlphaZone,totalLength,!_ffp.m_patchClosing,startLength);

                pos += tmpStruct.length;

                //end
                tmpStruct.params.m_alphaValueB   = getPatchAlpha(pos,totalLength,!_ffp.m_patchClosing,startLength);
            }
            //else if (pos < lenAlphaZone && pos + tmpStruct.length > lenAlphaZone)
            //{
            //    //start
            //    tmpStruct.params.m_alphaLimitA = 0.0f;
            //    f32 alphaStart = getPatchAlpha(pos,totalLength, !m_patchClosing,startLength);
            //    tmpStruct.params.m_alphaValueA = alphaStart;
            //    tmpStruct.params.m_startColor.m_a = alphaStart;

            //    //interm
            //    tmpStruct.params.m_alphaLimitB = /*(lenAlphaZone - pos)*/startLength / tmpStruct.length;
            //    tmpStruct.params.m_endColor.m_a = getPatchAlpha(lenAlphaZone,totalLength,!m_patchClosing,startLength);

            //    pos += tmpStruct.length;

            //    //end
            //    tmpStruct.params.m_alphaValueB   = getPatchAlpha(pos,totalLength,!m_patchClosing,startLength);
            //}
            else
            {
                tmpStruct.params.m_alphaLimitA = 0.0f;
                f32 alphaStart = getPatchAlpha(pos,totalLength, !_ffp.m_patchClosing,startLength);
                tmpStruct.params.m_alphaValueA = alphaStart;
                tmpStruct.params.m_startColor.m_a = alphaStart;

                pos += tmpStruct.length;

                f32 alphEnd = getPatchAlpha(pos,totalLength,!_ffp.m_patchClosing,startLength);
                tmpStruct.params.m_alphaLimitB = 1.0f;
                tmpStruct.params.m_alphaValueB = alphEnd;

                tmpStruct.params.m_endColor.m_a   = alphEnd;

            }
            FluidFallProgress::SegmentCurve& segment = _ffp.m_listSegment[tmpStruct.index];
            BezierPatch& patch = segment.m_patch;

            if (totalLength > 0)
            {
                if ( getTemplate()->getTessellationRatio() > 0 )
                {
                    tmpStruct.params.m_vdivToDraw = (u32)(tmpStruct.length * getTemplate()->getTessellationRatio());
                }

                patch.drawPatch(tmpStruct.params);
            }


        }
    }

    void Ray_FluidFallAIComponent::calculateStimShape( FluidFallProgress& _ffp, ITF_VECTOR<PatchData> &paramsArray, f32 totalLength )
    {
        if (totalLength < 1.0f)
            return;

        f32 pos = 0.0f;

        _ffp.m_pointsList.clear();

        f32 currentLength = 0.0f;

        const Vec2d actorPos = m_actor->get2DPos();
        const f32 stimBezierLength = getTemplate()->getStimBezierLength();
        const f32 stimBezierWidthFactor = getTemplate()->getStimBezierWidthFactor();

        for (u32 i = 0; i < paramsArray.size(); ++i) 
        {

            PatchData & tmpStruct = paramsArray[i];

            while (pos <= currentLength + tmpStruct.length)
            {
                const f32 t = (pos - currentLength) / tmpStruct.length;
                const Vec2d bPos = getBezierPosition(tmpStruct.params.m_p0,tmpStruct.params.m_p1,tmpStruct.params.m_p2,tmpStruct.params.m_p3,t);
                Vec2d tan = getBezierTangent(tmpStruct.params.m_p0,tmpStruct.params.m_p1,tmpStruct.params.m_p2,tmpStruct.params.m_p3,t).getPerpendicular();
                tan.normalize();
                const f32 width = tmpStruct.params.m_startWidth + t * (tmpStruct.params.m_endWidth - tmpStruct.params.m_startWidth);  
                const Vec2d dir = 0.5f * width * tan * stimBezierWidthFactor;
                _ffp.m_pointsList.push_back(bPos + dir -actorPos);
                _ffp.m_pointsList.push_back(bPos - dir -actorPos);

                if (pos == totalLength)
                    pos += 1.0f; //to stop 
                else
                    pos = Min(pos + stimBezierLength,totalLength);
            } 

            currentLength += tmpStruct.length;
        }

#ifndef ITF_FINAL
        //DebugDraw::line2D()//
#endif // ITF_FINAL
    }
    void Ray_FluidFallAIComponent::calculatePolyline( FluidFallProgress& _ffp, ITF_VECTOR<PatchData> &paramsArray, f32 totalLength )
    {
        if (totalLength < 1.0f)
            return;

        f32 pos = 0.0f;

        _ffp.m_pointsList.clear();

        f32 currentLength = 0.0f;

        const Vec2d actorPos = m_actor->get2DPos();
        const f32 polylineLength = getTemplate()->getPolylineLength();

        for (u32 i = 0; i < paramsArray.size(); ++i) 
        {

            PatchData & tmpStruct = paramsArray[i];

            while (pos <= currentLength + tmpStruct.length)
            {
                const f32 t = (pos - currentLength) / tmpStruct.length;
                const Vec2d bPos = getBezierPosition(tmpStruct.params.m_p0,tmpStruct.params.m_p1,tmpStruct.params.m_p2,tmpStruct.params.m_p3,t);

                _ffp.m_pointsList.push_back(bPos);

                if (pos == totalLength)
                    pos += 1.0f; //to stop 
                else
                    pos = Min(pos + polylineLength,totalLength);
            } 

            currentLength += tmpStruct.length;
        }

#ifndef ITF_FINAL
        //DebugDraw::line2D()//
#endif // ITF_FINAL
    }

    void Ray_FluidFallAIComponent::registerObjects()
    {
        if (!m_registered)
        {
            m_registered = btrue;
            AIData * data = AI_MANAGER->addActor(m_actor->getRef(),m_actor->getDepth());
            if (data)
                data->m_faction = getTemplate()->getFaction();
        }
    }

    void Ray_FluidFallAIComponent::unregisterObjects()
    {
        if (m_registered)
        {
            m_registered = bfalse;
            AI_MANAGER->removeActor(m_actor->getRef(),m_actor->getDepth());
        }
    }

    void Ray_FluidFallAIComponent::onDepthChanged( f32 _oldZ, f32 _newZ )
    {
        Super::onDepthChanged( _oldZ,_newZ );

        if (m_registered)
        {
            AI_MANAGER->changeDepth(m_actor->getRef(),_oldZ,_newZ);
        }
    }

    void Ray_FluidFallAIComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();
        unregisterObjects();
    }

    void Ray_FluidFallAIComponent::updateStim( FluidFallProgress& ffp )
    {
        if (getTemplate()->getUseStim() && ffp.m_pointsList.size())
        {
            // Send stim
            PunchStim punchStim;
            punchStim.setReceivedHitType(RECEIVEDHITTYPE_HURTBOUNCE);
            punchStim.setIsRadial(bfalse);
            punchStim.setLevel(0);
            punchStim.setSender(m_actor->getRef());
            punchStim.setFaction(getTemplate()->getFaction());

            ITF_MAP<ActorRef,ITF_VECTOR<SCollidableContact*> > actorCollisions;

            for (u32 i = 0; i < ((ffp.m_pointsList.size() / 2) -1); ++i)
            {
                const u32 base = i * 2;
                PhysShapePolygon::PointsList pointsList;
                pointsList.push_back(ffp.m_pointsList[base]);
                pointsList.push_back(ffp.m_pointsList[base+2]);
                pointsList.push_back(ffp.m_pointsList[base+3]);
                pointsList.push_back(ffp.m_pointsList[base+1]);

                ffp.m_stimShape.setPoints(pointsList);

#ifndef ITF_FINAL
                //DebugDraw::shape(m_actor->getPos(),0.0f,&m_stimShape);
#endif // ITF_FINAL

                PhysContactsContainer contacts;
                PHYSWORLD->collidePhantoms(m_actor->get2DPos(), m_actor->get2DPos(), 0.0f,
                    &ffp.m_stimShape, m_actor->getDepth(), ECOLLISIONFILTER_CHARACTERS,
                    contacts);

                const u32 contactCount = contacts.size();

                for ( u32 i = 0; i < contactCount; i++ )
                {
                    ActorRef otherActorRef = contacts[i].m_collidableUserData;

                    if (otherActorRef == m_actor->getRef())
                    {
                        continue;
                    }

                    Actor* otherActor = otherActorRef.getActor();
                    if (!otherActor)
                    {
                        continue;
                    }

                    actorCollisions[otherActorRef].push_back(&contacts[i]);
                }
                const f32 z = m_actor->getDepth();
                punchStim.setAngle(0.0f);

                for (ITF_MAP<ActorRef,ITF_VECTOR<SCollidableContact*> >::iterator it = actorCollisions.begin(); it != actorCollisions.end(); ++it)
                {

                    ITF_VECTOR<SCollidableContact*> & collidables = it->second;
                    SCollidableContact* firstContact = collidables[0];

                    punchStim.setDirection(firstContact->m_normal);
                    punchStim.setFxPos(firstContact->m_contactPoint.to3d(z));
                    punchStim.setPos(firstContact->m_contactPoint);
                    punchStim.setPrevPos(firstContact->m_contactPoint);
                    for (ITF_VECTOR<SCollidableContact*>::iterator it2 = collidables.begin(); it2 != collidables.end(); ++it2)
                    {
                        if (punchStim.getContacts().size() < punchStim.getContacts().capacity())
                            punchStim.addContact(*(*it2));
                    }

                    Actor * actor = it->first.getActor();
                    if (actor)
                        actor->onEvent(&punchStim);
                    punchStim.resetContacts();
                }
            }

        }
    }
    void Ray_FluidFallAIComponent::updatePolyline( FluidFallProgress& ffp, f32 _dt )
    {
        if (getTemplate()->getUsePolyline())
        {
            if  (ffp.m_pointsList.size() > 1)
            {
                if (!ffp.m_polyline.isActive())
                {
                    ffp.m_polyline.activate();
                }
                ffp.m_polyline.setPoints(ffp.m_pointsList);
                ffp.m_polyline.update(_dt);
            }
            else
            {
                if (ffp.m_polyline.isActive())
                    ffp.m_polyline.deactivate();
            }
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_FluidFallAIComponent::drawEdit( class ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;


        for(u32 i = 0; i < m_listFluidProgress.size(); i++)
        {

            const FluidFallProgress& ffp = *m_listFluidProgress[i];

            if (ffp.m_pointsList.size() && getTemplate()->getUseStim())
            {
                PhysShapePolygon polygon;
                for (u32 i = 0; i < ((ffp.m_pointsList.size() / 2) -1); ++i)
                {
                    const u32 base = i * 2;
                    PhysShapePolygon::PointsList pointsList;
                    pointsList.push_back(ffp.m_pointsList[base]);
                    pointsList.push_back(ffp.m_pointsList[base+2]);
                    pointsList.push_back(ffp.m_pointsList[base+3]);
                    pointsList.push_back(ffp.m_pointsList[base+1]);

                    polygon.setPoints(pointsList);

                    DebugDraw::shape(m_actor->getPos(),0.0f,&polygon);
                }
            }
            if (getTemplate()->getUsePolyline())
            {
                ffp.m_polyline.drawEdit(_drawInterface,_flags);
            }

        }
    }

#endif // ITF_SUPPORT_EDITOR

    void Ray_FluidFallAIComponent::onActorClearComponents()
    {
        Super::onActorClearComponents();
        unregisterObjects();

        // Clear texture resources
        if (m_texture.isValidResourceId())
        {
            m_actor->removeResource(m_texture);
            m_texture.invalidateResourceId();
        }

        // Clear m_listFluidProgress
        for (ITF_VECTOR<FluidFallProgress*>::iterator it = m_listFluidProgress.begin(); it != m_listFluidProgress.end(); ++it)
        {
            delete *it;
        }

    }

    //*****************************************************************************
    // FluidFallProgress
    //*****************************************************************************

    Ray_FluidFallAIComponent::FluidFallProgress::FluidFallProgress()
    : m_deployCurDist(0.0f)
    , m_deployCurDistFluid(0.0f)
    , m_deployCurDistAnnunFX(0.0f)
    , m_deployCurOrigin(0.0f)
    , m_state(STATE_CLOSED)
    , m_patchClosing(bfalse)
    , m_currentTotalLength(0.0f)
    , m_timer(0.0f)
    , m_handleAnnunciatorFx(U32_INVALID)
    , m_handleAnnunciatorSound(U32_INVALID)
    , m_timeAnnunciationFx(0.0f)
    , m_handleStartFx(U32_INVALID)
    , m_handleEndFx(U32_INVALID)
    , m_handleStartSound(U32_INVALID)
    {
        // none
    }

    //*****************************************************************************

    void Ray_FluidFallAIComponent::FluidFallProgress::init(CurveComponent* _curveCompo, bbool _startOpen, f32 _widthStart, f32 _widthBase)
    {

        u32 countSegment = _curveCompo->getCountNode() - 1;
        if (countSegment == U32_INVALID)
        {
            countSegment = 0;
        }

        m_listSegment.clear();

        f32 widthA = _widthStart;
        f32 widthB = _widthBase;

        for (u32 i = 0; i < countSegment; i++)
        {
            FluidFallProgress::SegmentCurve newSegment;
            newSegment.m_widthStart = widthA;
            newSegment.m_widthEnd = widthB;

            m_listSegment.push_back(newSegment);

            widthA = widthB;
            widthB = _widthBase; 
        }

        if (_startOpen)
        {
            m_deployCurDist = _curveCompo->getDistCurveTotal();
            m_deployCurDistFluid = m_deployCurDist;
            m_deployCurOrigin = 0.0f;
            m_state = STATE_OPEN;
        }
        else
        {
            m_deployCurDist = 0.0f;
            m_deployCurDistFluid = 0.0f;
            m_deployCurOrigin = 0.0f;
            m_state = STATE_CLOSED;
        }   

    }


    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_FluidFallAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_FluidFallAIComponent_Template)
        SERIALIZE_MEMBER("texture",                     m_textureFile);
        SERIALIZE_MEMBER("tileLength",                  m_tileLength);
        SERIALIZE_MEMBER("tessellationLength",          m_tessellationLength);
        SERIALIZE_MEMBER("stimBezierLength",            m_stimBezierLength);
        SERIALIZE_MEMBER("stimBezierWidthFactor",       m_stimBezierWidthFactor);
        SERIALIZE_MEMBER("alphaLength",                 m_alphaLength);
        SERIALIZE_MEMBER("useStim",                     m_useStim);
        SERIALIZE_MEMBER("faction",                     m_faction);
        SERIALIZE_MEMBER("useAnnunciatorFx",            m_useAnnunciatorFx);
        SERIALIZE_MEMBER("durationAnnunciationFx",      m_durationAnnunciationFx);
        SERIALIZE_MEMBER("modTileTexture",              m_modTileTexture);

        SERIALIZE_MEMBER("fxNameAnnunciator",           m_fxNameAnnunciator);
        SERIALIZE_MEMBER("fxNameStartFFP",              m_fxNameStartFFP);
        SERIALIZE_MEMBER("fxNameEndFFP",                m_fxNameEndFFP);
        
        SERIALIZE_MEMBER("soundNameStartFFP",           m_soundNameStartFFP);
        SERIALIZE_MEMBER("soundNameAnnunciatorFFP",     m_soundNameAnnunciatorFFP);

        SERIALIZE_MEMBER("usePolyline",                 m_usePolyline);
        SERIALIZE_MEMBER("polylineLength",              m_polylineLength);
        SERIALIZE_MEMBER("gameMaterial",                m_gameMaterial);

        SERIALIZE_MEMBER("zOffset",                     m_zOffset);
        SERIALIZE_MEMBER("ignoreEventTrigger",          m_ignoreEventTrigger);

    END_SERIALIZATION()

    //*****************************************************************************

    Ray_FluidFallAIComponent_Template::Ray_FluidFallAIComponent_Template()
    : m_tileLength(0.0f) 
    , m_tessellationLength(0.1f)
    , m_tessellationRatio(0.f)
    , m_drawUvFromRoot(btrue)
    , m_stimBezierLength(1.0f)
    , m_stimBezierWidthFactor(0.2f)
    , m_alphaLength(2.0f)
    , m_useStim(bfalse)
    , m_faction(0)
    , m_useAnnunciatorFx(bfalse)
    , m_durationAnnunciationFx(5.0f)
    , m_modTileTexture(10)
    , m_usePolyline(bfalse)
    , m_polylineLength(2.0f)
    , m_zOffset(0.0f)
    , m_ignoreEventTrigger(bfalse)
    {
        // none
    }

    //*****************************************************************************

    Ray_FluidFallAIComponent_Template::~Ray_FluidFallAIComponent_Template()
    {
        // none
    }

    void Ray_FluidFallAIComponent_Template::onTemplateLoaded()
    {
        if (m_tessellationLength > 0)
        {
            m_tessellationRatio = f32_Inv(m_tessellationLength);
        }
    }

    //*****************************************************************************


};
