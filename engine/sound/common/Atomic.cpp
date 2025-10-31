#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_WWISE

#include "engine\sound\common\Atomic.h"



namespace ITF
{
    namespace wwise
    {

        void Atomic::setWithMemBarrier(volatile i32& _ref, const i32 _value)
        {
            _ref = _value;

            // volatiles are real volatiles on win32: no need for memory barriers on win32


        }

        u32 Atomic::getWithMemBarrier(const volatile i32& _ref)
        {
            // volatiles are real volatiles on win32: no need for memory barriers on win32



            return _ref;
        }



        void Atomic::setWithMemBarrier(volatile u32& _ref, const u32 _value)
        {
            _ref = _value;

            // volatiles are real volatiles on win32: no need for memory barriers on win32


        }

        u32 Atomic::getWithMemBarrier(const volatile u32& _ref)
        {
            // volatiles are real volatiles on win32: no need for memory barriers on win32



            return _ref;
        }



        void Atomic::setWithMemBarrier(volatile bool& _ref, const bool _value)
        {
            _ref = _value;

            // volatiles are real volatiles on win32: no need for memory barriers on win32


        }

        bool Atomic::getWithMemBarrier(const volatile bool& _ref)
        {
            // volatiles are real volatiles on win32: no need for memory barriers on win32


            return _ref;
        }





        /*
        template <typename T>
        void Atomic::setWithMemBarrier( volatile T * & _ref, const T * _value)
        {
            _ref = _value;

            // volatiles are real volatiles on win32: no need for memory barriers on win32

        #if defined WORK_PLATFORM_XB360
            __lwsync();
        #endif // WORK_PLATFORM_XB360
        }


        template <typename T>
        T * Atomic::getWithMemBarrier( const volatile T * & _ref )
        {
            // volatiles are real volatiles on win32: no need for memory barriers on win32

        #if defined WORK_PLATFORM_XB360
            __lwsync();
        #endif // WORK_PLATFORM_XB360

            return _ref;
        }


        template <typename T>
        T * Atomic::getWithMemBarrier( volatile T * & _ref )
        {
            // volatiles are real volatiles on win32: no need for memory barriers on win32

        #if defined WORK_PLATFORM_XB360
            __lwsync();
        #endif // WORK_PLATFORM_XB360

            return _ref;
        }
        */

    }

} // namespace ITF
#endif


