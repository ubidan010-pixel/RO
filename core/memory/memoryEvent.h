#ifndef ITF_MEMORYEVENT_H_
#define ITF_MEMORYEVENT_H_

#define PX_JOIN_HELPER( X, Y ) X##Y
#define PX_JOIN( X, Y ) PX_JOIN_HELPER( X, Y )
#define PX_MEMORYEVENT_LINE PX_JOIN(evt__, __LINE__)
#define PX_MEMORYSCOPE_LINE PX_JOIN(evtScope__, __LINE__)

#ifdef ITF_TRACKING_MEMORY_ENABLE

#define MEMORY_EVENT(nameEvent)   static memoryEvent PX_MEMORYEVENT_LINE(nameEvent);memoryEventScope PX_MEMORYSCOPE_LINE(&PX_MEMORYEVENT_LINE) \

namespace ITF
{

class memoryEvent
{
public:

    static  u32 m_TaggerEvent;
    static  u64 m_CurrentContextTag;

    memoryEvent(const char* _name): m_Name(_name),m_allocationCount(0),m_ParentTag(U32_INVALID),m_ParentVolatile(NULL)
    {
        m_TaggerEvent++;
        m_Tag = m_TaggerEvent;
        ITF_ASSERT(m_TaggerEvent<64);
    };

    void setName(const char* _name)
    {
        m_Name = _name;
    }

    void * operator new( size_t size )
    {
        return ::malloc( size );
    }

    const char* getName()     const                         {return m_Name;}
    memoryEvent* getParentVolatile()                        {return m_ParentVolatile;}
    void setParentVolatile(memoryEvent* _pParentVolatile)   {m_ParentVolatile = _pParentVolatile;m_ParentTag = _pParentVolatile->m_Tag;}

    u32                                         m_Tag;
    u32                                         m_ParentTag;
private:

    const char*                                 m_Name;
    memoryEvent*                                m_ParentVolatile;

    u32                                         m_allocationCount;

};

    class memoryEventScope
    {
    public:
        memoryEventScope(memoryEvent* _memEvent);
        ~memoryEventScope();

    protected:
        memoryEvent*          m_Event;
    };

}
#else

#define MEMORY_EVENT(nameEvent)   ((void)0)

#endif //ITF_TRACKING_MEMORY_ENABLE

#endif //ITF_MEMORYEVENT_H_