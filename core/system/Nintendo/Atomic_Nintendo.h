#ifndef ATOMIC_NX_H_
#define ATOMIC_NX_H_

#pragma once


namespace ITF
{

    namespace Atomic
    {
        inline i32 andOp(volatile i32& _variable, i32 _value)
        {
            return  __atomic_and_fetch(&_variable, _value, __ATOMIC_SEQ_CST);
        }

        inline i32 orOp(volatile i32& _variable, i32 _value)
        {
            return  __atomic_or_fetch(&_variable, _value, __ATOMIC_SEQ_CST);
        }

        inline i32 add(volatile i32& _variable, i32 _value)
        {
            return __atomic_add_fetch(&_variable, _value,__ATOMIC_SEQ_CST);
        }

        inline i32 sub(volatile i32& _variable, i32 _value)
        {
            return __atomic_sub_fetch(&_variable, _value, __ATOMIC_SEQ_CST);
        }

        inline i32 increment(volatile i32& _variable)
        {
            return __atomic_add_fetch(&_variable, 1,__ATOMIC_SEQ_CST);
        }

        inline i32 decrement(volatile i32& _variable)
        {
            return __atomic_sub_fetch(&_variable, 1, __ATOMIC_SEQ_CST);
        }

        inline i32 exchange(volatile i32& _variable, i32 _value)
        {
            return __atomic_exchange_n(&_variable, _value, __ATOMIC_SEQ_CST);
        }


        inline  i32 exchangeIfEqual(volatile i32& _variable, i32 _compareValue, i32 _assignValue)
        {
            i32 expectedValue = _compareValue;
            __atomic_compare_exchange_n(&_variable, &expectedValue, _assignValue, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);

            return expectedValue;
        }

        inline i64 exchangeIfEqual(volatile i64& _variable, i64 _compareValue, i64 _assignValue)
        {
            i64 expectedValue = _compareValue;
            __atomic_compare_exchange_n(&_variable, &expectedValue, _assignValue, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);

            return expectedValue;
        }

    }

}

#endif // __ATOMIC_NX_H__
