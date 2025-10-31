#include "precompiled_core.h"

#ifndef SERIALIZEROBJECT_H
#include "core/serializer/ZSerializerObject.h"
#endif // SERIALIZEROBJECT_H

namespace ITF
{

void CSerializerObject::SerializeContainerDesc( const char* tag )
{
#ifndef ITF_FINAL
    if ( (getProperties()&ESerializerProperty_Tool) && !IsLoading() )
    {
        if ( OpenGroup("CONTAINER_DESCRIPTION") )
        {
            String strTag = tag;
            Serialize("TAG",strTag);
            CloseGroup();
        }
    }
#endif
}

void CSerializerObject::SerializeFactoryDesc( const char* tag, ObjectFactory* factory, bbool _list )
{
#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID)
    if ( (getProperties()&ESerializerProperty_Tool) && !IsLoading() )
    {
        if ( !factory )
        {
            factory = m_factory;
        }

        const ObjectFactory::ObjectsNamesContainer& names = factory->GetObjectsNames();
        String tagSave = tag;

        if ( OpenGroup("FACTORY_DESCRIPTION") )
        {
            Serialize("TAG",tagSave);
            Serialize("LIST",_list);

            for ( ObjectFactory::ObjectsNamesContainer::const_iterator it = names.begin(); it != names.end(); ++it )
            {
                if ( OpenGroup(it->getDebugString()) )
                {
                    CloseGroup();
                }
            }

            CloseGroup();
        }
    }
#endif
}

} // namespace ITF
