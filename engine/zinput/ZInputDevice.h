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
    enum EInputCategory
    {
        InputCategory_Gameplay = 0,
        InputCategory_Menu,
        InputCategory_Count
    };
    enum EInputSourceType
    {
        InputSource_Gamepad = 0,
        InputSource_Keyboard,
        InputSource_Count
    };

#define REGISTER_CONTROL(control,controlId)     m_controlMap[controlId] = control;
typedef ITF_MAP<StringID,int>     ControlTranslateMap;

#ifdef ITF_WII
    #define PADTYPE_TO_FLAG(type)   (1 << (type))
#endif // ITF_WII

    class IInputDevice
    {
    public:
        struct SDeviceInfo
        {
            ITF_VECTOR<SInputInfo>  m_inputInfo;
        };

#ifdef ITF_WII
        IInputDevice(u32 id, u32 _ValidType): m_id(id), m_initialized(bfalse), m_ValidType(_ValidType) { }
#else // ITF_WII
        IInputDevice(u32 id): m_id(id), m_initialized(bfalse) { }
#endif // ITF_WII
        virtual ~IInputDevice() {}

        void    Update( );
        void    ProcessActions ( ActionMap * actionMap );
        void    ProcessActionsWithCategory ( ActionMap * actionMap, EInputCategory category );
        
        ITF_INLINE bbool IsDeviceValid() const
        {
            return IsSourceAllowed() && INPUT_ADAPTER->isPadConnected(m_id)
#ifdef ITF_WII
                && (PADTYPE_TO_FLAG(INPUT_ADAPTER->getPadType(m_id)) & m_ValidType)
#endif // ITF_WII
                ;
        }

        ITF_INLINE u32 GetId() const { return m_id; }

        virtual EInputSourceType GetInputSourceType() const { return InputSource_Gamepad; }

        const StringID & getSpecificConfig() const { return m_specificConfig; }
        virtual void        SetRemap(u32 logicalControl, u32 physicalControl);
        virtual void        ResetRemapping();
        void                ApplyRemapping(SDeviceInfo& deviceInfo);
        virtual u32         GetRemap(u32 logicalControl) const;
        virtual u32         GetFirstActiveControl() const;

    protected:
        
        void    ImplUpdate();
        void    ImplProcessActions( ActionMap& actionMap );
        void    ImplProcessActionsWithCategory( ActionMap& actionMap, EInputCategory category );

        virtual void   InitDeviceInfo()= 0;
        virtual void   UpdateDeviceInfo( SDeviceInfo& deviceInfo )=0;
        virtual void   UpdateDeviceInfoRaw( SDeviceInfo& deviceInfo ) { UpdateDeviceInfo(deviceInfo); }


        SDeviceInfo                 m_deviceInfo;          
        SDeviceInfo                 m_deviceInfoRaw;       
        u32                         m_id;
        ControlTranslateMap         m_controlMap;
        bbool                       m_initialized;
        StringID                    m_specificConfig;
        
        ITF_VECTOR<u32>             m_controlRemap;

#ifdef ITF_WII
        u32                         m_ValidType;
#endif // ITF_WII

        // Helpers
        static u32 TranslateControl( const StringID& control, const ControlTranslateMap& map );

        virtual bbool IsSourceAllowed() const { return btrue; }
    };

}//namespace ITF

#endif // _ITF_INPUTDEVICE_H_
