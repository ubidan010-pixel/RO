#include "precompiled_engine.h"
#include "engine/sound/common/RoutingGroup.h"
#include "engine/sound/common/RoutingChild.h"





namespace ITF
{


    RoutingChild::RoutingChild()
        : m_routingGroup( NULL )
        , m_decibelVolume( 0.f )
        , m_needsVolumeUpdate( false )
    {
    }


    RoutingChild::~RoutingChild()
    {
    }


    void RoutingChild::updateVolume( bool _force )
    {
        bool needsVolumeUpdate = _force || m_needsVolumeUpdate;

        if ( m_routingGroup )
            needsVolumeUpdate |= m_routingGroup->childrenNeedVolumeUpdate();

        if ( needsVolumeUpdate )
        {
            f32 decibelVolume = m_decibelVolume;

            if ( m_routingGroup )
                decibelVolume += m_routingGroup->getDecibelVolume();

            internalSetDecibelVolume( decibelVolume );

            m_needsVolumeUpdate = false;
        }
    }


    void RoutingChild::setDecibelVolume( f32 _decibelVolume )
    {
        if ( _decibelVolume > 0.f )
            _decibelVolume = 0.f;

        if ( _decibelVolume < RAKI_ROUTING_MIN_VOLUME_DECIBELS )
            _decibelVolume = RAKI_ROUTING_MIN_VOLUME_DECIBELS;

        if ( !floatsAlmostEqual( m_decibelVolume, _decibelVolume ) )
        {
            m_decibelVolume = _decibelVolume;

            m_needsVolumeUpdate = true;
        }
    }


    void RoutingChild::setLinearVolume( f32 _linearVolume )
    {
        f32 decibelVolume;

        if ( _linearVolume > 1.f )
            _linearVolume = 1.f;

        if ( _linearVolume <= 0.f )
            decibelVolume = RAKI_ROUTING_MIN_VOLUME_DECIBELS;
        else
        {
            decibelVolume = getDecibelVolumeFromLinearVolume( _linearVolume );
        }

        if ( !floatsAlmostEqual( m_decibelVolume, decibelVolume ) )
        {
            m_decibelVolume = decibelVolume;

            m_needsVolumeUpdate = true;
        }
    }


    void RoutingChild::setRoutingGroup( RoutingGroup * _routingGroup )
    {
        m_routingGroup = _routingGroup;
    }





} // namespace ITF 

