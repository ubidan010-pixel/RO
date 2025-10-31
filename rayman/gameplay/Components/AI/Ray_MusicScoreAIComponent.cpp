#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_MUSICSCOREAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_MusicScoreAIComponent.h"
#endif //_ITF_RAY_MUSICSCOREAICOMPONENT_H_

#ifndef _ITF_RAY_MUSICSCORESNAPCOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_MusicScoreSnapComponent.h"
#endif //_ITF_RAY_MUSICSCORESNAPCOMPONENT_H_

#ifndef _ITF_CURVECOMPONENT_H_
#include "gameplay/Components/Misc/CurveComponent.h"
#endif //_ITF_CURVECOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#define STATE_CLOSED                    0
#define STATE_OPENING                   1
#define STATE_OPEN                      2
#define STATE_CLOSING                   3

#define MESSAGE_TYPE_REGISTER                  0
#define MESSAGE_TYPE_UPDATE_OPENING            1
#define MESSAGE_TYPE_UPDATE_OPENING_FORCED     2
#define MESSAGE_TYPE_UPDATE_CLOSING            3
#define MESSAGE_TYPE_SNAP                      4

#define COUNT_NOTES                     5

#define LEN_ALPHA_ZONE                  2.0f

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_MusicScoreAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_MusicScoreAIComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("startOpen",  m_startOpen);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_MusicScoreAIComponent)
        VALIDATE_COMPONENT_PARAM("", m_curveComponent,  "Ray_MusicScoreAIComponent requires an CurveComponent.");
    END_VALIDATE_COMPONENT()

    //*****************************************************************************

    Ray_MusicScoreAIComponent::Ray_MusicScoreAIComponent()
    : m_curveComponent(NULL)
    , m_fxControllerComponent(NULL)
    , m_deployCurDist(0.0f)
    , m_state(STATE_CLOSED)
    , m_timer(0.0f)
    , m_registered(bfalse)
    , m_startOpen(bfalse)
    {
        // none
    }

    //*****************************************************************************

    Ray_MusicScoreAIComponent::~Ray_MusicScoreAIComponent()
    {
        if (!m_actor)
        {
            return;
        }

        // Clear texture resources
        if (m_texture.isValidResourceId())
        {
            m_actor->removeResource(m_texture);
            m_texture.invalidateResourceId();
        }
    }

    //*****************************************************************************

    void Ray_MusicScoreAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Register Events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);

        // Get components
        m_curveComponent = m_actor->GetComponent<CurveComponent>(); // Mandatory
        m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();

        ITF_ASSERT(m_curveComponent);

        // Init resources
        if (!getTemplate()->getTextureFile().isEmpty())
        {
            m_texture = m_actor->addResource(Resource::ResourceType_Texture,getTemplate()->getTextureFile());
        }

        // Init Compo
        init();


 
    }

    //*****************************************************************************

    void Ray_MusicScoreAIComponent::init(bbool _isReinit /*=bfalse*/)
    {
        u32 countSegment = m_curveComponent->getCountNode() - 1;
        if (countSegment == U32_INVALID)
        {
            countSegment = 0;
        }

        m_listSegment.clear();

        f32 widthBase = getTemplate()->getWidthBase();
        f32 amplitudeSin = getTemplate()->getAmplitudeSin();
        f32 frequencySin = getTemplate()->getFrequencySin();

        ITF_WARNING(m_actor, widthBase > amplitudeSin, "AplitudeSin must be smaller than widthBase !");

        f32 widthA = getTemplate()->getWidthStart();
        f32 widthB = widthBase;


        for (u32 i = 0; i < countSegment; i++)
        {
            SegmentMusic newSegment;
            newSegment.m_widthStart = widthA;
            newSegment.m_widthEnd = widthB;

            m_listSegment.push_back(newSegment);

            widthA = widthB;
            f32 sinRes = f32_Sin((i+1) * frequencySin);
            widthB = widthBase + (sinRes * amplitudeSin); 
        }
 
        if (_isReinit)
        {
            m_deployCurDist = m_curveComponent->getDistCurveTotal();
        }
        else
        {
            if (m_startOpen)
            {
                m_deployCurDist = m_curveComponent->getDistCurveTotal();
                m_state = STATE_OPEN;
            }
            else
            {
                m_deployCurDist = 0.0f;
                m_state = STATE_CLOSED;
            }
        }

    }

    //*****************************************************************************

    void Ray_MusicScoreAIComponent::forceOpen()
    {
        m_deployCurDist = m_curveComponent->getDistCurveTotal();
        m_state = STATE_OPEN;
        updateOpeningForChild(btrue);
    }

    //*****************************************************************************

    void Ray_MusicScoreAIComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event);

        if(EventTrigger* trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
        {
            receiveEvent();
        }
    }

    //*****************************************************************************

    void Ray_MusicScoreAIComponent::receiveEvent()
    {
        switch (m_state)
        {
            case STATE_CLOSED:
            case STATE_CLOSING:
                open();
                break;

            case STATE_OPEN:
            case STATE_OPENING:
                close();
                break;
        }
    }

    //*****************************************************************************

    void Ray_MusicScoreAIComponent::open()
    {
        m_state = STATE_OPENING;
        playSound(getTemplate()->getOpenSoundID());
    }

    //*****************************************************************************

    void Ray_MusicScoreAIComponent::close()
    {
        m_state = STATE_CLOSING;
        playSound(getTemplate()->getCloseSoundID());
    }

    //*****************************************************************************

    void Ray_MusicScoreAIComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        if (!m_registered)
        {
            registerOnChilds();

            if (m_startOpen)
            {
                updateOpeningForChild(btrue);
            }

            m_registered = btrue;
        }
    }

    //*****************************************************************************

    void Ray_MusicScoreAIComponent::sendMessageToChilds( u32 _messageType )
    {
        
        AIUtils::ActorBindIterator iterator(m_actor);
        while ( Actor* child = iterator.getNextChild() )
        {
            Ray_MusicScoreSnapComponent* musicScoreSnapAIComponent = child->GetComponent<Ray_MusicScoreSnapComponent>();

            if (musicScoreSnapAIComponent)
            {    
                switch(_messageType)
                {
                case MESSAGE_TYPE_REGISTER:
                    musicScoreSnapAIComponent->registerMusicScore(m_actor);    
                    break;
                case MESSAGE_TYPE_UPDATE_OPENING:
                    musicScoreSnapAIComponent->updateOpening(m_deployCurDist, bfalse);
                    break;
                case MESSAGE_TYPE_UPDATE_OPENING_FORCED:
                    m_deployCurDist = m_curveComponent->getDistCurveTotal();
                    musicScoreSnapAIComponent->updateOpening(m_deployCurDist, btrue);
                    break;
                case MESSAGE_TYPE_UPDATE_CLOSING:
                    musicScoreSnapAIComponent->updateClosing(m_deployCurDist);
                    break;
                }
            }
        }


/*
        if (m_linkComponent)
        {
            const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();
            const u32 childrenCount = children.size();

            for (u32 i = 0; i < childrenCount; i++)
            {
                if ( Pickable* child = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[i].getPath()) )
                {
                    Actor* actorChild = child->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

                    if ( actorChild )
                    {
                        Ray_MusicScoreSnapComponent* musicScoreSnapAIComponent = actorChild->GetComponent<Ray_MusicScoreSnapComponent>();

                        if (musicScoreSnapAIComponent)
                        {    
                            switch(_messageType)
                            {
                            case MESSAGE_TYPE_REGISTER:
                                musicScoreSnapAIComponent->registerMusicScore(m_actor);    
                                break;
                            case MESSAGE_TYPE_UPDATE_OPENING:
                                musicScoreSnapAIComponent->updateOpening(m_deployCurDist);
                                break;
                            case MESSAGE_TYPE_UPDATE_CLOSING:
                                musicScoreSnapAIComponent->updateClosing(m_deployCurDist);
                                break;
                            }
                        }
                    }
                }
            }
        }

*/

    }

    //*****************************************************************************

    void Ray_MusicScoreAIComponent::registerOnChilds()
    {
        sendMessageToChilds(MESSAGE_TYPE_REGISTER);     
    }

    //*****************************************************************************

    void Ray_MusicScoreAIComponent::updateOpeningForChild(bbool _forced)
    {
        sendMessageToChilds(_forced
            ? MESSAGE_TYPE_UPDATE_OPENING_FORCED
            : MESSAGE_TYPE_UPDATE_OPENING);
    }

    //*****************************************************************************

    void Ray_MusicScoreAIComponent::updateClosingForChild()
    {
        sendMessageToChilds(MESSAGE_TYPE_UPDATE_CLOSING);   
    }

    //*****************************************************************************

    void Ray_MusicScoreAIComponent::Update( f32 _dt ) 
    {
        Super::Update(_dt);

        m_timer += _dt;
        growAABB(); // TODO : on update is bad ??
        updateDeploy(_dt);

    }

    //*****************************************************************************

    f32 Ray_MusicScoreAIComponent::getNoteInterval(u32 _indexSegment, Vec3d& _tangent3D, const Vec3d& _closetPosOut, bbool& _isBadSnap, bbool& _reverse, f32 t)
    {
        Vec2d centerNote = _closetPosOut.truncateTo2D();

		if (_indexSegment >= m_listSegment.size() )
		{
			return 0.0f;
		}

        SegmentMusic& segment = m_listSegment[_indexSegment];
        f32 curWidth  =  segment.m_widthStart + ((segment.m_widthEnd-segment.m_widthStart) * t);

        Vec2d tangent2D = _tangent3D.truncateTo2D();
        Vec2d perpendicular = tangent2D.getPerpendicular();
        perpendicular.normalize();

        f32 dot = perpendicular.dot(Vec2d::Up);

        _reverse = bfalse;
        if (dot < 0.0)
        {
            // Reverse : Curve is right to left
            dot = perpendicular.dot(-Vec2d::Up);
            _tangent3D = -_tangent3D;
            tangent2D = -tangent2D;
            perpendicular = -perpendicular;
            _reverse = btrue;
        }

        _isBadSnap = bfalse;
        if (dot <= 0.30f)
        {
            _isBadSnap = btrue;
#ifdef ITF_SUPPORT_DEBUGFEATURE
            DebugDraw::text(_closetPosOut.truncateTo2D() + Vec2d(2.0f, 1.0f), 0.0f, Color::red(), "Warning : here the bird can fall directly !", dot);
#endif // ITF_SUPPORT_DEBUGFEATURE
        }

        f32 demiWidthPatchOnCurve = (1.0f / dot);
        demiWidthPatchOnCurve = demiWidthPatchOnCurve * (curWidth * 0.5f);

        // Compute interval between notes
        f32 ratio = (demiWidthPatchOnCurve * 2.0f) / curWidth;
        f32 intervalFromPNG = getTemplate()->getIntervalNotePNG();
        f32 intervalNote = intervalFromPNG * curWidth;
        
#ifndef ITF_FINAL
#if 0
        // ### DEBUG
        // Draw dot product
        DebugDraw::text(_closetPosOut.truncateTo2D(), 0.0f, Color::black(), "%f", dot);

        // Draw perpendicular
        DebugDraw::line2D(centerNote, centerNote + perpendicular, 0.0f, Color::pink(), 4.0f, 1.0f);

        // Draw note pos
        DebugDraw::circle(centerNote + Vec2d(0.0f, intervalNote * ratio) * 2.0f, 0.0f, 0.1f, Color::pink(), 1.0f);
        DebugDraw::circle(centerNote + Vec2d(0.0f, intervalNote * ratio) * 1.0f, 0.0f, 0.1f, Color::pink(), 1.0f);
        DebugDraw::circle(centerNote, 0.0f, 0.1f, Color::pink(), 1.0f);
        DebugDraw::circle(centerNote - Vec2d(0.0f, intervalNote * ratio) * 1.0f, 0.0f, 0.1f, Color::pink(), 1.0f);
        DebugDraw::circle(centerNote - Vec2d(0.0f, intervalNote * ratio) * 2.0f, 0.0f, 0.1f, Color::pink(), 1.0f);

        // Draw width musicscore
        DebugDraw::line2D(centerNote, centerNote + (Vec2d::Up * demiWidthPatchOnCurve), 0.0f, Color::blue(), 4.0f, 1.0f);
        DebugDraw::line2D(centerNote, centerNote - (Vec2d::Up * demiWidthPatchOnCurve), 0.0f, Color::red(), 4.0f, 1.0f);
        // ### DEBUG
#endif
#endif // ITF_FINAL
        
        return intervalNote * ratio;
    }

    //*****************************************************************************
    
    // EDITOR ONLY !!!
    void Ray_MusicScoreAIComponent::getPosAtDist(Vec3d& _posOut, f32 _distOnCurve, u32& _selectedNote, f32& _intervalNote, bbool& _isBadSnap, bbool& _reverse)
    {
        Vec3d tangent3D;
        f32 t = 0.0f;
        u32 indexSegment = 0;
        m_curveComponent->getPosAndIndexSegAtDist(_distOnCurve, _posOut, tangent3D, indexSegment, t);
        
        if (indexSegment == U32_INVALID || indexSegment >= m_listSegment.size())
        {
            ITF_ASSERT(0);
            return;
        }

        bbool wasReverse = _reverse;

        _intervalNote = getNoteInterval(indexSegment, tangent3D, _posOut, _isBadSnap, _reverse, t);

        if (_reverse != wasReverse)
        {
            _selectedNote = COUNT_NOTES - _selectedNote - 1;
        }

        Vec3d vecIntervalNote = Vec3d::Up * _intervalNote;
        Vec3d posNoteTop = _posOut + (vecIntervalNote * 2.0f); // TODO : 2.0f -> COUNT_NOTES
        _posOut =  posNoteTop - (vecIntervalNote * (f32)_selectedNote);
    }

    //*****************************************************************************

    // EDITOR ONLY !!!
    void Ray_MusicScoreAIComponent::getClosestPos( Vec3d _refPos, Vec3d& _closetPosOut, f32& _distOnCurveOut, u32& _selectedNote, f32& _intervalNote, bbool& _isBadSnap, bbool& _reverse)
    {   
        Vec3d tangent3D;
        f32 t = 0.0f;
        u32 indexSegment = 0;
        m_curveComponent->getClosestPos(_refPos, _closetPosOut, tangent3D, _distOnCurveOut, indexSegment, t);

        if (indexSegment == U32_INVALID || indexSegment >= m_listSegment.size())
        {
            ITF_ASSERT(0);
            return;
        }

        _intervalNote = getNoteInterval(indexSegment, tangent3D, _closetPosOut, _isBadSnap, _reverse, t);

        // Compute snap on specif note
        f32 distMin = F32_INFINITY;
        Vec3d vecIntervalNote = Vec3d::Up * _intervalNote;
        Vec3d posNoteTop = _closetPosOut + (vecIntervalNote * 2.0f); // TODO : 2.0f -> COUNT_NOTES
        u32 selectedNote = 0;

        for(u32 i = 0; i < COUNT_NOTES; i++)
        {
            Vec3d posCurNote =  posNoteTop - (vecIntervalNote * (f32)i);

            f32 dist = (_refPos - posCurNote).sqrnorm();

            if (dist < distMin)
            {
                distMin = dist;
                selectedNote = i;
                _closetPosOut = posCurNote;
            }
        }

        _selectedNote = selectedNote;
    }

    //*****************************************************************************

    void Ray_MusicScoreAIComponent::updateDeploy( f32 _dt)
    {        
        if (m_state != STATE_OPENING && m_state != STATE_CLOSING)
        {
            return;
        }

        f32 dist = getTemplate()->getOpeningSpeed() * _dt;


        if (m_state == STATE_OPENING)
        {
            m_deployCurDist += dist;
        }
        else if(m_state == STATE_CLOSING)
        {
            m_deployCurDist -= dist;
        }
        

        if (m_deployCurDist > 0 && m_deployCurDist <= m_curveComponent->getDistCurveTotal())
        {
            Vec3d deployCurPos, deployCurTgt;
            u32 deployCurSegment = 0;    
            f32 t = 0.0f; 

            m_curveComponent->getPosAndIndexSegAtDist(m_deployCurDist, deployCurPos, deployCurTgt, deployCurSegment, t);

#ifndef ITF_FINAL
            // Debug
            //DebugDraw::circle(deployCurPos.truncateTo2D(), 0.0f, 0.5f, Color::red(), 0.0f);
            //DebugDraw::text(deployCurPos.truncateTo2D(), 0.0f, Color::red(), "Seg : %d", deployCurSegment);
#endif // ITF_FINAL
        }
        else
        {
            if (m_state == STATE_OPENING)
            {
                m_deployCurDist = m_curveComponent->getDistCurveTotal();
                m_state = STATE_OPEN;
            }
            else if(m_state == STATE_CLOSING)
            {
                m_deployCurDist = 0.0f;
                m_state = STATE_CLOSED;
            }
        }

        // update child deploy
        if (m_state == STATE_OPENING)
        {
            updateOpeningForChild(bfalse);
        }
        else if(m_state == STATE_CLOSING)
        {
            updateClosingForChild();
        }
    }

    //*****************************************************************************

    void Ray_MusicScoreAIComponent::growAABB()
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

    void Ray_MusicScoreAIComponent::Draw()
    {
        Super::Draw();

        if (m_state == STATE_CLOSED)
        {
            return;
        }

        if (m_listSegment.size() == 0)
        {
            return;
        }
        
        if (m_deployCurDist == 0.0f)
        {
            return;
        }

        CurveComponent::TrajectoryArray& nodeArray = m_curveComponent->getNodeArrayRef();

        Vec2d actorPos = m_actor->get2DPos();
        
        Vec3d deployCurPos, deployCurTgt;
        u32 deployCurSegment = 0;  
        f32 k = 0.0f;
        f32 distAlreadyDeploy = 0.0f;

        m_curveComponent->getPosAndIndexSegAtDist(m_deployCurDist, deployCurPos, deployCurTgt, deployCurSegment, k);

        f32 curTile;

        if (getTemplate()->getDrawUvFromRoot())
        {
            // deploy from root
            curTile = -(m_deployCurDist / getTemplate()->getTileLength());
        }
        else
        {
            // deploy from head
            curTile = 0.0f;
        }

        for (u32 i = 0; i <= deployCurSegment; i++) 
        {

            if(m_state == STATE_CLOSED)
            {
                return;
            }

            SegmentMusic& segment = m_listSegment[i];
            BezierPatch& patch = segment.m_patch;
            BezierPatchParams params;

            CurveComponent::TrajectoryNode& nodeSource  = nodeArray[i];
            CurveComponent::TrajectoryNode& nodeDest    = nodeArray[i+1];

            const Vec2d p0 = m_curveComponent->actorTransform(nodeSource.m_pos).truncateTo2D();
            const Vec2d p1 = m_curveComponent->actorTransform(nodeSource.m_tangB).truncateTo2D();
            const Vec2d p2 = m_curveComponent->actorTransform(nodeDest.m_tangA).truncateTo2D();
            const Vec2d p3 = m_curveComponent->actorTransform(nodeDest.m_pos).truncateTo2D();    


            params.m_z = m_actor->getDepth();
            params.m_texture = (Texture*)m_texture.getResource();

            params.m_startColor = params.m_endColor = getColorFactor();
            params.m_fogColor = getColorFog();


            // Alpha blend
            params.m_alphaLimitA = 0.2f;
            params.m_alphaValueA = 1.0f;

            params.m_alphaLimitB = 0.8f;
            params.m_alphaValueB = 1.0f;


            if (i == deployCurSegment)
            {   
                f32 countTileSeg = (m_deployCurDist - distAlreadyDeploy) / getTemplate()->getTileLength();
                params.m_startUV.m_x = curTile;
                params.m_endUV.m_x = curTile + countTileSeg;


                params.m_p0 = p0;
                params.m_p1 = p0 * (1 - k) + p1 * k;
                params.m_p2 = p0 * powf(1-k, 2) + 2 * (p1 * k) * (1-k) + p2 * powf(k, 2);
                params.m_p3 = deployCurPos.truncateTo2D(); 

#ifndef ITF_FINAL
                //DebugDraw::circle(q1, 0.0f, 0.2f, Color::blue(), 0.0f);
                //DebugDraw::circle(q2, 0.0f, 0.2f, Color::blue(), 0.0f);   
                //DebugDraw::bezier(q0, q1, q2, q3, 0.0f, Color::blue(), 4.0f);
#endif // ITF_FINAL

                // compute t of alpha zone
                f32 distDeployOnThisSeg = m_deployCurDist - distAlreadyDeploy;              
                if (distDeployOnThisSeg <= LEN_ALPHA_ZONE)
                {
                    params.m_alphaLimitA      = 1.0f;
                    params.m_alphaValueA      = distDeployOnThisSeg / LEN_ALPHA_ZONE;

                    params.m_alphaLimitB      = 1.0f;
                    params.m_alphaValueB      = 0.0f;

                    params.m_startColor.m_a = 0.0f;
                    params.m_endColor.m_a   = 0.0f;
                }
                else
                {
                    params.m_alphaLimitA = 0.0f;
                    params.m_alphaValueA = 1.0;

                    params.m_alphaLimitB = (distDeployOnThisSeg - LEN_ALPHA_ZONE) / distDeployOnThisSeg;
                    params.m_alphaValueB = 0.0f;
                }

                params.m_startWidth = segment.m_widthStart;
                params.m_endWidth = segment.m_widthStart + ((segment.m_widthEnd-segment.m_widthStart) * k);

                patch.drawPatch(params);
            }
            else
            {
                f32 countTileSeg   = nodeSource.m_lengthSeg / getTemplate()->getTileLength(); 
                params.m_startUV.m_x = curTile;
                params.m_endUV.m_x = curTile + countTileSeg;

                params.m_p0 = p0;
                params.m_p1 = p1;
                params.m_p2 = p2;
                params.m_p3 = p3;

                if (i == deployCurSegment - 1)
                {                    
                    // compute t of alpha zone
                    f32 distDeployOnLastSeg = m_deployCurDist - (distAlreadyDeploy + nodeSource.m_lengthSeg);

                    if (distDeployOnLastSeg <= LEN_ALPHA_ZONE)
                    { 
                        f32 distAlphaOnPrevSeg = LEN_ALPHA_ZONE - distDeployOnLastSeg;

                        params.m_alphaLimitA = 0.1f; // TODO : mettre 0.0 ??
                        params.m_alphaValueA = 1.0f;

                        params.m_alphaLimitB = (nodeSource.m_lengthSeg - distAlphaOnPrevSeg) / nodeSource.m_lengthSeg;
                        params.m_alphaValueB = 1.0f - (distAlphaOnPrevSeg / LEN_ALPHA_ZONE);
                    }
                }

                params.m_startWidth = segment.m_widthStart;
                params.m_endWidth = segment.m_widthEnd;

                patch.drawPatch(params);


                curTile += countTileSeg;                
                distAlreadyDeploy += nodeSource.m_lengthSeg;                    
            }  
        }
    }
   
    //*****************************************************************************

    void Ray_MusicScoreAIComponent::playSound(const StringID& _sound)
    {
        if (m_fxControllerComponent && _sound.isValid())
        {
            m_fxControllerComponent->playFX(_sound);
        }
    }

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_MusicScoreAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_MusicScoreAIComponent_Template)

        SERIALIZE_MEMBER("texture",         m_textureFile);
        SERIALIZE_MEMBER("tileLength",      m_tileLength);

        SERIALIZE_MEMBER("widthStart",      m_widthStart);
        SERIALIZE_MEMBER("widthBase",       m_widthBase);
        SERIALIZE_MEMBER("amplitudeSin",    m_amplitudeSin);
        SERIALIZE_MEMBER("frequencySin",    m_frequencySin);

        SERIALIZE_MEMBER("openingSpeed",    m_openingSpeed);
        SERIALIZE_MEMBER("drawUvFromRoot",  m_drawUvFromRoot);

        SERIALIZE_MEMBER("intervalNotePNG", m_intervalNotePNG);

        SERIALIZE_MEMBER("openSound",       m_openSound);
        SERIALIZE_MEMBER("closeSound",      m_closeSound);

    END_SERIALIZATION()

    //*****************************************************************************

    Ray_MusicScoreAIComponent_Template::Ray_MusicScoreAIComponent_Template()
    : m_widthStart(1.0f)
    , m_widthBase(1.0f)
    , m_amplitudeSin(0.0f)
    , m_frequencySin(0.0f)
    , m_tileLength(0.0f) 
    , m_openingSpeed(5.0f)
    , m_drawUvFromRoot(btrue)
    , m_intervalNotePNG(0.2f)
    {
        // none
    }

    //*****************************************************************************

    Ray_MusicScoreAIComponent_Template::~Ray_MusicScoreAIComponent_Template()
    {
        // none
    }

    //*****************************************************************************


};
