#include "precompiled_engine.h"

#ifndef _ITF_BOOLEVENTLIST_H_
#include "engine/sequenceplayer/seq_utilities/BoolEventList.h"
#endif //_ITF_BOOLEVENTLIST_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_


namespace ITF
{
    /************************************************************************/
    /* BoolEvent                                                            */
    /************************************************************************/
    BEGIN_SERIALIZATION_SUBCLASS(BoolEventList, BoolEvent)
        SERIALIZE_MEMBER("value", m_value);
        SERIALIZE_MEMBER("Frame", m_frame);
    END_SERIALIZATION()


    void BoolEventList::BoolEvent::serializeToBlob (Blob* _blob)
    {
        _blob->pushInt32(m_id);
        _blob->pushInt32(m_frame);
        _blob->pushBool(m_value);
    }

    void BoolEventList::BoolEvent::fillFromBlob (Blob* _blob)
    {
        m_id = _blob->extractInt32();
        m_frame = _blob->extractInt32();
        m_value = _blob->extractBool();
    }


    /************************************************************************/
    /* BoolEventList                                                        */
    /************************************************************************/

    BEGIN_SERIALIZATION(BoolEventList)
        SERIALIZE_CONTAINER_OBJECT("Keys",m_keys);
        SERIALIZE_FUNCTION(postLoadForId, ESerialize_Data_Load);
    END_SERIALIZATION()

    void BoolEventList::postLoadForId()
    {
        const u32 count = m_keys.size();
        for (u32 i = 0; i < count ; i++)
        {
            m_keys[i].m_id = m_pointIdCounter;
            m_pointIdCounter++;
        }
    }

    void BoolEventList::addKey(BoolEvent& _bf, bbool _computeId /* = true*/)
    {
        m_keys.push_back(_bf);
        if(_computeId)
        {
            m_keys[m_keys.size() - 1].m_id = m_pointIdCounter;
            m_pointIdCounter++;
        }
        else
        {
            if(m_pointIdCounter <= _bf.m_id)
                m_pointIdCounter = _bf.m_id + 1;
        }
    }

    void BoolEventList::clear()
    {
        m_pointIdCounter = 0;
        m_keys.clear();
    }

    u32 BoolEventList::size() const
    {
        return m_keys.size();
    }

    const BoolEventList::BoolEvent& BoolEventList::getAt(u32 _index) const
    {
        return m_keys[_index];
    }


    void BoolEventList::fillFromBlob (Blob* _blob)
    {
        m_keys.clear();
        i32 count = _blob->extractInt32();
        for (i32 i = 0; i < count; i++)
        {
            BoolEvent bf;
            bf.fillFromBlob(_blob);
            m_keys.push_back(bf);
        }
    }

    void BoolEventList::serializeToBlob (Blob* _blob)
    {
        u32 count = m_keys.size();
        _blob->pushInt32(count);
        for (u32 i = 0; i < count; i++)
        {
            m_keys[i].serializeToBlob(_blob);        
        }
    }

} // namespace ITF