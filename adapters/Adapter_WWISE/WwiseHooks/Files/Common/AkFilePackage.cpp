//////////////////////////////////////////////////////////////////////
// 
// AkFilePackage.h
//
// This class represents a file package that was created with the 
// AkFilePackager utility app (located in ($WWISESDK)/samples/FilePackager/). 
// It holds a system file handle and a look-up table (CAkFilePackageLUT).
//
// CAkFilePackage objects can be chained together using the ListFilePackages
// typedef defined below.
// 
// Copyright (c) 2007-2009 Audiokinetic Inc. / All Rights Reserved
//
//////////////////////////////////////////////////////////////////////

#include "precompiled_Adapter_WWISE.h"
#ifdef ITF_SUPPORT_WWISE
#include "AkFilePackage.h"

// Destroy file package and free memory.
void CAkFilePackage::Destroy()
{
	// Cache memory pointer to ensure memory is free'd _after_ deleting this.
	void * pToRelease	= m_pToRelease;

	// Call destructor.
	this->~CAkFilePackage();

	// Free memory.
	ClearMemory(pToRelease);
}

void CAkFilePackage::ClearMemory(
	void *		in_pMemToRelease	// Memory to free.
	)
{
	if (in_pMemToRelease)
	{
		AkFree(AkMemID_FilePackage, in_pMemToRelease);
	}
}
#endif