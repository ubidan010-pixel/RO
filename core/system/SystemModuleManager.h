#ifndef ITF_SYSTEMMODULEMANAGER_H_
#define ITF_SYSTEMMODULEMANAGER_H_

#pragma once

#include "core/container/extendedVector.h"
#include "core/String8.h"

namespace ITF
{
    // This class is here to control the loading/unloading of OS specific modules
    // It is handling multiple load count.
    // 
    // It was designed for module loading of Sony platforms.

    class SystemModuleManager
    {
    public:

        using ModuleId = u64; // ModuleId is platform specific
        using LogCallback = void (*)(const char* logMessage);

        static SystemModuleManager* getInstance() { return ms_instance; }
        static void destroyInstance();

        // Ask for loading a module if we need. Return the load count after the operation.
        i32 loadModule(ModuleId _moduleId);
        // Ask for unloading a module if not used by another lib. Return the load count after the operation.
        i32 unloadModule(ModuleId _moduleId);

        i32 getModuleLoadCount(ModuleId _moduleId) const;

        void logModulesActivities();
        virtual String8 getModuleName(ModuleId _module) const { return String8::emptyString; }

    protected:

        static void setInstance(SystemModuleManager* _instance) { ms_instance = _instance; }

        explicit SystemModuleManager(LogCallback _logCB)
            : m_logCallback(_logCB)
        {
        }

        SystemModuleManager(const SystemModuleManager&) = delete;
        SystemModuleManager& operator= (const SystemModuleManager&) = delete;

        virtual ~SystemModuleManager();

        virtual i64 doLoadModule(ModuleId _module) = 0;
        virtual i64 doUnloadModule(ModuleId _module) = 0;
        // Return the success code according to the platform, to compare with what doLoadModule/doUnloadModule return.
        virtual i64 getSuccessCode() const = 0;

        void unloadAllModules();

        // internal class for load count 
        class Module
        {
        public:
            explicit Module(ModuleId _module)
                : m_loadCount(1)
                , m_moduleId(_module)
            {
            }

            bool operator==(const Module& m) const
            {
                return (m.m_moduleId == m_moduleId);
            }

            bool operator!=(const Module& m) const
            {
                return !(*this == m);
            }

            void incrementLoadCount()
            {
                m_loadCount++;
            }

            void decrementLoadCount()
            {
                m_loadCount--;
            }

            i32 getLoadCount() const
            {
                return m_loadCount;
            }

            ModuleId getModuleId() const
            {
                return m_moduleId;
            }

        private:
            i32 m_loadCount;
            u64 m_moduleId;
        };


        using ModulesList = ITF_VECTOR<Module>;
        ModulesList m_modulesLoaded{};

    private:
        void invokeCallback(const char* _format, ...) const;
        static SystemModuleManager * ms_instance;
        LogCallback m_logCallback = nullptr;
    };

} // namespace ITF

#endif // ITF_SYSTEMMODULEMANAGER_H_
