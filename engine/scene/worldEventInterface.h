#ifndef _ITF_WORLD_EVENT_INTERFACE_H_
#define _ITF_WORLD_EVENT_INTERFACE_H_

#ifndef _ITF_WORLD_H_
#include "engine/scene/world.h"
#endif //_ITF_WORLD_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_


namespace ITF
{

    /*
    WARNING: All interface classes must have only pure virtual methods and no variable
    */
    class Interface_WorldEventsListener
    {
        public:
            virtual void onSwitchToWorld(World* /*_pWorld*/, bbool /*_bSwitchDone*/) = 0;
            virtual void onDeleteWorld(World* /*_pWorld*/) = 0;
            virtual void onStartDrawProcess() = 0;
            virtual void onEndDrawProcess() = 0;
            /// called before updates 
            virtual void onStartUpdateProcess(f32 /*_dt*/) = 0;    
            /// called after updates 
            virtual void onEndUpdateProcess(f32 /*_dt*/) = 0;
            /// called during updates 
            virtual void onUpdateProcess(f32 /*_dt*/) = 0;
    };

} // namespace ITF

#endif // _ITF_WORLD_EVENT_INTERFACE_H_