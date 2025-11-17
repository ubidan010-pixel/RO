#ifndef _ITF_INPUTDEVICE_H_
#define _ITF_INPUTDEVICE_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_ACTIONMAP_H_
#include "engine/zinput/ZActionMap.h"
#endif //_ITF_ACTIONMAP_H_

namespace ITF
{

#define REGISTER_CONTROL(control,controlId)     m_controlMap[controlId] = control;
typedef ITF_MAP<StringID,int>     ControlTranslateMap;

#ifdef ITF_WII
    #define PADTYPE_TO_FLAG(type)   (1 << (type))
#endif // ITF_WII

    class IInputDevice
    {
    public:

#ifdef ITF_WII
        IInputDevice(u32 id, u32 _ValidType): m_id(id), m_initialized(bfalse), m_ValidType(_ValidType) { }
#else // ITF_WII
        IInputDevice(u32 id): m_id(id), m_initialized(bfalse) { }
#endif // ITF_WII
        virtual ~IInputDevice() {}

        void    Update( );
        void    ProcessActions ( ActionMap * actionMap );
        
        ITF_INLINE virtual bbool IsDeviceValid() const
        {
            return INPUT_ADAPTER->isPadConnected(m_id)
#ifdef ITF_WII
                && (PADTYPE_TO_FLAG(INPUT_ADAPTER->getPadType(m_id)) & m_ValidType)
#endif // ITF_WII
                ;
        }

        const StringID & getSpecificConfig() const { return m_specificConfig; }

    protected:
        
        void    ImplUpdate();
        void    ImplProcessActions( ActionMap& actionMap );

        struct SDeviceInfo
        {
            ITF_VECTOR<SInputInfo>  m_inputInfo;
        };

        virtual void   InitDeviceInfo()= 0;
        virtual void   UpdateDeviceInfo( SDeviceInfo& deviceInfo )=0;


        SDeviceInfo                 m_deviceInfo;
        u32                         m_id;
        ControlTranslateMap         m_controlMap;
        bbool                       m_initialized;
        StringID                    m_specificConfig;

#ifdef ITF_WII
        u32                         m_ValidType;
#endif // ITF_WII

        // Helpers
        static u32 TranslateControl( const StringID& control, const ControlTranslateMap& map );
    };

}//namespace ITF

#endif // _ITF_INPUTDEVICE_H_
