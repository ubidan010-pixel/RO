#ifndef _ITF_FRIEZEPLUGINREGISTRY_H_
#define _ITF_FRIEZEPLUGINREGISTRY_H_

#ifndef _ITF_KEYARRAY_H_
#include "core/container/KeyArray.h"
#endif //_ITF_KEYARRAY_H_

namespace ITF
{
    class FriezePlugin;
    class Frise;
    typedef FriezePlugin* (*FriezePluginCreator)(Frise *);

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///Shortcut macro
    #define FRIEZE_PLUGIN_REGISTRY FriezePluginRegistry::getInstance()


    //////////////////////////////////////////////////////////////////////////
    class FriezePluginRegistry
    {
    public:

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// register a plugin
        /// @param _name : name of the plugin
        /// @param _creator : creation function
        void registerPlugin(const StringID &_name, FriezePluginCreator _creator) {m_friezePluginCreators.setID(_name, _creator);}
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// create an instance of a frieze plugin
        /// @param _name : name of the plugin
        /// @param _frieze : the frieze that will be linked
        FriezePlugin *createPluginInstance(const StringID &_name, Frise *_frieze) {i32 index = m_friezePluginCreators.find(_name); if (index>=0) return m_friezePluginCreators.getAt(index)(_frieze); else return NULL;}


        ///////////////////////////////////////////////////////////////////////////////////////////
        static FriezePluginRegistry *getInstance() {if (!s_instance) {create();} return s_instance;}        
        static void terminate() {SF_DEL(s_instance);}

    private:
        ~FriezePluginRegistry() {}

        static FriezePluginRegistry *s_instance;
        static void create() {ITF_ASSERT(!s_instance); s_instance = newAlloc(mId_Frieze, FriezePluginRegistry);}

        KeyArray<FriezePluginCreator> m_friezePluginCreators;
    };
}

#endif

