#ifndef _ITF_SCENE_TYPES_H_
#define _ITF_SCENE_TYPES_H_

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_

#ifndef _ITF_OBJECTFACTORY_H_
#include "core/serializer/ObjectFactory.h"
#endif //_ITF_OBJECTFACTORY_H_

#ifndef _ITF_OBJECTPATH_H_
#include "core/ObjectPath.h"
#endif //_ITF_OBJECTPATH_H_

namespace ITF
{
    class Pickable;
    class PolyLine;

    typedef SafeArray<Pickable*>                        PickableList;

    // the bbool is used to request a delete
    typedef std::pair<Pickable*, bbool>                 PendingRequest;
    typedef SafeArray<PendingRequest >                  PendingRequestList;

    // the ref is a the destination scene ref
    typedef SafeArray<std::pair<Pickable*, ObjectRef> > PendingTransferList;

    struct TargetFilterList
    {
        DECLARE_SERIALIZE()

        bbool               operator==(const String& _platform) const               { return m_platform == _platform; }

        String              m_platform;
        ITF_VECTOR<String8> m_objList;
    };

    class FriezeConnectionResult
    {
    public:
        DECLARE_SERIALIZE()

        FriezeConnectionResult()
            : m_prev(NULL), m_next(NULL)
        {
            m_prevColId = U32_INVALID;
            m_nextColId = U32_INVALID;
        }

        FriezeConnectionResult(PolyLine* _prev, const Vec2d& _pos, PolyLine* _next)
            : m_prev(_prev), m_pos(_pos), m_next(_next)
        {
            m_prevColId = U32_INVALID;
            m_nextColId = U32_INVALID;
        }

        void    applyResult()const;
        void    generateSerializationDataFromRuntime();
        void    generateRuntimeDataFromSerialization();

    private:

        // Runtime result
        PolyLine* m_prev;
        PolyLine* m_next;

        // For serialization
        Vec2d       m_pos;
        ObjectPath  m_prevOwner;
        u32         m_prevColId;
        ObjectPath  m_nextOwner;
        u32         m_nextColId;
    };

} // namespace ITF

#endif // _ITF_SCENE_TYPES_H_