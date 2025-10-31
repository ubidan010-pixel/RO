#pragma once

#include "core/system/SystemModuleManager.h"

namespace ITF
{

    class SystemModuleManager_PS5 : public SystemModuleManager
    {
    public:
        static void createInstance(LogCallback logCB = nullptr);

    private:
        explicit SystemModuleManager_PS5(LogCallback logCB)
            : SystemModuleManager(logCB)
        {
        }

        String8 getModuleName(ModuleId _module) const override;

        i64 doLoadModule(ModuleId _module) override;
        i64 doUnloadModule(ModuleId _module) override;
        i64 getSuccessCode() const override;

        static void logMessage(const char* msg);
    };

#define SYSTEM_MODULE_MANAGER SystemModuleManager_PS5::getInstance()

} // namespace ITF
