#ifndef _ATOMIC_H
#define _ATOMIC_H






namespace ITF
{
    namespace wwise
    {
        class Atomic
        {

        public:

            static void setWithMemBarrier(volatile i32& _ref, const i32 _value);
            static u32 getWithMemBarrier(const volatile i32& _ref);

            static void setWithMemBarrier(volatile u32& _ref, const u32 _value);
            static u32 getWithMemBarrier(const volatile u32& _ref);

            static void setWithMemBarrier(volatile bool& _ref, const bool _value);
            static bool getWithMemBarrier(const volatile bool& _ref);

            template <typename T>
            static void setWithMemBarrier(volatile T*& _ref, const T* _value)
            {
                _ref = (volatile T*)_value;

                // volatiles are real volatiles on win32: no need for memory barriers on win32


            }


            template <typename T>
            static T* getWithMemBarrier(const T volatile*& _ref)
            {
                // volatiles are real volatiles on win32: no need for memory barriers on win32



                return _ref;
            }


            template <typename T>
            static T* getWithMemBarrier(T volatile*& _ref)
            {
                // volatiles are real volatiles on win32: no need for memory barriers on win32



                T* value = (T*)_ref;

                return value;
            }

        };

} // namespace Sound

} // namespace ITF

#endif // _ATOMIC_H

