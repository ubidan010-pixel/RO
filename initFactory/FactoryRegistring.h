
#ifndef REGISTER_CALL_INITH
#define REGISTER_CALL_INIT_H(name)                      extern void itf_init_factory_##name();
#define REGISTER_CALL_INIT_H_PARENT(name,parent)        extern void itf_init_factory_##name();

#define REGISTER_CALL_INIT_CPP(name)                    ITF::InitFactory::registerFunctionExternal(#name, itf_init_factory_##name);
#define REGISTER_CALL_INIT_CPP_PARENT(name,parent)      ITF::InitFactory::registerFunctionExternal(#name, itf_init_factory_##name);
    
#define REGISTER_CALL_INIT_BODY(name)                   void itf_init_factory_##name() { SINGLETONS.set##name(newAlloc(mId_Singleton,name())); };
#define REGISTER_CALL_INIT_BODY_PARENT(name,parent)     void itf_init_factory_##name() { SINGLETONS.set##parent(newAlloc(mId_Singleton,name())); };
#endif //REGISTER_CALL_INITH


#undef REGISTER_ITF
#undef REGISTER_ITF_PARENT
#undef BEGIN_REGISTER 
#undef END_REGISTER

#if defined (FACTORY_REGISTERING_H)
#define BEGIN_REGISTER 
#define END_REGISTER
#define REGISTER_ITF(name)                      REGISTER_CALL_INIT_H(name)
#define REGISTER_ITF_PARENT(name,parent)        REGISTER_CALL_INIT_H_PARENT(name,parent)

#elif defined (FACTORY_REGISTERING_BODY)
#define BEGIN_REGISTER 
#define END_REGISTER
#define REGISTER_ITF(name)                      REGISTER_CALL_INIT_BODY(name)
#define REGISTER_ITF_PARENT(name,parent)        REGISTER_CALL_INIT_BODY_PARENT(name,parent)

#elif defined (FACTORY_REGISTERING_CPP)
#define BEGIN_REGISTER                          void initRegisteringFactory() {
#define END_REGISTER                            }
#define REGISTER_ITF(name)                      REGISTER_CALL_INIT_CPP(name)
#define REGISTER_ITF_PARENT(name,parent)        REGISTER_CALL_INIT_CPP_PARENT(name,parent)

#endif
