///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains a new optimized pair manager.
 *	\file		IcePairManager.h
 *	\author		Pierre Terdiman
 *	\date		December, 16, 2003
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef ICEPAIRMANAGER_H
#define ICEPAIRMANAGER_H

#include "core/types.h"

namespace ITF
{
	struct UserPair
	{
		u16	    mID0;
		u16 	mID1;
        u64 	mUserData;
	};

	ITF_COMPILE_TIME_ASSERT(UserPair_error,sizeof(UserPair)==16);

	class UserPairManager2
	{
		public:
									UserPairManager2();
									~UserPairManager2();

				void				Purge();
    
				UserPair*		    AddPair		(u16 id0, u16 id1);
				bool				RemovePair	(u16 id0, u16 id1,u64 &_value);
				const UserPair*		FindPair	(u16 id0, u16 id1)	const;
		ITF_INLINE	u32				GetPairIndex(const UserPair* pair)	const
									{
                                        return uSize(pair - mActivePairs);
										//return ((u32)((size_t(pair) - size_t(mActivePairs)))/sizeof(UserPair));
									}

		ITF_INLINE	u32				GetNbActivePairs()	const	{ return mNbActivePairs;	}
		ITF_INLINE	const UserPair*		GetActivePairs()	const	{ return mActivePairs;		}

		private:
				u32				    mHashSize;
				u32				    mMask;
				u32				    mNbActivePairs;
				u32*				mHashTable;
				u32*				mNext;
				UserPair*			mActivePairs;
		ITF_INLINE	UserPair*			FindPair(u16 id0, u16 id1, u32 hashValue) const;
	};
}

#endif // ICEPAIRMANAGER_H
