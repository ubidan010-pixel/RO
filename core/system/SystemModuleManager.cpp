#include "precompiled_core.h"

#include "core/system/SystemModuleManager.h"

namespace ITF
{
    SystemModuleManager* SystemModuleManager::ms_instance = nullptr;

    void SystemModuleManager::destroyInstance()
    {
        SystemModuleManager* instance = getInstance();
        setInstance(nullptr);
        delete instance;
    }

    SystemModuleManager::~SystemModuleManager()
    {
#ifndef ITF_FINAL
        logModulesActivities();
#endif //ITF_FINAL

        unloadAllModules();
    }

#ifndef ITF_FINAL
    #define ITF_SYSTEMMODULEMANAGER_LOG(_format, ...) \
        [&](){ if (m_logCallback != nullptr) invokeCallback(_format, ##__VA_ARGS__); }()
#else
    #define ITF_SYSTEMMODULEMANAGER_LOG(_format, ...) do {} while(0)
#endif

    void SystemModuleManager::invokeCallback(const char* _format, ...) const
    {
        if (m_logCallback == nullptr)
            return;
        va_list args;
        va_start(args, _format);
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), _format, args);
        va_end(args);
        m_logCallback(buffer);
    }

    //--------------------------------------------------------------------------
    i32 SystemModuleManager::loadModule(ModuleId _moduleId)
    {
        auto it = std::find_if(m_modulesLoaded.begin(), m_modulesLoaded.end(),
            [&](const auto & _module)
            {
                return _module.getModuleId() == _moduleId;
            }
        );

        if (it != m_modulesLoaded.end())
        {
            // Found 
            it->incrementLoadCount();

            ITF_SYSTEMMODULEMANAGER_LOG("Module %s (ID: %zu) not loaded (already done), count: %d\n", getModuleName(_moduleId).cStr(), _moduleId, it->getLoadCount());

            return it->getLoadCount();;
        }
        else
        {
            i64 res = doLoadModule(_moduleId);
            if (getSuccessCode() == res)
            {
                m_modulesLoaded.push_back(Module{ _moduleId });
                ITF_SYSTEMMODULEMANAGER_LOG("Module %s (ID: %zu) loaded.\n", getModuleName(_moduleId).cStr(), _moduleId);
            }
            else
            {
                ITF_SYSTEMMODULEMANAGER_LOG("Module %s loading error: 0x%08x\n", getModuleName(_moduleId).cStr(), res);
            }

            return 1;
        }
    }
    //--------------------------------------------------------------------------
    i32 SystemModuleManager::unloadModule(ModuleId _moduleId)
    {
        auto it = std::find_if(m_modulesLoaded.begin(), m_modulesLoaded.end(),
            [&](const auto& _module)
            {
                return _module.getModuleId() == _moduleId;
            }
        );

        if (it != m_modulesLoaded.end())
        {
            // Found
            if (it->getLoadCount() == 1)
            {
                i64 res = doUnloadModule(_moduleId);
                if (res == getSuccessCode())
                {
                    m_modulesLoaded.erase(it);
                    ITF_SYSTEMMODULEMANAGER_LOG("Module %s (ID: %zu) unloaded.\n", getModuleName(_moduleId).cStr(), _moduleId);
                    return 0;
                }
                else
                {
                    ITF_SYSTEMMODULEMANAGER_LOG("Module %s unloading error: %x\n", getModuleName(_moduleId).cStr(), res);
                    return 1;
                }
            }
            else
            {
                it->decrementLoadCount();
                ITF_SYSTEMMODULEMANAGER_LOG("Module %s (ID: %zu) not unloaded, remain: %u\n", getModuleName(_moduleId).cStr(), _moduleId, it->getLoadCount());
                return it->getLoadCount();
            }
        }
        else
        {
            ITF_ASSERT_CRASH(0, "Module not found in the list");
            return -1;
        }
    }

    i32 SystemModuleManager::getModuleLoadCount(ModuleId _moduleId) const
    {
        auto it = std::find_if(m_modulesLoaded.begin(), m_modulesLoaded.end(),
            [&](const auto& _module)
            {
                return _module.getModuleId() == _moduleId;
            }
        );

        if (it != m_modulesLoaded.end())
        {
            return it->getLoadCount();
        }
        return 0;
    }

    //--------------------------------------------------------------------------
    void SystemModuleManager::logModulesActivities()
    {
        invokeCallback("---------------------------------------\n");
        invokeCallback("ModuleManager: dumpModulesActivities\n");
        invokeCallback("Modules currently loaded found: %u\n\n", m_modulesLoaded.size());
        for(Module & moduleInfo : m_modulesLoaded)
        {
            invokeCallback("Module %s, ID: %zu, count: %u\n",
                    getModuleName(moduleInfo.getModuleId()).cStr(),
                    moduleInfo.getModuleId(),
                    moduleInfo.getLoadCount());
        }
        invokeCallback("---------------------------------------\n");
    }

    //--------------------------------------------------------------------------
    void SystemModuleManager::unloadAllModules()
    {
        for(Module& moduleInfo : m_modulesLoaded)
        {
            doUnloadModule(moduleInfo.getModuleId());
        }
        m_modulesLoaded.clear();
    }


} // namespace ITF
