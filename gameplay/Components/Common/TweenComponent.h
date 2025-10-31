#ifndef _ITF_TWEENCOMPONENT_H_
#define _ITF_TWEENCOMPONENT_H_

#ifndef _ITF_TWEENINSTRUCTIONS_H_
#include "gameplay/Components/Common/TweenInstructions.h"
#endif //_ITF_TWEENINSTRUCTIONS_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

#ifndef _ITF_METRONOMEMANAGER_H_
#include "engine/sound/MetronomeManager.h"
#endif //_ITF_METRONOMEMANAGER_H_

namespace ITF
{
    class TweenInstruction;
    class EventActivate;

    class TweenComponent : public ActorComponent
    {
        friend class Tweening_Plugin;
        friend class Ray_BezierTree_Plugin;

        DECLARE_OBJECT_CHILD_RTTI(TweenComponent, ActorComponent,3047809396);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        TweenComponent();
        virtual ~TweenComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onBecomeActive();
        virtual void        onSceneActive();
        virtual void        onCheckpointLoaded();
        virtual void        onEvent( Event* _event );
        virtual void        Update( f32 _dt );

#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface , u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR
        void                drawSet( const Transform3d& _initTransform, const StringID& _setName ) const;

        Vec3d               getPosAtTime( f32 _t, f32* _angle = NULL );
        Vec3d               getPosAtTimeOffset( f32 _tOffset, f32* _angle = NULL );

        ITF_INLINE const Vec3d& getTargetPos() const { return m_targetPos; }

        ITF_INLINE void     setApplyCoordinates( const bbool _value ) { m_applyPosition = m_applyRotation = m_applyScale = _value; }
        ITF_INLINE bbool    getApplyPosition() const { return m_applyPosition; }
        ITF_INLINE void     setApplyPosition( const bbool _value ) { m_applyPosition = _value; }
        ITF_INLINE bbool    getApplyRotation() const { return m_applyRotation; }
        ITF_INLINE void     setApplyRotation( const bbool _value ) { m_applyRotation = _value; }
        ITF_INLINE bbool    getApplyScale() const { return m_applyScale; }
        ITF_INLINE void     setApplyScale( const bbool _value ) { m_applyScale = _value; }
        ITF_INLINE bbool    isPlayingSet() const { return ( !m_activated || isTweeningSuspended() ? bfalse : ( m_currentSetIndex >= m_instructionSets.size() ? bfalse : btrue ) ); }

        void                setStartSet( const StringID& _setName ) { playSet(_setName); }
        void                playSet( const StringID& _setName );
        void                stopCurrentSet() { queueSet(U32_INVALID); }
        ITF_INLINE u32      getCurrentSetIndex() const { return m_currentSetIndex; }

        bbool               isTweeningSuspended() const { return isPaused(); }
        void                suspendTweening() { pause(); }
        void                resumeTweening();

        // for editor
        bbool               getSetTransform( const TweenInstruction* _instruction, Transform3d& _setTransform ) const;

        // for instructions
        void                sendEventToChildren( Event* _event );

        // Inputs
        template <typename T>
        void                    setInput(StringID _inputName, T _value );
        const InputContainer&   getInputs() const { return m_inputList; }

    private:

        ITF_INLINE const class TweenComponent_Template* getTemplate() const;

        LinkComponent*      m_linkComponent;

        class InstructionSet
        {
            DECLARE_SERIALIZE()

        public:
            InstructionSet();
            ~InstructionSet();

            void    clear();

            const StringID& getName() const { return m_name; }

            StringID            m_name;
            InstructionList     m_instructions;
        };

        typedef ITF_VECTOR<InstructionSet> InstructionSetList;

        InstructionSet*     findSet( InstructionSetList& _sets, const StringID& _name) const;
        TweenInstruction*   findInstruction( InstructionList& _instructions, const StringID& _name) const;
        TweenInstruction*   findAndRemoveInstruction( InstructionSet* _set, const StringID& _name) const;
        //InstructionSet_Template* findSet( InstructionSetList_Template& _sets, const StringID& _name) const;
        void                reinit();

        void                registerStartStopEvents();
        void                initInstructionSets();
        u32                 getStartSetIndex() const;
        void                playStartSet();
        void                initPosition();
        void                processActivation( EventActivate* _eventActivate );
        void                playNextTriggableSet();
        u32                 getTriggableIndex( const u32 _setIndex ) const;
        void                queueSet( u32 _setIndex );

        void                onSetFinished( InstructionSet& _set );
        void                computeSyncOffset( const bbool _accountForCurrentSetTime = bfalse );
        void                jumpToTime( const InstructionList& _instructions, f32 _time );
        void                applyCoordinates( const TweenCoordinates& _coordinates, const f32 _actorAngleOffset, const f32 _dt );

        // instance-serialized (overrides template data)
        f32                 m_syncOffset_instance;
        StringID            m_startSet;

        InstructionSetList  m_instructionSets;

        u32                 m_activated:1,
                            m_applyPosition:1,
                            m_applyRotation:1,
                            m_applyScale:1,
                            m_justStarted:1;


        u32                 m_currentSetIndex;
        u32                 m_nextSetIndex;
        u32                 m_currentSetIteration;
        u32                 m_triggableIndex;

        u32                 m_instructionIndex;
        f32                 m_instructionTime;      // time since instruction start
        f32                 m_instructionSetSyncOffset; // [0,1]
        f32                 m_instructionSetTime;   // time since set start

        TweenCoordinates    m_setBaseCoordinates;   // coordinates at last InstructionSet end
        TweenCoordinates    m_baseCoordinates;      // coordinates at last instruction end
        TweenCoordinates    m_currentCoordinates;   // coordinates at last frame

        Vec3d               m_targetPos;            // for accessor & debug

        // Inputs
        void                initInputs();
        InputContainer      m_inputList;


        bbool               getPosAtTime( const InstructionList& _instructions, f32& _time, TweenCoordinates& _coordinates );

#ifdef ITF_SUPPORT_EDITOR
        void                drawEditCurrent() const;
        void                drawEditDeterministic() const;

        void                drawSetAndNexts( const u32 _firstSetIndex, const u32 _triggableOffset,
                                         const Transform3d& _initTransform, TweenCoordinates& _coordinates, bbool _altFirstSetColor = bfalse ) const;
        void                pushSetAndNexts( SafeArray<u32>& _setList, u32 _setIndex ) const;

        void                drawSetList( SafeArray<u32>& _setList,
                                         const Transform3d& _initTransform, TweenCoordinates& _coordinates, bbool _altFirstSetColor = bfalse ) const;
        void                drawExtraIterations( const u32 _setIndex,
                                                 const Transform3d& _initTransform, TweenCoordinates& _coordinates ) const;
        void                drawInstructionList( const InstructionList& _instructions,
                                                 const Transform3d& _initTransform, TweenCoordinates& _coordinates,
                                                 const Color& _normalColor, const Color& _alternateColor ) const;

        void                drawCurrentInfo() const;
        void                drawDebugText( f32& textY, const char* _format, ... ) const;

        static bbool        s_debugLock;
#endif // ITF_SUPPORT_EDITOR

#ifdef ITF_SUPPORT_DEBUGFEATURE
        void                resetDebugSpeed();

        f32                 m_debugLinearSpeed2d;
        f32                 m_debugLinearSpeed3d;
        f32                 m_debugAngularSpeed;
#endif // ITF_SUPPORT_DEBUGFEATURE

//#define DEBUG_TWEENING
#ifdef DEBUG_TWEENING
        struct DebugCoordinates
        {
            u32                 m_currentSetIndex;
            u32                 m_currentSetIteration;
            u32                 m_nextSetIndex;
            u32                 m_instructionIndex;
            f32                 m_instructionTime;      // time since instruction start
            f32                 m_instructionSetTime;   // time since set start
            f32                 m_instructionSetSyncOffset; // [0,1]
        };
        SafeArray<DebugCoordinates> m_prevCoordinates;
#endif // DEBUG_TWEENING
    };

    //-------------------------------------------------------------------------------------
    class TweenComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(TweenComponent_Template,TemplateActorComponent,2864992789);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(TweenComponent);

    public:

        class InstructionSet
        {
            DECLARE_SERIALIZE()

        public:
            InstructionSet();
            ~InstructionSet();

            void                            clear();
            void                            onTemplateLoaded();

            StringID                        getName() const { return m_name; }
            const InstructionList_Template& getInstructions() const { return m_instructions; }
            u32                             getIterationCount() const { return m_iterationCount; }
            f32                             getActorAngleOffset() const { return m_angleOffset.ToRadians(); }
            const Event*                    getStartEvent() const { return m_startEvent; }
            const Event*                    getStopEvent() const { return m_stopEvent; }
            ITF_INLINE bbool                getInterruptible() const { return m_interruptible; }
            ITF_INLINE bbool                getTriggable() const { return m_triggable; }
            ITF_INLINE const StringID&      getNextSet() const { return m_nextSet; }
            ITF_INLINE u32                  getNextSetIndex() const { return m_nextSetIndex; }
            ITF_INLINE void                 setNextSetIndex( u32 _value ) { m_nextSetIndex = _value; }
            const ProceduralInputData&      getProceduralCursor() const { return m_proceduralCursor; }

            // computed onTemplateLoaded
            ITF_INLINE f32                  getDuration() const { return m_duration; }
            ITF_INLINE f32                  getSyncRatio() const { return m_syncRatio; }
            ITF_INLINE bbool                getIsLooping() const { return m_isLooping; }

        private:

            StringID                m_name;
            InstructionList_Template m_instructions;
            u32                     m_iterationCount;
            AngleAmount             m_angleOffset;
            Event*                  m_startEvent;
            Event*                  m_stopEvent;
            bbool                   m_interruptible;
            bbool                   m_triggable;
            StringID                m_nextSet;
            ProceduralInputData     m_proceduralCursor;

            // computed onTemplateLoaded
            f32     m_duration;
            f32     m_syncRatio;
            bbool   m_isLooping;
            u32     m_nextSetIndex;
        };

        typedef ITF_VECTOR<InstructionSet> InstructionSetList;
        typedef SafeArray<u32> TriggableSetList;


        enum StartMode
        {
            // on by default, off if linked to a trigger
            StartMode_Auto,
            // always on
            StartMode_On,
            // always off
            StartMode_Off,
            ENUM_FORCE_SIZE_32(StartMode)
        };

        #define SERIALIZE_STARTMODE(name,val) \
        SERIALIZE_ENUM_BEGIN(name,val); \
            SERIALIZE_ENUM_VAR(StartMode_Auto); \
            SERIALIZE_ENUM_VAR(StartMode_On); \
            SERIALIZE_ENUM_VAR(StartMode_Off); \
        SERIALIZE_ENUM_END();


        TweenComponent_Template();
        ~TweenComponent_Template() {}

        const InstructionSetList&       getInstructionSets() const { return m_instructionSets; }
        bbool                           getSyncWithMetronome() const { return m_syncWithMetronome; }
        ITF_INLINE MetronomeType        getMetronome() const { return m_metronome; }
        f32                             getSyncOffset() const { return m_syncOffset; }
        ITF_INLINE StartMode            getStartMode() const { return m_startMode; }
        ITF_INLINE const StringID&      getStartSet() const { return m_startSet; }
        ITF_INLINE bbool                getApplyPosition() const { return m_applyPosition; }
        ITF_INLINE bbool                getApplyRotation() const { return m_applyRotation; }
        ITF_INLINE bbool                getApplyScale() const { return m_applyScale; }
        const SafeArray<InputDesc>&     getInputs() const { return m_inputList; }
#ifndef ITF_FINAL
        ITF_INLINE f32      getDebugDuration() const { return m_debugDuration; }
#endif // ITF_FINAL

        // computed onTemplateLoaded
        ITF_INLINE u32                  getStartSetIndex() const { return m_startSetIndex; }
        const TriggableSetList&         getTriggableSets() const { return m_triggableSets; }

        virtual void onTemplateLoaded();
        void clear();
        u32 getSetIndex( const StringID& _setName ) const;

    private:

        InstructionSetList  m_instructionSets;
        bbool               m_syncWithMetronome;
        MetronomeType       m_metronome;
        f32                 m_syncOffset;
        StartMode           m_startMode;
        StringID            m_startSet;
        bbool               m_applyPosition;
        bbool               m_applyRotation;
        bbool               m_applyScale;
        SafeArray<InputDesc> m_inputList;
#ifndef ITF_FINAL
        f32                 m_debugDuration;
#endif // ITF_FINAL

        // computed onTemplateLoaded
        TriggableSetList    m_triggableSets;
        u32                 m_startSetIndex;
    };

    typedef TweenComponent_Template::InstructionSet InstructionSet_Template;
    typedef TweenComponent_Template::InstructionSetList InstructionSetList_Template;

    ITF_INLINE const class TweenComponent_Template* TweenComponent::getTemplate() const
    {
        return static_cast<const TweenComponent_Template*>(m_template);
    }

    template <typename T>
    ITF_INLINE void TweenComponent::setInput( StringID _inputName, T _value )
    {
        u32 numInputs = m_inputList.size();

        for ( u32 i = 0; i < numInputs; i++ )
        {
            Input& input = m_inputList[i];

            if ( input.getId() == _inputName )
            {
                input.setValue(_value);
                break;
            }
        }
    }

};

#endif //_ITF_TWEENCOMPONENT_H_