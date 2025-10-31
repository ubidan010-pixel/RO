#include "precompiled_engine.h"

#ifndef _ITF_ENGINESIGNATURE_H_
#include "engine/signature/engineSignature.h"
#endif //_ITF_ENGINESIGNATURE_H_

#ifndef SERIALIZEROBJECTSIGNATURE_H
#include "core/serializer/ZSerializerObjectSignature.h"
#endif // SERIALIZEROBJECTSIGNATURE_H

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_

#ifndef _ITF_BTNODE_H_
#include "gameplay/ai/BTs/BTNode.h"
#endif // _ITF_BTNODE_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/AI/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_


namespace ITF
{
    u32 EngineSignature::m_EngineSignature = 0;

#ifndef ITF_FINAL
    typedef void (*RTTICallback)( CSerializerObjectSignature& serSignature,IRTTIObject* obj );

    #define SERIALIZE_FORSIGNATURE   ITF_ASSERT(templateObj);\
                                      if (templateObj)\
                                       templateObj->Serialize(&serSignature,ESerialize_Data_Save);\

    void templateObjCallback(CSerializerObjectSignature& serSignature,IRTTIObject* obj )
    {
        TemplateObjBase* templateObj = obj->DynamicCast<TemplateObjBase >(ITF_GET_STRINGID_CRC(TemplateObjBase,3395210690));
        SERIALIZE_FORSIGNATURE
    }

    void objectComponentObjCallback(CSerializerObjectSignature& serSignature,IRTTIObject* obj )
    {
        ActorComponent* templateObj = obj->DynamicCast<ActorComponent>(ITF_GET_STRINGID_CRC(ActorComponent,2669192659));
        SERIALIZE_FORSIGNATURE
    }

    void BTNode_TemplateCallback(CSerializerObjectSignature& serSignature,IRTTIObject* obj )
    {
        BTNode_Template* templateObj = obj->DynamicCast<BTNode_Template >(ITF_GET_STRINGID_CRC(BTNode_Template,2115900170));
        SERIALIZE_FORSIGNATURE
    }


    void templateAIBehaviorObjCallback(CSerializerObjectSignature& serSignature,IRTTIObject* obj )
    {
         TemplateAIBehavior* templateObj = obj->DynamicCast< TemplateAIBehavior>(ITF_GET_STRINGID_CRC(TemplateAIBehavior,3890605354));
         SERIALIZE_FORSIGNATURE
    }

    void templateAIActionObjCallback(CSerializerObjectSignature& serSignature,IRTTIObject* obj )
    {
        AIAction_Template* templateObj = obj->DynamicCast< AIAction_Template>(ITF_GET_STRINGID_CRC(AIAction_Template,2801106802));
        SERIALIZE_FORSIGNATURE
    }


    void eventObjCallback(CSerializerObjectSignature& serSignature,IRTTIObject* obj )
    {
        Event* templateObj = obj->DynamicCast< Event>(ITF_GET_STRINGID_CRC(Event,772421353));
        SERIALIZE_FORSIGNATURE
    }


    void SequenceEvent_TemplateObjCallback(CSerializerObjectSignature& serSignature,IRTTIObject* obj )
    {
        SequenceEvent_Template* templateObj = obj->DynamicCast< SequenceEvent_Template>(ITF_GET_STRINGID_CRC(SequenceEvent_Template,4216994033));
        SERIALIZE_FORSIGNATURE
    }
    //--------------------------------------------------------------------------------------------------------------------------------------------------------
    struct FactoryKey
    {
        FactoryKey( ObjectFactory* _factory , RTTICallback _callback): m_factory(_factory),m_callback(_callback) {};
        ObjectFactory* m_factory;
        RTTICallback m_callback;
    };

    void EngineSignature::computeEngineSignature()
    {
        ArchiveMemory* pArchiveMemory = newAlloc(mId_System,ArchiveMemory);
        CSerializerObjectSignature serSignature(pArchiveMemory);




        ITF_VECTOR<FactoryKey> vFactory;
    

        vFactory.push_back( FactoryKey(TEMPLATEDATABASE->getComponentsFactory(),templateObjCallback));
        vFactory.push_back( FactoryKey(TEMPLATEDATABASE->getClassTemplateFactory(),templateObjCallback));
        vFactory.push_back( FactoryKey(TEMPLATEDATABASE->getBTNodesFactory(),BTNode_TemplateCallback));
        vFactory.push_back( FactoryKey(ACTORSMANAGER->getActorComponentsFactory(),objectComponentObjCallback));
        vFactory.push_back( FactoryKey(ACTORSMANAGER->getAIBehaviorsFactory(),templateAIBehaviorObjCallback));
        vFactory.push_back( FactoryKey(ACTORSMANAGER->getAIActionsFactory(),templateAIActionObjCallback));
        vFactory.push_back( FactoryKey(ACTORSMANAGER->getEventFactory(),eventObjCallback));
        vFactory.push_back( FactoryKey(ACTORSMANAGER->getAnimTreeNodeFactory(),templateObjCallback));
        vFactory.push_back( FactoryKey(ACTORSMANAGER->getSequenceEventFactory(),SequenceEvent_TemplateObjCallback));
        vFactory.push_back( FactoryKey(ACTORSMANAGER->getMusicTreeNodeFactory(),templateObjCallback));

        for (  ITF_VECTOR<FactoryKey> ::const_iterator iterFactory = vFactory.begin();iterFactory!=vFactory.end();++iterFactory)
        {
            const FactoryKey& factoryKey =  (*iterFactory);
            const ObjectFactory::ObjectContainer& containers = factoryKey.m_factory->GetObjectContainer();
            
            for (ObjectFactory::ObjectContainer::const_iterator it = containers.begin();it!=containers.end();it++  )
            {
                const ObjectFactory::ClassInfo& cinfo =(*it).second;
                IRTTIObject* obj = (IRTTIObject*)cinfo.CreateObject();
                factoryKey.m_callback(serSignature,obj);
                SF_DEL(obj);
            }
        }

        m_EngineSignature = serSignature.computeCRC();

        LOG("Signature >> 0x%08x",m_EngineSignature);

        SF_DEL(pArchiveMemory); 
    }

#endif //ITF_FINAL
}