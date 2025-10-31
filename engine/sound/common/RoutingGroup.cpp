#include "precompiled_engine.h"
#include "engine/sound/common/RoutingGroup.h"
#include "engine/sound/common/RoutingChild.h"


namespace ITF
{


    RoutingGroup::RoutingGroup()
        : m_parentGroup( NULL )
        , m_fadeDuration( 0.f )
        , m_currentFadeTime( 0.f )
        , m_currentDecibelVolume( 0.f )
        , m_fadeBeginningDecibelVolume( 0.f )
        , m_fadeEndDecibelVolume( 0.f )
        , m_fading( false )
    {
    }


    RoutingGroup::~RoutingGroup()
    {
    }


    void RoutingGroup::setDecibelVolume( f32 _decibelVolume, f32 _fadeDurationSeconds )
    {
        if ( _decibelVolume >= 0.f )
            _decibelVolume = 0.f;

        if ( _decibelVolume < RAKI_ROUTING_MIN_VOLUME_DECIBELS )
            _decibelVolume = RAKI_ROUTING_MIN_VOLUME_DECIBELS;

        if ( ( _fadeDurationSeconds > 0.01f ) && ( _fadeDurationSeconds < 100.f ) )
        {
            m_fadeDuration = _fadeDurationSeconds;
            m_currentFadeTime = 0.f;

            m_fadeBeginningDecibelVolume = m_currentDecibelVolume;
            m_fadeEndDecibelVolume = _decibelVolume;

            m_fading = true;
        }
        else
        {
            m_currentDecibelVolume = _decibelVolume;

            internalSetDecibelVolume( _decibelVolume );

            m_fading = false;
        }
    }


    void RoutingGroup::updateFade( f32 _deltaTime )
    {
        if ( m_fading )
        {
            f32 decibelVolume;

            m_currentFadeTime += _deltaTime;

            if ( m_currentFadeTime >= m_fadeDuration )
            {
                // finished fade

                decibelVolume = m_fadeEndDecibelVolume;

                m_fadeDuration = 0.f;
                m_currentFadeTime = 0.f;

                m_fading = false;
            }
            else
            {
                decibelVolume = m_fadeBeginningDecibelVolume + ( m_fadeEndDecibelVolume - m_fadeBeginningDecibelVolume ) * ( m_currentFadeTime / m_fadeDuration );
            }

            //here do not call setDecibelVolume(decibelVolume) because it will break the current fade !
            //instead set directly new volume
            m_currentDecibelVolume = decibelVolume;
            internalSetDecibelVolume(decibelVolume);
        }
    }







} // namespace ITF 

