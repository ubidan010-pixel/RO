#ifndef _ITF_TWEENINTERPRETER_H_
#define _ITF_TWEENINTERPRETER_H_

#ifndef _ITF_TWEENINSTRUCTIONS_H_
#include "gameplay/Components/Common/TweenInstructions.h"
#endif //_ITF_TWEENINSTRUCTIONS_H_

namespace ITF
{
    class TweenInstructionSet_Template
    {
        DECLARE_SERIALIZE()

    public:
        TweenInstructionSet_Template();
        ~TweenInstructionSet_Template();

        void clear();   // TODO: doesn't need to be public
        void onTemplateLoaded();


        StringID                        getName() const { return m_name; }
        const InstructionList_Template& getInstructions() const { return m_instructions; }
        u32                             getIterationCount() const { return m_iterationCount; }

        // computed onTemplateLoaded
        ITF_INLINE f32                  getDuration() const { return m_duration; }
        ITF_INLINE f32                  getSyncRatio() const { return m_syncRatio; }
        ITF_INLINE bbool                getIsLooping() const { return m_isLooping; }

    private:

        StringID                m_name;
        InstructionList_Template m_instructions;
        u32                     m_iterationCount;

        // computed onTemplateLoaded
        f32     m_duration;
        f32     m_syncRatio;
        bbool   m_isLooping;
    };

    typedef ITF_VECTOR<TweenInstructionSet_Template> TweenInstructionSetList_Template;


    //---------------------------------------------------------------------------------------------------

    class TweenInterpreter_Template
    {
        DECLARE_SERIALIZE();

    public:
        TweenInterpreter_Template()
            : m_sync(btrue)
        {
        }

        void clear();
        void onTemplateLoaded();


        ITF_INLINE const TweenInstructionSetList_Template& getInstructionSets() const { return m_instructionSets; }
        ITF_INLINE bbool getSync() const { return m_sync; }

        u32 getSetIndex( const StringID& _setName ) const;

    private:

        TweenInstructionSetList_Template m_instructionSets;
        bbool m_sync;
    };


    //---------------------------------------------------------------------------------------------------

    class TweenInterpreter
    {
    public:
        TweenInterpreter()
            : m_instructionSet(NULL)
            , m_sync(btrue)
            , m_syncOffset(0.f)
            , m_timer(0.f)
        {
        }

        void clear()
        {
            m_instructionSet = NULL;
            m_sync = btrue;
            m_syncOffset = 0.f;
            m_timer = 0.f;
        }

        ITF_INLINE const TweenInstructionSet_Template* getInstructionSet() const { return m_instructionSet; }
        ITF_INLINE void setInstructionSet( const TweenInstructionSet_Template* _value ) { m_instructionSet = _value; }

        ITF_INLINE bbool getSync() const { return m_sync; }
        ITF_INLINE void setSync( const ITF::bbool _value ) { m_sync = _value; }

        ITF_INLINE f32 getSyncOffset() const { return m_syncOffset; }
        ITF_INLINE void setSyncOffset( const f32 _value ) { m_syncOffset = _value; }


        ITF_INLINE bbool isInitialized() const { return m_instructionSet != NULL; }
                   void update( const f32 _dt );
        ITF_INLINE const TweenCoordinates& getCurrentCoordinates() const { return m_currentCoordinates; }

    private:
        const TweenInstructionSet_Template* m_instructionSet;

        bbool m_sync;
        f32 m_syncOffset;
        f32 m_timer;

        TweenCoordinates m_currentCoordinates;   // coordinates at last frame
    };

};

#endif //_ITF_TWEENINTERPRETER_H_