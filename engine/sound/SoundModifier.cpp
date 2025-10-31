#include "precompiled_engine.h"


#ifndef _ITF_SOUNDMODIFIER_H_
#include "engine/sound/SoundModifier.h"
#endif //_ITF_SOUNDMODIFIER_H_

#ifndef _ITF_SOUNDINSTANCE_H_
#include "engine/sound/SoundInstance.h"
#endif //_ITF_SOUNDINSTANCE_H_

#ifndef _ITF_SOUNDMANAGER_H_
#include "engine/sound/SoundManager.h"
#endif //_ITF_SOUNDMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(SoundModifier)
    IMPLEMENT_OBJECT_RTTI(ScreenRollOff)
    IMPLEMENT_OBJECT_RTTI(ScreenRollOffXY)
    IMPLEMENT_OBJECT_RTTI(ZRollOff)
    IMPLEMENT_OBJECT_RTTI(ControlVolume)
    IMPLEMENT_OBJECT_RTTI(ControlPitch)
    IMPLEMENT_OBJECT_RTTI(SpatializedPanning)
    IMPLEMENT_OBJECT_RTTI(Doppler)
    IMPLEMENT_OBJECT_RTTI(ControlFilter)

    BEGIN_SERIALIZATION_ABSTRACT(SoundModifier)
    END_SERIALIZATION()
    BEGIN_VALIDATE_OBJECT(SoundModifier)
    END_VALIDATE_OBJECT()

    BEGIN_SERIALIZATION(ScreenRollOff)
        SERIALIZE_MEMBER("distanceMin",m_distanceMin);
        SERIALIZE_MEMBER("distanceMax",m_distanceMax);
    END_SERIALIZATION()
    BEGIN_VALIDATE_OBJECT(ScreenRollOff)
    VALIDATE_PARAM("distanceMin",m_distanceMin < m_distanceMax,"distanceMin must be smaller than distance max");
    END_VALIDATE_OBJECT()

    BEGIN_SERIALIZATION(ScreenRollOffXY)
        SERIALIZE_MEMBER("distanceMinX",m_distanceMinX);
        SERIALIZE_MEMBER("distanceMaxX",m_distanceMaxX);
        SERIALIZE_MEMBER("distanceMinY",m_distanceMinY);
        SERIALIZE_MEMBER("distanceMaxY",m_distanceMaxY);
        SERIALIZE_MEMBER("useX",m_useX);
        SERIALIZE_MEMBER("useY",m_useY);
    END_SERIALIZATION()
    BEGIN_VALIDATE_OBJECT(ScreenRollOffXY)
    VALIDATE_PARAM("distanceMinX",m_distanceMinX < m_distanceMaxX,"distanceMin X must be smaller than distance max");
    VALIDATE_PARAM("distanceMinY",m_distanceMinY < m_distanceMaxY,"distanceMin Y must be smaller than distance max");
    VALIDATE_PARAM("useX",m_useX || m_useY,"you must use X or Y");
    END_VALIDATE_OBJECT()


    BEGIN_SERIALIZATION(ZRollOff)
        SERIALIZE_MEMBER("distanceMin",m_distanceMin);
        SERIALIZE_MEMBER("distanceMax",m_distanceMax);
    END_SERIALIZATION()
    BEGIN_VALIDATE_OBJECT(ZRollOff)
    VALIDATE_PARAM("distanceMin",m_distanceMin < m_distanceMax,"distanceMin must be smaller than distance max");
    END_VALIDATE_OBJECT()

        
    BEGIN_SERIALIZATION(ControlVolume)
        SERIALIZE_OBJECT("input",m_input);
    END_SERIALIZATION()
    BEGIN_VALIDATE_OBJECT(ControlVolume)
    END_VALIDATE_OBJECT()

    BEGIN_SERIALIZATION(ControlPitch)
        SERIALIZE_OBJECT("input",m_input);
    END_SERIALIZATION()
    BEGIN_VALIDATE_OBJECT(ControlPitch)
    END_VALIDATE_OBJECT()

        
    BEGIN_SERIALIZATION(SpatializedPanning)
        SERIALIZE_MEMBER("widthMin",m_widthMin);
        SERIALIZE_MEMBER("widthMax",m_widthMax);
    END_SERIALIZATION()
    BEGIN_VALIDATE_OBJECT(SpatializedPanning)
    VALIDATE_PARAM("distanceMin",m_widthMin < m_widthMax,"widthMin must be smaller than widthMax");
    VALIDATE_PARAM("widthMin",m_widthMin > 0.f,"widthMin must be postive");
    VALIDATE_PARAM("widthMax",m_widthMax > 0.f,"widthMax must be postive");
    END_VALIDATE_OBJECT()

    BEGIN_SERIALIZATION(Doppler)
        SERIALIZE_MEMBER("factor",m_factor);
    END_SERIALIZATION()
    BEGIN_VALIDATE_OBJECT(Doppler)
    END_VALIDATE_OBJECT()

    BEGIN_SERIALIZATION(ControlFilter)
        SERIALIZE_OBJECT("input",m_input);
    END_SERIALIZATION()
    BEGIN_VALIDATE_OBJECT(ControlFilter)
    END_VALIDATE_OBJECT()


        //************************************
    // @name	ITF::ScreenRollOff::getVolume
    // @public 
    // @param	SoundInstance * _instance
    // @param	f32 _volume
    // @return	ITF::f32
    //************************************
    f32 ScreenRollOff::getVolume(const SoundInstance * _instance, f32 /*_volume*/) const
    {
        ITF_ASSERT(_instance);
        ITF_ASSERT(m_distanceMax != 0.0f);
        ITF_ASSERT(m_distanceMax != m_distanceMin);

        // 2 D ROLL OFF
        f32 twoOverScreenWidth = 2.0f/(f32)GFX_ADAPTER->getScreenWidth();

        Vec3d pos2d;
        GFX_ADAPTER->compute3DTo2D(_instance->getPos(),pos2d);

        pos2d.m_z = 0.0f;
        pos2d.m_x = (pos2d.m_x * twoOverScreenWidth) -1.0f;
        pos2d.m_y = (pos2d.m_y * twoOverScreenWidth) -1.0f;
        f32 distanceToMic = pos2d.norm();

        f32 fadedVolume = 1.0f - Clamp((distanceToMic - m_distanceMin) / (m_distanceMax - m_distanceMin),0.0f,1.0f);
        f32 newVolume = distanceToMic > m_distanceMin ? fadedVolume:1.0f;

        ITF_ASSERT( ( newVolume >= 0 ) && ( newVolume <= 1.f ) );

        return newVolume;
    }

            //************************************
    // @name	ITF::ScreenRollOff::getVolume
    // @public 
    // @param	SoundInstance * _instance
    // @param	f32 _volume
    // @return	ITF::f32
    //************************************
    f32 ScreenRollOffXY::getVolume(const SoundInstance * _instance, f32 /*_volume*/ ) const
    {
        ITF_ASSERT(_instance);
        ITF_ASSERT(m_distanceMaxX != 0.0f);
        ITF_ASSERT(m_distanceMaxX != m_distanceMinX);
        ITF_ASSERT(m_distanceMaxY != 0.0f);
        ITF_ASSERT(m_distanceMaxY != m_distanceMinY);

        // 2 D ROLL OFF
        f32 twoOverScreenWidth = 2.0f/(f32)GFX_ADAPTER->getScreenWidth();
        f32 twoOverScreenHeight = 2.0f/(f32)GFX_ADAPTER->getScreenHeight();

        Vec3d pos2d;
        GFX_ADAPTER->compute3DTo2D(_instance->getPos(),pos2d);

        pos2d.m_z = 0.0f;
        pos2d.m_x = (pos2d.m_x * twoOverScreenWidth) -1.0f;
        pos2d.m_y = (pos2d.m_y * twoOverScreenHeight) -1.0f;

        f32 xVolume = 1.0f;
        f32 yVolume = 1.0f;

        if (m_useX)
        {
            f32 fadedVolumeX = 1.0f - Clamp((f32_Abs(pos2d.m_x) - m_distanceMinX)/(m_distanceMaxX - m_distanceMinX),0.0f,1.0f);
            xVolume = f32_Abs(pos2d.m_x) > m_distanceMinX ? fadedVolumeX:1.0f;
        }

        if (m_useY)
        {
            f32 fadedVolumeY = 1.0f - Clamp((f32_Abs(pos2d.m_y) - m_distanceMinY)/(m_distanceMaxY - m_distanceMinY),0.0f,1.0f);
            yVolume = f32_Abs(pos2d.m_y) > m_distanceMinY ? fadedVolumeY:1.0f;
        }

        f32 result = 0.5f * (xVolume + yVolume);
        
        ITF_ASSERT( ( result >= 0 ) && ( result <= 1.f ) );

        return result;
    }
    //************************************
    // @name	ITF::ZRollOff::getVolume
    // @public 
    // @param	SoundInstance * _instance
    // @param	f32 _volume
    // @return	ITF::f32
    //************************************
    f32 ZRollOff::getVolume(const SoundInstance * _instance, f32 /*_volume*/ ) const
    {
        ITF_ASSERT(_instance);
        ITF_ASSERT(m_distanceMax != 0.0f);
        ITF_ASSERT(m_distanceMax != m_distanceMin);

        f32 zDistanceToMic = f32_Abs(_instance->getPos().m_z - _instance->getMicPos().m_z);
        f32 zfadedVolume = 1.0f - Clamp((zDistanceToMic - m_distanceMin)/(m_distanceMax - m_distanceMin),0.0f,1.0f);
        f32 newVolume = zDistanceToMic > m_distanceMin ? zfadedVolume:1.0f;
        ITF_ASSERT( ( newVolume >= 0 ) && ( newVolume <= 1.f ) );
        return newVolume;
    }

    //************************************
    // @name	ITF::ControlVolume::getVolume
    // @public 
    // @param	SoundInstance * _instance
    // @param	f32 _volume
    // @return	ITF::f32
    //************************************
    f32 ControlVolume::getVolume(const SoundInstance * _instance, f32 _volume) const
    {
        ITF_ASSERT(_instance);
        f32 volume = Volume::DecibelsToAmplitudeRatio(m_input.getValue( _volume, 0.0f, _instance->getInputs() ));

        ITF_ASSERT( ( volume >= 0 ) && ( volume <= 1.f ) );

        return volume;
    }

    //************************************
    // @name	ITF::ControlPitch::getPitch
    // @public 
    // @param	SoundInstance * _instance
    // @param	f32 _pitch
    // @return	ITF::f32
    //************************************
    f32 ControlPitch::getPitch(const SoundInstance * _instance, f32 _pitch) const
    {
        ITF_ASSERT(_instance);
        return m_input.getValue( _pitch, 0.0f, _instance->getInputs() );
    }

    //************************************
    // @name	ITF::SpatializedPanning::getPan
    // @public 
    // @param	SoundInstance * _instance
    // @return	f32
    //************************************
    f32 SpatializedPanning::getPan( const SoundInstance * _instance ) const
    {
        ITF_ASSERT(_instance);

        f32 pan = 0.f;

        f32 twoOverScreenWidth = 2.0f/(f32)GFX_ADAPTER->getScreenWidth();

        Vec3d pos2d;
        GFX_ADAPTER->compute3DTo2D(_instance->getPos(),pos2d);

        f32 screenPos = pos2d.m_x;
        screenPos *= twoOverScreenWidth;
        screenPos -= 1.0f;

        if ( screenPos >= m_widthMax )
        {
            pan = 1.f;
        }
        else if ( screenPos <= -m_widthMax )
        {
            pan = -1.f;
        }
        else if ( ( screenPos >= -m_widthMin ) && ( screenPos <= m_widthMin ) )
        {
            pan = 0.f;
        }
        else if ( screenPos > m_widthMin )
        {
            pan = ( screenPos - m_widthMin ) / ( m_widthMax - m_widthMin );
        }
        else 
        {
            pan = ( screenPos + m_widthMin ) / ( m_widthMax - m_widthMin );
        }

        return pan;
    }

    //************************************
    // @name	ITF::Doppler::getPitch
    // @public 
    // @param	SoundInstance * _instance
    // @param	f32 _pitch
    // @return	ITF::f32
    //************************************
    f32 Doppler::getPitch(const SoundInstance * _instance, f32 _pitch) const
    {
        ITF_ASSERT(_instance);
        Vec3d pos = _instance->getPos();
        Vec3d micPos = _instance->getMicPos();
        Vec3d speed = _instance->getSpeed();
        Vec3d micSpeed = _instance->getMicSpeed();

        Vec3d dir = pos - micPos;
        dir.normalize();

        f32 speedN = speed.dot(dir) - micSpeed.dot(dir);
        f32 pitch = ( 1.0f - (speedN* m_factor)/ 343.2f )  ;

        return _pitch * pitch;
    }

    //************************************
    // @name	ITF::ControlFilter::getFilterFrequency
    // @public 
    // @param	SoundInstance * _instance
    // @param	FilterType _filterType
    // @param	f32 _filterFrequency
    // @param	f32 _filterQ
    // @return	ITF::f32
    //************************************
    f32 ControlFilter::getFilterFrequency(const SoundInstance * _instance, FilterType _filterType, f32 _filterFrequency, f32 _filterQ) const
    {
        ITF_ASSERT(_instance);
        return m_input.getValue( _filterFrequency, 0.0f, _instance->getInputs() );
    }

}


