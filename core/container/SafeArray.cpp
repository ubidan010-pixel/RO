#include "precompiled_core.h"

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_



namespace ITF
{
#ifdef _DEBUG

    #define TEST_ARRAY_AMOUNT   1000
    void    checkArray(SafeArray<i32>& _arr)
    {
        ITF_ASSERT(_arr.size() == TEST_ARRAY_AMOUNT);
        for (u32 i= 0; i < _arr.size(); i++)
        {
            ITF_ASSERT(_arr[i] == i32(i));
            ITF_ASSERT(_arr.find(i) == i32(i));
        }
        ITF_ASSERT(_arr.find(TEST_ARRAY_AMOUNT*2) == -1);
        ITF_ASSERT(_arr.findStartIndex(0, 1) == -1);
        ITF_ASSERT(_arr.findStartIndex(5, 5) == 5);
        ITF_ASSERT(_arr.findStartIndex(5, 4) == 5);
    }

    void SafeArray_Test()
    {
        {
            i32 i;
            SafeArray<i32> testi32Array(1);
            for (i=0; i < TEST_ARRAY_AMOUNT; i++)
                testi32Array.push_back(i);
            checkArray(testi32Array);
            testi32Array.fit();
            checkArray(testi32Array);

            for (i=0; i < TEST_ARRAY_AMOUNT; i++)
                testi32Array.eraseKeepOrder(0);

            for (i=0; i < TEST_ARRAY_AMOUNT; i++)
                testi32Array.push_back(i);
            checkArray(testi32Array);

            for (i=0; i < TEST_ARRAY_AMOUNT; i++)
                testi32Array.eraseKeepOrder(testi32Array.size()-1);

            for (i=0; i < TEST_ARRAY_AMOUNT; i++)
                testi32Array.push_back(i);
            checkArray(testi32Array);
        }
        {
            i32 i;
            SafeArray<i32> testi32Array(1);
            for (i=TEST_ARRAY_AMOUNT-1; i >= 0; i--)
                testi32Array.insert(i, 0);
            checkArray(testi32Array);
            testi32Array.clear();
            for (i=0; i < TEST_ARRAY_AMOUNT; i++)
            {
                testi32Array.push_back(i);
                testi32Array.push_back(TEST_ARRAY_AMOUNT*2);
            }
            for (i=0; i < (i32)testi32Array.size(); i++)
            {
                if (testi32Array[i] == TEST_ARRAY_AMOUNT*2)
                {
                    testi32Array.eraseKeepOrder(i);
                    i--;
                }
            }
            checkArray(testi32Array);
        }
        {
            i32 i;
            SafeArray<i32> testi32Array(1);
            for (i=TEST_ARRAY_AMOUNT-1; i >= 0; i--)
                testi32Array.insert(i, 0);
            checkArray(testi32Array);
            testi32Array.clear();
            for (i=0; i < TEST_ARRAY_AMOUNT; i++)
            {
                testi32Array.push_back(i);
                testi32Array.push_back(TEST_ARRAY_AMOUNT*2);
            }
            for (i=0; i < (i32)testi32Array.size(); i++)
            {
                if (testi32Array[i] == TEST_ARRAY_AMOUNT*2)
                {
                    testi32Array.eraseNoOrder(i);
                    i--;
                }
            }
            ITF_ASSERT(testi32Array.size() == TEST_ARRAY_AMOUNT);
        }

        ///////////////////// TEST eraseRangeNoOrder SECTION
        {   // delete whole array and rebuild it (to check it's  not fucked up when totally emptied)
            SafeArray<i32> test;
            for (i32 i=0;  i<10; i++)
                test.push_back(i);
            test.eraseRangeNoOrder(0,10);
            ITF_ASSERT(test.size() == 0);
            for (i32 i=0;  i<10; i++)
                test.push_back(i);
            ITF_ASSERT(test.size() == 10);
            for (i32 i=0;  i<10; i++)
                ITF_ASSERT(test[i] == i);
        }

        {   // erase a part in the middle of the array
            SafeArray<i32> test;
            for (i32 i=0;  i<10; i++)
                test.push_back(i);
            test.eraseRangeNoOrder(2,5);
            ITF_ASSERT(test.size() == 7);
            ITF_ASSERT(test[0] == 0);   // first element unchanged
            ITF_ASSERT(test[1] == 1);   // second element unchanged
            ITF_ASSERT(test[2] == 7);   // copied from tail (NoOrder rules)
            ITF_ASSERT(test[3] == 8);   // copied from tail (NoOrder rules)
            ITF_ASSERT(test[4] == 9);   // copied from tail (NoOrder rules)
            ITF_ASSERT(test[5] == 5);   // sixth element unchanged
            ITF_ASSERT(test[6] == 6);   // seventh element unchanged
        }

        { // erase a part at the beginning of fthe array
            SafeArray<i32> test;
            for (i32 i=0;  i<10; i++)
                test.push_back(i);
            test.eraseRangeNoOrder(0,5);
            ITF_ASSERT(test.size() == 5);
            ITF_ASSERT(test[0] == 5);
            ITF_ASSERT(test[1] == 6);
            ITF_ASSERT(test[2] == 7);
            ITF_ASSERT(test[3] == 8);
            ITF_ASSERT(test[4] == 9);
        }

        { // erase a part at the end of the array
            SafeArray<i32> test;
            for (i32 i=0;  i<10; i++)
                test.push_back(i);
            test.eraseRangeNoOrder(2,10);
            ITF_ASSERT(test.size() == 2);
            ITF_ASSERT(test[0] == 0);
            ITF_ASSERT(test[1] == 1);
        }

        ///////////////////// TEST buildFrom SECTION
        {
            SafeArray<i32> test;
            SafeArray<i32> test2;
            for (i32 i=0;  i<10; i++)
                test.push_back(i);
            test2.buildFrom(test, 0, 10);
            for (i32 i=0;  i<10; i++)
                ITF_ASSERT(test2[i] == i);
        }

        {
            SafeArray<i32> test;
            SafeArray<i32> test2;
            for (i32 i=0;  i<10; i++)
                test.push_back(i);
            test2.buildFrom(test, 5, 10);
            for (i32 i=5;  i<10; i++)
                ITF_ASSERT(test2[i-5] == i);
        }

        {
            SafeArray<i32> test;
            SafeArray<i32> test2;
            for (i32 i=0;  i<10; i++)
                test.push_back(i);
            test2.buildFrom(test, 5, 8);
            for (i32 i=5;  i<8; i++)
                ITF_ASSERT(test2[i-5] == i);
        }

    }

    void checkSortedArray(SafeArray<i32>& _array)
    {
        for (u32 i=1; i<_array.size(); i++)
        {
            ITF_ASSERT(_array[i] >= _array[i-1]);
        }
    }

    void SortedSafeArray_Test()
    {
        SafeArray<i32> testi32Array(1);
        testi32Array.push_back_sort(1);
        testi32Array.push_back_sort(2);
        testi32Array.push_back_sort(2);
        testi32Array.push_back_sort(3);
        checkSortedArray(testi32Array);
        ITF_ASSERT(-1 == testi32Array.sortedFind(0));
        ITF_ASSERT(0 == testi32Array.sortedFind(1));
        ITF_ASSERT(1 == testi32Array.sortedFind(2) || 2 == testi32Array.sortedFind(2));
        ITF_ASSERT(3 == testi32Array.sortedFind(3));

        testi32Array.clear();
        testi32Array.push_back_sort(3);
        testi32Array.push_back_sort(2);
        testi32Array.push_back_sort(1);
        testi32Array.push_back_sort(2);
        checkSortedArray(testi32Array);
        ITF_ASSERT(-1 == testi32Array.sortedFind(0));
        ITF_ASSERT(0 == testi32Array.sortedFind(1));
        ITF_ASSERT(1 == testi32Array.sortedFind(2) || 2 == testi32Array.sortedFind(2));
        ITF_ASSERT(3 == testi32Array.sortedFind(3));

        testi32Array.clear();
        testi32Array.push_back_sort(2);
        testi32Array.push_back_sort(3);
        testi32Array.push_back_sort(1);
        testi32Array.push_back_sort(2);
        checkSortedArray(testi32Array);
        ITF_ASSERT(-1 == testi32Array.sortedFind(0));
        ITF_ASSERT(0 == testi32Array.sortedFind(1));
        ITF_ASSERT(1 == testi32Array.sortedFind(2) || 2 == testi32Array.sortedFind(2));
        ITF_ASSERT(3 == testi32Array.sortedFind(3));

        testi32Array.clear();
        testi32Array.push_back_sort(1);
        testi32Array.push_back_sort(2);
        testi32Array.push_back_sort(3);
        testi32Array.push_back_sort(2);
        testi32Array.push_back_sort(4);
        checkSortedArray(testi32Array);
        ITF_ASSERT(-1 == testi32Array.sortedFind(0));
        ITF_ASSERT(0 == testi32Array.sortedFind(1));
        ITF_ASSERT(1 == testi32Array.sortedFind(2) || 2 == testi32Array.sortedFind(2));
        ITF_ASSERT(3 == testi32Array.sortedFind(3));
        ITF_ASSERT(4 == testi32Array.sortedFind(4));

        testi32Array.clear();
        testi32Array.push_back_sort(4);
        testi32Array.push_back_sort(3);
        testi32Array.push_back_sort(2);
        testi32Array.push_back_sort(2);
        testi32Array.push_back_sort(1);
        checkSortedArray(testi32Array);
        ITF_ASSERT(-1 == testi32Array.sortedFind(0));
        ITF_ASSERT(0 == testi32Array.sortedFind(1));
        ITF_ASSERT(1 == testi32Array.sortedFind(2) || 2 == testi32Array.sortedFind(2));
        ITF_ASSERT(3 == testi32Array.sortedFind(3));
        ITF_ASSERT(4 == testi32Array.sortedFind(4));

        testi32Array.clear();
        testi32Array.push_back_sort(2);
        testi32Array.push_back_sort(3);
        testi32Array.push_back_sort(1);
        testi32Array.push_back_sort(4);
        testi32Array.push_back_sort(2);
        checkSortedArray(testi32Array);
        ITF_ASSERT(-1 == testi32Array.sortedFind(0));
        ITF_ASSERT(0 == testi32Array.sortedFind(1));
        ITF_ASSERT(1 == testi32Array.sortedFind(2) || 2 == testi32Array.sortedFind(2));
        ITF_ASSERT(3 == testi32Array.sortedFind(3));
        ITF_ASSERT(4 == testi32Array.sortedFind(4));

        testi32Array.clear();
    }

#endif // _DEBUG

    namespace SafeArrayUtils
    {
        i32     find32(u32 _data2Test, u32* _data, u32 Number, i32* _lastFoundIndex)
        {
            u32* _dataBase = _data;
            u32* _dataEnd = _data + Number;
            while (_dataBase < _dataEnd)
            {
                if (_data2Test == *(_dataBase))
                {
                    i32 K = i32(_dataBase - _data);
                    return (i32)K;
                }
                _dataBase++;
            }
            return -1;
        }

        i32     find16(u16 _data2Test, u16* _data, u32 Number, i32* _lastFoundIndex)
        {
            for (u32 K = 0; K < Number; K++)
                if (_data2Test == *(_data++))
                {
                    return (i32)K;
                }
            return -1;
        }
    }

} // namespace ITF

