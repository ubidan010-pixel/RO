#ifndef _ITF_SOFTWAREBUS_H_
#define _ITF_SOFTWAREBUS_H_

#ifdef ITF_SUPPORT_RAKI
#ifndef _ITF_RAKIINCLUDES_
#include "engine/sound/raki/RakiIncludes.h"
#endif // _ITF_RAKIINCLUDES_

#ifndef RAKI_SOFTWAREROUTINGGROUP_H
#include "RakiEngine/Routing/SoftwareRoutingGroup.h"
#endif // RAKI_SOFTWAREROUTINGGROUP_H
#else
#include "engine/sound/common/SoftwareRoutingGroup.h"

#endif



namespace ITF
{


    struct BusDef;



    class SoftwareBus
#ifdef ITF_SUPPORT_RAKI
        : public raki::SoftwareRoutingGroup
#else
        : public ITF::SoftwareRoutingGroup
#endif
    {

    public:

        SoftwareBus( const BusDef * _busdef );
    
        virtual ~SoftwareBus();

        ITF_INLINE const BusDef * getBusDef();


    private:

        const BusDef * m_busdef;
    };



    const BusDef * SoftwareBus::getBusDef()
    {
        return m_busdef;
    }


} // namespace ITF

#endif // _ITF_SOFTWAREBUS_H_
