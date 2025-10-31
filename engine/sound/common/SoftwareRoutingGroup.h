#ifndef ITF_SOFTWAREROUTINGGROUP_H
#define ITF_SOFTWAREROUTINGGROUP_H




#include "engine/sound/common/RoutingGroup.h"





namespace ITF
{

    class SoftwareRoutingGroup 
        : public RoutingGroup
    {
    public:

        SoftwareRoutingGroup();

        virtual ~SoftwareRoutingGroup(); 

        virtual void resetVolumeUpdateFlag();

        virtual void internalSetDecibelVolume( f32 _decibelVolume );

        virtual f32 getDecibelVolume();

        virtual bool childrenNeedVolumeUpdate();

    private:

        f32 m_decibelVolume;

        bool m_childrenNeedVolumeUpdate;

    };



} // namespace ITF




#endif 


