#ifndef _ITF_TEMPLATESINGLETON_H_
#define _ITF_TEMPLATESINGLETON_H_

namespace ITF
{
    //This class is not thread safe because all singletons must be allocated in Singleton::initInternal
    //We don't allow autoCreate singleton

    template <typename T>
    class TemplateSingleton
    {
    public:

        TemplateSingleton() 
        {
            become_singleton();
        }

        void become_singleton() 
        {
#ifndef ITF_FINAL
            if( ms_instance!=0 ) 
            {
                ITF_ASSERT_MSG(0,"Duplicate singleton creation");
            }
#endif
            ms_instance = static_cast<T*>(this);
        }

        virtual ~TemplateSingleton()
        {
            if (ms_instance ==this)
                ms_instance = 0;
        }

        static T& get() {
#ifndef ITF_FINAL
            if(ms_instance==0)
            {
              ITF_ASSERT(0);
            }
#endif
            return *ms_instance;
        }

        static T* getptr() 
        {
            return ms_instance;
        }

        static bbool exists() 
        {
            return ms_instance!=0;
        }

        static T* instance() {
            return ms_instance;
        }

    private:
        static T* ms_instance;
    };

    template <typename T> T* TemplateSingleton<T>::ms_instance = nullptr;
}


#endif //_ITF_TEMPLATESINGLETON_H_