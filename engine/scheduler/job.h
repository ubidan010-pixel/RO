#ifndef _ITF_JOB_H_
#define _ITF_JOB_H_

namespace ITF
{

class Job
{

    public:

        Job():m_bmanaged(bfalse) {};

        virtual u32 getTag() {ITF_ASSERT(0);return U32_INVALID;};

        void setAsManaged(bbool _bmanaged){m_bmanaged = _bmanaged;}
        ITF_INLINE bbool isManaged() const {return m_bmanaged;}                  

        virtual void execute(bbool _immediate) {ITF_ASSERT(0);};
        virtual void synchronize() {ITF_ASSERT(0);};

        virtual bbool isSynchronizable() {return bfalse;};

        static void destroy(Job* pJob);
        static void destroySelf(Job* pJob);

    protected:

        friend class Scheduler;

        static void destroyManaged(Job* pJob);

         virtual ~Job() {};
        bbool m_bmanaged;//true if it s owned by an instance ,in this case,the owner destroy it
};

}

#endif // _ITF_JOB_H_

