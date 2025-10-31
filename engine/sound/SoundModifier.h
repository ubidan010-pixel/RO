#ifndef _ITF_SOUNDMODIFIER_H_
#define _ITF_SOUNDMODIFIER_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_SOUND_ADAPTER_
#include "engine/AdaptersInterfaces/SoundAdapter.h"
#endif //_ITF_SOUND_ADAPTER_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

namespace ITF
{
    class SoundInstance;

    class SoundModifier : public IRTTIObject
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(SoundModifier,IRTTIObject,1934625813);
        DECLARE_SERIALIZE_VIRTUAL()
        DECLARE_VALIDATE_COMPONENT()
    public:
        SoundModifier(){}
        //************************************
        // @name	ITF::SoundModifier::getVolume
        // @virtual public 
        // @param	SoundInstance * _instance
        // @param	f32 _volume
        // @return	ITF::f32
        //************************************
        virtual f32     getVolume(const SoundInstance * _instance, f32 _volume) const { return _volume; }
        //************************************
        // @name	ITF::SoundModifier::getPitch
        // @virtual public 
        // @param	SoundInstance * _instance
        // @param	f32 _pitch
        // @return	ITF::f32
        //************************************
        virtual f32     getPitch(const SoundInstance * _instance, f32 _pitch) const { return _pitch; }
        //************************************
        // @name	ITF::SoundModifier::getPan
        // @virtual public 
        // @param	SoundInstance * _instance
        // @return	f32
        //************************************
        virtual f32    getPan(const SoundInstance * _instance) const { return 0.f; }
        //************************************
        // @name	ITF::SoundModifier::getFilterFrequency
        // @virtual public 
        // @param	SoundInstance * _instance
        // @param	FilterType _filterType
        // @param	f32 _filterFrequency
        // @param	f32 _filterQ
        // @return	ITF::f32
        //************************************
        virtual f32     getFilterFrequency(const SoundInstance * _instance, FilterType _filterType, f32 _filterFrequency, f32 _filterQ) const { return _filterFrequency; }
        //************************************
        // @name	ITF::SoundModifier::usePitch
        // @virtual public 
        // @return	ITF::bbool
        //************************************
        virtual bbool   usePitch() const = 0;
        //************************************
        // @name	ITF::SoundModifier::useFilter
        // @virtual public 
        // @return	ITF::bbool
        //************************************
        virtual bbool   useFilter() const = 0;
    protected:
    };

    class ScreenRollOff : public SoundModifier
    {
        DECLARE_OBJECT_CHILD_RTTI(ScreenRollOff,SoundModifier,1323053104);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()
    public:
        ScreenRollOff() : m_distanceMin(0.0f), m_distanceMax(2.0f) {}
        f32 getVolume(const SoundInstance * _instance, f32 _volume) const ;
        bbool   usePitch() const { return bfalse; }
        bbool   useFilter() const { return bfalse; }
    protected:
        f32 m_distanceMin;
        f32 m_distanceMax;
    };

    class ScreenRollOffXY : public SoundModifier
    {
        DECLARE_OBJECT_CHILD_RTTI(ScreenRollOffXY, SoundModifier,1821326457);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()
    public:
        ScreenRollOffXY() 
            : m_distanceMinX(0.0f)
            , m_distanceMaxX(2.0f) 
            , m_distanceMinY(0.0f)
            , m_distanceMaxY(2.0f) 
            , m_useX(btrue)
            , m_useY(btrue)
        {}
        f32 getVolume(const SoundInstance * _instance, f32 _volume) const ;
        bbool   usePitch() const { return bfalse; }
        bbool   useFilter() const { return bfalse; }
    protected:
        bbool   m_useX;
        bbool   m_useY;
        f32 m_distanceMinX;
        f32 m_distanceMaxX;
        f32 m_distanceMinY;
        f32 m_distanceMaxY;
    };
    class ZRollOff : public SoundModifier
    {
        DECLARE_OBJECT_CHILD_RTTI(ZRollOff, SoundModifier,3667597554);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()
    public:
        f32     getVolume(const SoundInstance * _instance, f32 _volume) const ;
        bbool   usePitch() const { return bfalse; }
        bbool   useFilter() const { return bfalse; }
    protected:
        f32 m_distanceMax;
        f32 m_distanceMin;
    };

    class ControlVolume : public SoundModifier
    {
        DECLARE_OBJECT_CHILD_RTTI(ControlVolume, SoundModifier,3496911000);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()
    public:
        f32     getVolume(const SoundInstance * _instance, f32 _volume) const ;
        bbool   usePitch() const { return bfalse; }
        bbool   useFilter() const { return bfalse; }
    protected:
        ProceduralInputData m_input;
    };

    class ControlPitch : public SoundModifier
    {
        DECLARE_OBJECT_CHILD_RTTI(ControlPitch, SoundModifier,2612890702);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()
    public:
        f32     getPitch(const SoundInstance * _instance, f32 _pitch) const ;
        bbool   usePitch() const { return btrue; }
        bbool   useFilter() const { return bfalse; }
    protected:
        ProceduralInputData m_input;
    };

    class SpatializedPanning : public SoundModifier
    {
        DECLARE_OBJECT_CHILD_RTTI(SpatializedPanning,SoundModifier,1763862685);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()
    public:
        SpatializedPanning() 
            : m_widthMin(0.5f)
            , m_widthMax(2.0f)
        {
        }
        virtual f32 getPan(const SoundInstance * _instance) const;
        bbool   usePitch() const { return bfalse; }
        bbool   useFilter() const { return bfalse; }
    private:
        f32     m_widthMin;
        f32     m_widthMax;
    };

    class Doppler : public SoundModifier
    {
        DECLARE_OBJECT_CHILD_RTTI(Doppler, SoundModifier,4039480662);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()
    public:
        Doppler()
            : m_factor(1.0f)
        {}
        f32     getPitch(const SoundInstance * _instance, f32 _pitch) const;
        bbool   usePitch() const { return btrue; }
        bbool   useFilter() const { return bfalse; }
    private:
        f32     m_factor;
    };

    class ControlFilter : public SoundModifier
    {
        DECLARE_OBJECT_CHILD_RTTI(ControlFilter, SoundModifier,1892405494);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()
    public:
        bbool   usePitch() const { return bfalse; }
        bbool   useFilter() const { return btrue; }
        f32     getFilterFrequency(const SoundInstance * _instance, FilterType _filterType, f32 _filterFrequency, f32 _filterQ) const;
    private:
        ProceduralInputData m_input;
    };


}

#endif // _ITF_SOUNDMODIFIER_H_

