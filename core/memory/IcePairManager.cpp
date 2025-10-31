///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains a new optimized pair manager.
 *	\file		IcePairManager.cpp
 *	\author		Pierre Terdiman
 *	\date		December, 16, 2003
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GG : domain public
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "precompiled_core.h"

#include "icePairManager.h"

namespace ITF
{

//#define DEBUG_PAIR
#ifdef DEBUG_PAIR
#define ASSERT_PAIR(cond) ITF_ASSERT(cond)
#else
#define ASSERT_PAIR(cond)  ((void)0)
#endif //DEBUG_PAIR


#ifdef _MSC_VER
#ifdef _DEBUG
#pragma optimize ("g", on) 
#endif //_DEBUG
#endif
    ITF_FORCE_INLINE void StoreDwords(u32* dest, u32 nb, u32 value)
    {
        while(nb--)	*dest++ = value;
    }

    ITF_FORCE_INLINE u32	NextPowerOfTwo(u32 x)
    {
        x |= (x >> 1);
        x |= (x >> 2);
        x |= (x >> 4);
        x |= (x >> 8);
        x |= (x >> 16);
        return x+1;
    }
    template<class Type>
    ITF_FORCE_INLINE void TSwap(Type& a, Type& b)			
    { const Type c = a; a = b; b = c;			}


// Thomas Wang's hash
ITF_FORCE_INLINE int Hash32Bits_1(u32 key)
{
    key += ~(key << 15);
    key ^=  (key >> 10);
    key +=  (key << 3);
    key ^=  (key >> 6);
    key += ~(key << 11);
    key ^=  (key >> 16);
    return key;
}


ITF_INLINE void Sort(u16& id0, u16& id1)
{
	//if(id0>id1)	TSwap(id0, id1);
}

ITF_FORCE_INLINE u32 Hash(u32 id0, u32 id1)
{
    return Hash32Bits_1( (id0&0xffff)|(id1<<16) );
}

ITF_FORCE_INLINE	bool DifferentPair(const UserPair& p, u16 id0, u16 id1)
{
	return (id0!=p.mID0) || (id1!=p.mID1);
}

#define INVALID_USER_ID	0xffff

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UserPairManager2::UserPairManager2() :
	mHashSize		(0),
	mMask			(0),
	mHashTable		(NULL),
	mNext			(NULL),
	mNbActivePairs	(0),
	mActivePairs	(NULL)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UserPairManager2::~UserPairManager2()
{
	Purge();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UserPairManager2::Purge()
{
    ::free(mNext);
	::free(mActivePairs);
	::free(mHashTable);
	mHashSize		= 0;
	mMask			= 0;
	mNbActivePairs	= 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const UserPair* UserPairManager2::FindPair(u16 id0, u16 id1) const
{
	if(!mHashTable)	return NULL;	// Nothing has been allocated yet

	// Order the ids
	Sort(id0, id1);

	// Compute hash value for this pair
	const u32 HashValue = Hash(id0, id1) & mMask;

	// Look for it in the table
	u32 Offset = mHashTable[HashValue];
	while(Offset!=U32_INVALID && DifferentPair(mActivePairs[Offset], id0, id1))
	{
		ASSERT_PAIR(mActivePairs[Offset].mID0!=INVALID_USER_ID);
		Offset = mNext[Offset];		// Better to have a separate array for this
	}
	if(Offset==U32_INVALID)	return NULL;
	ITF_ASSERT(Offset<mNbActivePairs);
	// Match mActivePairs[Offset] => the pair is persistent
	return &mActivePairs[Offset];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Internal version saving hash computation
UserPair* UserPairManager2::FindPair(u16 id0, u16 id1, u32 hashValue) const
{
	if(!mHashTable)	return NULL;	// Nothing has been allocated yet

	// Look for it in the table
	u32 Offset = mHashTable[hashValue];
	while(Offset!=U32_INVALID && DifferentPair(mActivePairs[Offset], id0, id1))
	{
		ASSERT_PAIR(mActivePairs[Offset].mID0!=INVALID_USER_ID);
		Offset = mNext[Offset];		// Better to have a separate array for this
	}
	if(Offset==U32_INVALID)	return NULL;
	ASSERT_PAIR(Offset<mNbActivePairs);
	// Match mActivePairs[Offset] => the pair is persistent
	return &mActivePairs[Offset];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 UserPair* UserPairManager2::AddPair(u16 id0, u16 id1)
{
	// Order the ids
	Sort(id0, id1);

	u32 HashValue = Hash(id0, id1) & mMask;

/*	UserPair* P = FindPair(id0, id1, HashValue);
	if(P)
		return P;	// Persistent pair*
*/

	// This is a new pair
	if(mNbActivePairs>=mHashSize)
	{
		// Get more entries
		mHashSize = NextPowerOfTwo(mNbActivePairs+1);
		mMask = mHashSize - 1;

        ::free(mHashTable);
        mHashTable = (u32*)::malloc(mHashSize*sizeof(u32));
		StoreDwords(mHashTable, mHashSize, U32_INVALID);

		// Get some bytes for new entries
        UserPair*   NewPairs	= (UserPair*)::malloc(mHashSize * sizeof(UserPair));	ITF_ASSERT_CRASH(NewPairs,"can't allocate newPairs");
        u32*        NewNext		= (u32*)::malloc(mHashSize * sizeof(u32));		        ITF_ASSERT_CRASH(NewNext,"can't allocate nextPairs");

		// Copy old data if needed
		if(mNbActivePairs)	ITF_MEMCOPY(NewPairs, mActivePairs, mNbActivePairs*sizeof(UserPair));
		// ### check it's actually needed... probably only for pairs whose hash value was cut by the and
		// yeah, since Hash(id0, id1) is a constant
		// However it might not be needed to recompute them => only less efficient but still ok
		for(u32 i=0;i<mNbActivePairs;i++)
		{
			u32 HashValue = Hash(mActivePairs[i].mID0, mActivePairs[i].mID1) & mMask;	// New hash value with new mask
			NewNext[i] = mHashTable[HashValue];
			mHashTable[HashValue] = i;
		}

		// Delete old data
        ::free(mNext);
        ::free(mActivePairs);

		// Assign new pointer
		mActivePairs = NewPairs;
		mNext = NewNext;

		// Recompute hash value with new hash size
		HashValue = Hash(id0, id1) & mMask;
	}

	UserPair* p = &mActivePairs[mNbActivePairs];
	p->mID0		= id0;
	p->mID1		= id1;

	mNext[mNbActivePairs] = mHashTable[HashValue];
	mHashTable[HashValue] = mNbActivePairs++;
	return p;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool UserPairManager2::RemovePair(u16 id0, u16 id1,u64 &_value)
{
	// Order the ids
	Sort(id0, id1);

	const u32 HashValue = Hash(id0, id1) & mMask;
	const UserPair* P = FindPair(id0, id1, HashValue);
	if(!P)	return false;
	ASSERT_PAIR(P->mID0==id0);
	ASSERT_PAIR(P->mID1==id1);

    //ITF_Change
    _value = P->mUserData;

	const u32 PairIndex = GetPairIndex(P);

	// Walk the hash table to fix mNext
	u32 Offset = mHashTable[HashValue];
    ASSERT_PAIR(Offset!=U32_INVALID);

	u32 Previous=U32_INVALID;
	while(Offset!=PairIndex)
	{
		Previous = Offset;
		Offset = mNext[Offset];
	}

	// Let us go/jump us
	if(Previous!=U32_INVALID)
	{
		ASSERT_PAIR(mNext[Previous]==PairIndex);
		mNext[Previous] = mNext[PairIndex];
	}
	// else we were the first
	else mHashTable[HashValue] = mNext[PairIndex];
	// we're now free to reuse next[pairIndex] without breaking the list

#ifdef DEBUG_PAIR
	mNext[PairIndex]=U32_INVALID;
#endif

	// Fill holes
//	if(1)
	{
		// 1) Remove last pair
		const u32 LastPairIndex = mNbActivePairs-1;
		if(LastPairIndex==PairIndex)
		{
			mNbActivePairs--;
			return true;
		}

		const UserPair* Last = &mActivePairs[LastPairIndex];
		const u32 LastHashValue = Hash(Last->mID0, Last->mID1) & mMask;

		// Walk the hash table to fix mNext
		u32 Offset = mHashTable[LastHashValue];
		ASSERT_PAIR(Offset!=U32_INVALID);

		u32 Previous=U32_INVALID;
		while(Offset!=LastPairIndex)
		{
			Previous = Offset;
			Offset = mNext[Offset];
		}

		// Let us go/jump us
		if(Previous!=U32_INVALID)
		{
			ITF_ASSERT(mNext[Previous]==LastPairIndex);
			mNext[Previous] = mNext[LastPairIndex];
		}
		// else we were the first
		else mHashTable[LastHashValue] = mNext[LastPairIndex];
		// we're now free to reuse mNext[LastPairIndex] without breaking the list

#ifdef DEBUG_PAIR
		mNext[LastPairIndex]=U32_INVALID;
#endif

		// Don't invalidate entry since we're going to shrink the array

		// 2) Re-insert in free slot
		mActivePairs[PairIndex] = mActivePairs[LastPairIndex];
#ifdef DEBUG_PAIR
		ASSERT_PAIR(mNext[PairIndex]==U32_INVALID);
#endif
		mNext[PairIndex] = mHashTable[LastHashValue];
		mHashTable[LastHashValue] = PairIndex;

		mNbActivePairs--;
	}
	return true;
}

#ifdef _MSC_VER
#ifdef _DEBUG
#pragma optimize ("g", off) 
#endif //_DEBUG
#endif
}
