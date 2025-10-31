
#ifndef _ITF_FXBANKCOMPONENT_H_
#define _ITF_FXBANKCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "../actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_SEEDER_H_
#include "core/Seeder.h"
#endif //_ITF_SEEDER_H_

#ifndef _ITF_SOUND_ADAPTER_
#include "engine/AdaptersInterfaces/SoundAdapter.h"
#endif //_ITF_SOUND_ADAPTER_

#ifndef _ITF_RESOURCE_H_   
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_   

#ifndef _ITF_PARTICULEGENERATOR_H_
#include "engine/display/particle/ParticuleGenerator.h"
#endif //_ITF_PARTICULEGENERATOR_H_

#ifndef _ITF_FXDESCRIPTOR_H_
#include "engine/display/FxDescriptor.h"
#endif //_ITF_FXDESCRIPTOR_H_

#ifndef _ITF_FEEDBACKFXMANAGER_H_
#include "engine/actors/managers/FeedbackFXManager.h"
#endif //_ITF_FEEDBACKFXMANAGER_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_


#define MAX_NUM_FX_INSTANCES 20

namespace ITF
{
    class ITF_ParticleGenerator;

    struct FxInstance
    {
        FxInstance()
            : m_gen(NULL)
            , m_playMode(0)
            , m_generatorIndexInPool(-1)
         {
             clear();
         }
        enum InstanceState
        {
            InstanceState_Unused,
            InstanceState_Playing,
            InstanceState_Stopping,
            ENUM_FORCE_SIZE_32(0)
        };
        void clear();
        ITF_ParticleGenerator*  m_gen;
        FxDescriptor *          m_descriptor;
        InstanceState           m_state;
        bbool                   m_emitFromBase;
        bbool                   m_useActorSpeed;
        bbool                   m_useActorOrientation;
        f32                     m_angle;
        u32                     m_boneIndex;
        bbool                   m_useBoneOrientation;
        Vec3d                   m_pos;
        f32                     m_timer;
        StringID                m_name;
        u32                     m_playMode;
        i32                     m_generatorIndexInPool;
    };

    class FxBankComponent : public GraphicComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(FxBankComponent,GraphicComponent,2523615645);

    public:
        DECLARE_SERIALIZE()

        FxBankComponent();
        ~FxBankComponent();
        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const { return !m_draw2D; }
        virtual bbool needsDraw2D() const { return m_draw2D; }
        virtual bbool keepAlive();

		virtual void    onBecomeInactive();

        virtual void Draw();
        virtual void Draw2D();
        void addLight(const LightInfo &_light);
        virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void Update( f32 _deltaTime );

        virtual void onEvent(Event * _event);

        u32 playFx( StringID _tag, u32 _boneIndex = U32_INVALID, bbool _useBoneOrientation = bfalse, bbool _emitFromBase = bfalse, bbool _useActorSpeed = btrue, bbool _useActorOrientation = bfalse, f32 _lifeTimeMult = 1.0f, const FxDescriptor_Template * _FxDesc = NULL);
        void stopFx(u32 _index);
        void setLifeTimeMultiplier(u32 _index, f32 _multiplier);
        FxInstance * getInstance(u32 _index) const 
        { 
            ITF_ASSERT(_index != U32_INVALID);
            ITF_ASSERT(_index < m_instance.size());
            ITF_ASSERT(m_instance[_index].m_gen != NULL);
            return (FxInstance*)&m_instance[_index];
        }
        bbool isPlaying(u32 _index) const;

        template <typename T>
        void                        setInput(StringID _inputName, T _value );
        const InputContainer &      getInputs() const { return m_inputList; }
        void                        setUpdateFxInput( IUpdateFxInputCriteria* _update ) { m_fxInputUpdate = _update; }
        u32                         getNumInputs() const { return m_inputList.size();}
#ifdef ITF_DEBUG_STRINGID
        const char*                 getInputName( u32 _index ) const { return m_inputList[_index].getName(); }
#endif
        InputType                   getInputType( u32 _index ) const { return m_inputList[_index].getType(); }

        template <typename T>
        void                        getInputValue( u32 _index, T& _val ) const
        {
            const Input& input = m_inputList[_index];
            input.getValue(_val);
        }

        ITF_ParticleGenerator*      getParticleGenerator(u32 _index);
        const StringID&             getInstanceName(u32 _index);
        void                        setFXPos(u32 _index, const Vec3d& _pos);
        void                        setFXAngle(u32 _index, f32 _angle);
        void                        attachToBone(u32 _index, u32 _boneIndex);

    protected:
        void clearInstance(u32 _index);

    private:

		ITF_INLINE const class FxBankComponent_Template*  getTemplate() const;

        void clear();
        void computeDraw2D();

        void updateInstance( FxInstance &pFxInstance, f32 _deltaTime );

        void updateAABB( FxInstance &_instance );

        void setInputs( FxInstance &_instance, f32 _deltaTime );

        void setPosAndAngle( FxInstance &_instance );

        typedef ITF_MAP<StringID,u32>             FxMap;
        typedef ITF_VECTOR<FxDescriptor>          FxList;
                
        ITF_VECTOR<FxInstance>                  m_instance;

        FxMap                       m_Fx;
        FxList                      m_FxList;
        Seeder                      m_random;
        bbool                       m_draw2D;

        IUpdateFxInputCriteria*     m_fxInputUpdate;
        InputContainer              m_inputList;

		// Used to lock slots on feedbackFXManager
		ITF_MAP<StringID,FxDescriptor *> m_feedbackFxs;
    };

    template <typename T>
    ITF_INLINE void FxBankComponent::setInput(StringID _inputName, T _value )
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

    class FxBankComponent_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(FxBankComponent_Template,GraphicComponent_Template,12983557);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(FxBankComponent);

    public:

        FxBankComponent_Template();
        ~FxBankComponent_Template();

        typedef ITF_VECTOR<FxDescriptor_Template>          FxList;

		virtual void		onTemplateLoaded();

        ITF_INLINE const FxList&		        getFxList() const { return m_FxList; }
        ITF_INLINE const SafeArray<InputDesc>&  getInputList() const { return m_inputList; }

    private:

        FxList                          m_FxList;
        SafeArray <InputDesc>           m_inputList;
    };

    ITF_INLINE const FxBankComponent_Template* FxBankComponent::getTemplate() const
    {
        return static_cast<const FxBankComponent_Template*>(m_template);
    }
}

#endif // _ITF_FXBANKCOMPONENT_H_

