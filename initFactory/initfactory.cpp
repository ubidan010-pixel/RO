#include "precompiled_initfactory.h"

#include "initfactory.h"

#include "editor/Editor.h"

#include "core/script/LUAHandler.h"

#include "engine/resources/ResourceManager.h"

#include "adapters/CodecImageAdapter_FreeImage/freeImageCodec_ITF.h"

// TCP Adapter
#include "engine/AdaptersInterfaces/TCPAdapter.h"
#include "adapters/TCPAdapter_ITF/TCPAdapter_ITF.h"

// Sound Adapter
#include "adapters/SoundAdapter_XAudio/SoundAdapter_XAudio.h"
#include "adapters/SoundAdapter_CTR/SoundAdapter_CTR.h"
#ifdef ITF_PS3
#include "adapters/SoundAdapter_PS3/SoundAdapter_PS3.h"
#endif // ITF_PS3
#include "adapters/SoundAdapter_Dummy/SoundAdapter_Dummy.h"
#include "adapters/Adapter_WWISE/Adapter_WWISE.h"


// System Adapter
#include "adapters/SystemAdapter_win/SystemAdapter_win.h"
#include "adapters/SystemAdapter_x360/SystemAdapter_x360.h"
#include "adapters/SystemAdapter_PS3/SystemAdapter_PS3.h"
#ifdef ITF_PS5
#include "adapters/SystemAdapter_PS5/SystemAdapter_PS5.h"
#endif // ITF_PS5
#ifdef ITF_NINTENDO
#include "adapters/SystemAdapter_Nintendo/SystemAdapter_Nintendo.h"
#endif // ITF_NINTENDO

#ifdef ITF_XBOX_SERIES
#include "adapters/SystemAdapter_XBoxSeries/SystemAdapter_XBoxSeries.h"
#endif // ITF_XBOX_SERIES

#include "adapters/SystemAdapter_Dummy/SystemAdapter_Dummy.h"

// GFX Adapter
#include "adapters/GFXAdapter_Directx9/GFXAdapter_Directx9.h"

#ifdef ITF_PS3
#include "adapters/GFXAdapter_GCM/GFXAdapter_GCM.h"
#endif // ITF_PS3

#ifdef ITF_PS5
#include "adapters/GFXAdapter_PS5/GFXAdapter_PS5.h"
#endif

#ifdef ITF_XBOX_SERIES
#include "adapters/GFXAdapter_DX12/GFXAdapter_DX12.h"
#endif

#include "adapters/GFXAdapter_Dummy/GFXAdapter_Dummy.h"
#ifdef ITF_NINTENDO
#include "adapters/GFXAdapter_NVN/GFXAdapter_NVN.h"
#endif

// File Manager
#include "adapters/FileManager_ITF/x360/FileManager_ITF_X360.h"
#include "adapters/FileManager_ITF/FileManager_ITF_Remote.h"
#include "adapters/FileManager_ITF/PS3/FileManager_ITF_PS3.h"
#include "adapters/FileManager_ITF/iPad/FileManager_ITF_iPad.h"
#include "adapters/FileManager_ITF/Win/FileManager_ITF_win.h"
#include "adapters/FileManager_ITF/CTR/FileManager_ITF_CTR.h"
#include "adapters/FileManager_ITF/WII/FileManager_ITF_WII.h"

// Thread Manager
#include "adapters/ITF_ThreadManager/ITF_ThreadManager.h"


// Network Services
#include "engine/NetworkServices/X360/NetworkServices_x360.h"
#include "engine/NetworkServices/PS3/NetworkServices_PS3.h"
#include "engine/NetworkServices/Win/NetworkServices_Win.h"

#include "engine/NetworkServices/NetworkServices.h"

// UPlay Service
#include "engine/NetworkServices/Win/UPlayService_Win.h"

#include "engine/NetworkServices/UPlayService.h"


// Save Game Adapter
#include "adapters/Adapter_Savegame_win/Adapter_Savegame_win.h"
#include "adapters/Adapter_Savegame_x360/Adapter_Savegame_x360.h"
#include "adapters/Adapter_Savegame_PS3/Adapter_Savegame_PS3.h"
#ifdef ITF_PS5
#include "adapters/Adapter_Savegame_PS5/Adapter_Savegame_PS5.h"
#endif //  ITF_PS5

#ifdef ITF_NINTENDO
#include "adapters/Adapter_Savegame_Nintendo/Adapter_Savegame_Nintendo.h"
#endif // ITF_NINTENDO

#ifdef ITF_XBOX_SERIES
#include "adapters/Adapter_Savegame_XBoxSeries/Adapter_Savegame_XBoxSeries.h"
#endif

#include "adapters/Adapter_Savegame_Dummy/Adapter_Savegame_Dummy.h"

// TRC Adapter
#include "adapters/TRCAdapter_win/TRCAdapter_win.h"
#include "adapters/TRCAdapter_x360/TRCAdapter_x360.h"
#include "adapters/TRCAdapter_PS3/TRCAdapter_PS3.h"
#include "adapters/TRCAdapter_Dummy/TRCAdapter_Dummy.h"

#ifdef ITF_PS5
#include "adapters/TRCAdapter_PS5/TRCAdapter_PS5.h"
#endif

#ifdef ITF_NINTENDO
#include "adapters/TRCAdapter_Nintendo/TRCAdapter_Nintendo.h"
#endif // ITF_NINTENDO

#ifdef ITF_XBOX_SERIES
#include "adapters/TRCAdapter_XBoxSeries/TRCAdapter_XBoxSeries.h"
#endif

// Online Tracking Adapter
#include "adapters/OnlineTrackingAdapter_win32/OnlineTrackingAdapter_win32.h"
#include "adapters/OnlineTrackingAdapter_x360/OnlineTrackingAdapter_x360.h"
#include "adapters/OnlineTrackingAdapter_ps3/OnlineTrackingAdapter_PS3.h"

// Adapter Ubiservices
#include "adapters/OnlineAdapter_Ubiservices/OnlineAdapter_Ubiservices.h"

// Reward adapter
#include "adapters/RewardAdapter_win/RewardAdapter_win.h"
#include "adapters/RewardAdapter_PS3/RewardAdapter_PS3.h"
#include "adapters/RewardAdapter_x360/RewardAdapter_x360.h"
#include "adapters/RewardAdapter_Dummy/RewardAdapter_Dummy.h"


namespace ITF
{


#define FACTORY_REGISTERING_BODY
#include "initRegistring.h"
#undef FACTORY_REGISTERING_BODY

#define FACTORY_REGISTERING_CPP
#include "initRegistring.h"
#undef FACTORY_REGISTERING_CPP

#ifndef _ITF_INITFACTORY_H_
#include "initfactory.h"
#endif //_ITF_INITFACTORY_H_
    
    InitFactory* InitFactory::m_pInstance = NULL;

    InitFactory::InitFactory()
    {
      m_pInstance = this;
      m_UsageMap["WIN"] = eRegisterUsage::eRegisterWin;
      m_UsageMap["X360"] = eRegisterUsage::eRegisterX360;
      m_UsageMap["PS3"] = eRegisterUsage::eRegisterPS3;
	  m_UsageMap["IPAD"] = eRegisterUsage::eRegisteriPad;
      m_UsageMap["CTR"] = eRegisterUsage::eRegisterCTR;
      m_UsageMap["WII"] = eRegisterUsage::eRegisterWII;
      m_UsageMap["PS5"] = eRegisterUsage::eRegisterPS5;
      m_UsageMap["NX"] = eRegisterUsage::eRegisterNX;
      m_UsageMap["OUNCE"] = eRegisterUsage::eRegisterOUNCE;
      m_UsageMap["XBOXSERIES"] = eRegisterUsage::eRegisterXBoxSeries;

#ifdef ITF_WINDOWS
        m_currentPlatformUsage = eRegisterUsage::eRegisterWin;
#elif defined (ITF_PS3)
        m_currentPlatformUsage = eRegisterUsage::eRegisterPS3;
#elif defined (ITF_X360)
        m_currentPlatformUsage = eRegisterUsage::eRegisterX360;
#elif defined (ITF_CTR)
      m_currentPlatformUsage = eRegisterUsage::eRegisterCTR;
#elif defined (ITF_WII)
      m_currentPlatformUsage = eRegisterUsage::eRegisterWII;
#elif defined (ITF_PS5)
      m_currentPlatformUsage = eRegisterUsage::eRegisterPS5;
#elif defined (ITF_NX)
      m_currentPlatformUsage = eRegisterUsage::eRegisterNX;
#elif defined (ITF_OUNCE)
      m_currentPlatformUsage = eRegisterUsage::eRegisterOUNCE;
#elif defined (ITF_XBOX_SERIES)
      m_currentPlatformUsage = eRegisterUsage::eRegisterXBoxSeries;
#endif

      initRegisteringFactory();
    }


    void InitFactory::registerFunctionExternal(const String& _szName,cbInitFactory _func)
   {
       getInstance()->registerFunction(_szName,_func);
   }


    void InitFactory::create()
    {
        ITF_ASSERT(!m_pInstance);
        newAlloc(mId_Singleton,InitFactory());
    }


    void InitFactory::destroy()
    {
        SF_DEL(m_pInstance);
    }

    InitFactory* InitFactory::getInstance()
    {
        ITF_ASSERT(m_pInstance);
        return m_pInstance;
    }

    void InitFactory::registerFunction(const String& _szName,cbInitFactory _func)
    {
        String moduleName = _szName;
        moduleName.toLower();
        m_FactoryFunction[moduleName] = _func;
    }

    void InitFactory::convertModuleToPlatform(const String& _szModuleName,String& _szDst)
    {
        if (m_currentPlatformUsage == eRegisterUsage::eRegisterWin)
        {
            _szDst =  _szModuleName+"_win";
        }
        else
        if (m_currentPlatformUsage == eRegisterUsage::eRegisterX360)
        {
            _szDst =  _szModuleName+"_x360";
        }
        else
        if (m_currentPlatformUsage == eRegisterUsage::eRegisterPS3)
        {
            _szDst =  _szModuleName+"_ps3";
        }
        else
        if (m_currentPlatformUsage == eRegisterUsage::eRegisteriPad)
        {
            _szDst =  _szModuleName+"_iPad";
        }
        else
        if (m_currentPlatformUsage == eRegisterUsage::eRegisterCTR)
        {
            _szDst =  _szModuleName+"_CTR";
        }
        else
        if (m_currentPlatformUsage == eRegisterUsage::eRegisterWII)
        {
            _szDst =  _szModuleName+"_WII";
        }
        else
        if (m_currentPlatformUsage == eRegisterUsage::eRegisterPS5)
        {
            _szDst = _szModuleName + "_PS5";
        }
        else
        if (m_currentPlatformUsage == eRegisterUsage::eRegisterNX)
        {
            _szDst = _szModuleName + "_NX";
        }
        else
        if (m_currentPlatformUsage == eRegisterUsage::eRegisterOUNCE)
        {
            _szDst = _szModuleName + "_OUNCE";
        }
        else
        if (m_currentPlatformUsage == eRegisterUsage::eRegisterXBoxSeries)
        {
            _szDst = _szModuleName + "_XBoxSeries";
        }

        _szDst.toLower();
    }

    void InitFactory::Register(const String& _szModuleName,const u32 ListUsage)
    {
        if (ListUsage== 0)
        {
           internalRegister(_szModuleName);
        }
        else
        if ((ListUsage & m_currentPlatformUsage) == u32(m_currentPlatformUsage))
        {
            internalRegister(_szModuleName);
        }
    }

    cbInitFactory InitFactory::getInitFunction(const String& _szModuleName,bbool bRaiseError /*btrue*/)
    {

        ITF_MAP<String,cbInitFactory>::const_iterator ite = m_FactoryFunction.find(_szModuleName);

        if(m_FactoryFunction.end() == ite)
        {
            if (bRaiseError)
            {
                ITF_ASSERT(0);
                return NULL;
            }
        }
        else
        {
            cbInitFactory func = (*ite).second;
            return func;
        }

        return NULL;
    }


    void InitFactory::internalRegister(const String& _szModuleName)
    {
        String moduleNameLower = _szModuleName;
        moduleNameLower.toLower();

        cbInitFactory func = getInitFunction(moduleNameLower,bfalse);

        if (func)
        {
            func();
        }
        else  //try with platform extension
        {
            String newModuleName;
            convertModuleToPlatform(moduleNameLower,newModuleName);
          
            func = getInitFunction(newModuleName,btrue);
            if (func)
            {
                func();
            }
        }
    }



} // namespace ITF


