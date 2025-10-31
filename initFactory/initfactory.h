#ifndef _ITF_INITFACTORY_H_
#define _ITF_INITFACTORY_H_

#pragma once

#include "core/container/extendedMap.h"

namespace ITF
{

    typedef void (*cbInitFactory)();
        
    #define FACTORY_REGISTERING_H
    #include "initRegistring.h"
    #undef FACTORY_REGISTERING_H

    struct eRegisterUsage
    {
        enum Enum : u32
        {
            eRegisterNone   = 0,
            eRegisterWin    = 1,
            eRegisterX360   = 1 << 1,
            eRegisterPS3    = 1 << 2,
            eRegisteriPad	= 1 << 3,
            eRegisterCTR    = 1 << 4,
            eRegisterWII    = 1 << 5,
            eRegisterPS5    = 1 << 6,
            eRegisterNX     = 1 << 7, // aka. Nintendo Switch
            eRegisterOUNCE  = 1 << 8, // aka. Nintendo Switch 2
            eRegisterXBoxSeries = 1 << 9, // aka. Scarlett

            eRegisterLatestPlus1, // One more the last one according to enum rules
            eRegisterAll    = ((eRegisterLatestPlus1 - 1) << 1) - 1,
        };
    };

   

    class InitFactory
    {

        public:

        InitFactory();

        static void create();
        static void destroy();
        static InitFactory* getInstance();
   
        void Register(const String& _szModuleName,const u32 ListUsage);
        cbInitFactory getInitFunction(const String& _szModuleName,bbool bRaiseError = btrue);
        void internalRegister(const String& _szModuleName);

        void convertModuleToPlatform(const String& _szModuleName,String& _szDst);
        void registerFunction(const String& _szName,cbInitFactory _func);
        
        static void registerFunctionExternal(const String& _szName,cbInitFactory _func);

        protected: 

        Map<String,u32> m_UsageMap;
        Map<String,cbInitFactory> m_FactoryFunction;

        eRegisterUsage::Enum      m_currentPlatformUsage;

        static InitFactory* m_pInstance;

        #define REG_WIN     ITF::eRegisterUsage::eRegisterWin
        #define REG_X360    ITF::eRegisterUsage::eRegisterX360
        #define REG_PS3     ITF::eRegisterUsage::eRegisterPS3
        #define REG_IPAD     ITF::eRegisterUsage::eRegisteriPad
        #define REG_CTR     ITF::eRegisterUsage::eRegisterCTR
        #define REG_WII     ITF::eRegisterUsage::eRegisterWII
        #define REG_PS5     ITF::eRegisterUsage::eRegisterPS5
        #define REG_NX      ITF::eRegisterUsage::eRegisterNX
        #define REG_OUNCE   ITF::eRegisterUsage::eRegisterOUNCE
        #define REG_XBOXSERIES   ITF::eRegisterUsage::eRegisterXBoxSeries
        #define REG_ALL     ITF::eRegisterUsage::eRegisterAll
        #define REG_INIT(name,pattern) ITF::InitFactory::getInstance()->Register(name, pattern);

    };

} // namespace ITF


#endif //_ITF_INITFACTORY_H_


