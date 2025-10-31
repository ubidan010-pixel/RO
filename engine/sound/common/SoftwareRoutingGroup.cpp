#include "precompiled_engine.h"
#include "engine/sound/common/SoftwareRoutingGroup.h"



namespace ITF
{


    SoftwareRoutingGroup::SoftwareRoutingGroup()
        : m_decibelVolume( 0.f )
        , m_childrenNeedVolumeUpdate( false )
    {
    }


    SoftwareRoutingGroup::~SoftwareRoutingGroup()
    {
    }


    void SoftwareRoutingGroup::resetVolumeUpdateFlag()
    {
        m_childrenNeedVolumeUpdate = false;
    }


    void SoftwareRoutingGroup::internalSetDecibelVolume( f32 _decibelVolume )
    {
        m_decibelVolume = _decibelVolume;

        m_childrenNeedVolumeUpdate = true;
    }


    f32 SoftwareRoutingGroup::getDecibelVolume()
    {
        f32 decibelVolume = m_decibelVolume;

        if ( getParentGroup() )
            decibelVolume += getParentGroup()->getDecibelVolume();

        return decibelVolume;
    }


    bool SoftwareRoutingGroup::childrenNeedVolumeUpdate()
    {
        bool childrenNeedVolumeUpdate_ = m_childrenNeedVolumeUpdate;

        if ( !childrenNeedVolumeUpdate_ && getParentGroup() )
            childrenNeedVolumeUpdate_= getParentGroup()->childrenNeedVolumeUpdate();

        return childrenNeedVolumeUpdate_;
    }



} // namespace ITF 

