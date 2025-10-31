#ifndef _ITF_MODULEMANAGER_PS3_H_
#define _ITF_MODULEMANAGER_PS3_H_

#include <cell/sysmodule.h>

namespace ITF
{
    //--------------------------------------------------------------------------
    // This class is here to control the loading/unloading of all PS3 modules
    // ref count purpose

    class ModuleManager 
    {
    public:
        static ModuleManager*   getInstance() { if(!m_instance) createInstance(); return m_instance; } 
        static void destroy();

        // ask for loading a module if we need
        i32 loadModule(u16 _module);
        // ask for unloading a module if not used by another lib
        i32 unloadModule(u16 _module);        

        // internal class for ref count 
        class Module 
        {
        public:
            Module(u16 _module): module(_module), counter(1) {}
            u32 counter; 
            u16 module;

            bool operator==(const Module & m)
            {
                return (m.module == module);
            }
        };
        
#ifndef ITF_FINAL
        void dumpModulesActivities();
        String8 getModuleName(u16 _module) const;
#endif //ITF_FINAL

    private:
        ~ModuleManager();
        ModuleManager(): m_activeLOG(btrue) {}
        
        void unloadAllModules();
        static void createInstance();


        typedef ITF_VECTOR<Module> ModulesList;
        ModulesList _modulesLoaded;

        bbool m_activeLOG;
        static ModuleManager* m_instance;
    };

#define MODULE_MANAGER          ModuleManager::getInstance()

} // namespace ITF

#endif //_ITF_MODULEMANAGER_PS3_H_
