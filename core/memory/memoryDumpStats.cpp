#include "precompiled_core.h"

#include <algorithm>

#ifndef _ITF_THREADALLOCATIONTRACKER_H_
#include "core/memory/threadAllocatorStacker.h"
#endif //_ITF_THREADALLOCATIONTRACKER_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_OUTPUTDEBUGGER_H_
#include "core/OutputDebugger.h"
#endif //_ITF_OUTPUTDEBUGGER_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef ITF_MEMORYSTATSMANAGER_H_
#include "core/memory/memoryStatsManager.h"
#endif //ITF_MEMORYSTATSMANAGER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_


#ifdef ITF_TRACKING_MEMORY_ENABLE

namespace ITF
{

    bool UDgreater ( u64 memchk1, u64 memchk2 )
    {
        HeaderMem* hMem1        = (HeaderMem*)GET_SIZECHUNCK(memchk1);
        HeaderMem* hMem2        = (HeaderMem*)GET_SIZECHUNCK(memchk2);


        return hMem1->m_size < hMem2->m_size;
    }

    bbool filename_excluded(const ITF_VECTOR<String8>& _excludeFiles,const char* _filename)
    {
        for (ITF_VECTOR<String8> ::const_iterator iter = _excludeFiles.begin();iter!=_excludeFiles.end();++iter)
        {
            if (strstr(_filename,(*iter).cStr()))
                return btrue;
        }

        return bfalse;
    }


    UserPair* threadAllocatorTracker::copyAllocations(u32& _retCountAllocation)
    {
        UserPair* pairList = NULL;

        u32 countAllocation = 0;
        csAutoLock cs(m_cs);
        {
            //use a copy of pair

            countAllocation = m_allocationMap.GetNbActivePairs();
            pairList = (UserPair*)::malloc(sizeof(UserPair)*countAllocation);//could be reduce by one parsing
            ITF_Memcpy(pairList,m_allocationMap.GetActivePairs(),sizeof(UserPair)*countAllocation);
        }

        _retCountAllocation = countAllocation;
        return pairList; 

    }



void threadAllocatorTracker::requestCaptureFrameAllocation(u32 _allocatorID, u32 _countMax)
{
    //enable tracking for a frame
    m_trackFrameRequested   = btrue;
    m_trackingAllocatorID   = _allocatorID;
    m_trackingMaxCount      = _countMax;
}

void threadAllocatorTracker::dumpCaptureFrameAllocation(Blob& _blob)
{   
#ifdef ITF_WINDOWS
    char szApplicationDirectory[MAX_PATH];
    char szDumpName[MAX_PATH];
    ::GetCurrentDirectoryA(MAX_PATH, szApplicationDirectory);

    sprintf_s(szDumpName, MAX_PATH, "%s//dumpMemory.txt",szApplicationDirectory);
    FILE *file = fopen(szDumpName,"wt");

    if (file)
    {

        memoryTrackingVector<frameAllocationTracking>::iterator iter = m_allocationFrame.begin();

        //std::reverse(m_allocationFrame.begin(),m_allocationFrame.end());
        char tmp[256];

        for (iter;iter!=m_allocationFrame.end();++iter)
        {
            const frameAllocationTracking& allocTracking = (*iter);
            const ITF_AllocatorAttributes& AllocAttributes = *( ITF_AllocatorAttributes* )&allocTracking.attributes;

            sprintf_s(tmp,256,"ALLOC: %zu %d %d %d\n", allocTracking.size, 0, AllocAttributes.dwAllocatorId, allocTracking.crc);
            fputs(tmp,file);

            //dump the crc;
            mapMemoryTracker<u_long,TraceInfo*>::iterator iterCallstack = m_callstack.find(allocTracking.crc);

            if (iterCallstack != m_callstack.end())
            {
                //push all the size 
                TraceInfo* pTrace = (*iterCallstack).second;
                m_stackWalker.fillDescriptorToFile(file, pTrace->m_stack, threadAllocatorTracker::MAX_STACK_DEPTH);

                fflush(file);
            }

        }

        fclose(file);
        _blob.pushString(szDumpName);
    }

    m_allocationFrame.clear();
#endif //ITF_WINDOWS
}


void threadAllocatorTracker::dumpCaptureTopAllocation(Blob& _blob, u32 _allocatorID, u32 _countMax)
{
#ifdef ITF_WINDOWS  //until support for other platform
    u32  countAllocation;
    UserPair* pairList = copyAllocations(countAllocation);

    uPtr minRange = 0;

    memoryTrackingVector<u64> mTracking;
    mTracking.reserve(_countMax);

    for (u32 index = 0;index<countAllocation;++index)
    {
        const UserPair* pair = pairList+index;
        uPtr address = ((pair->mID0)<<16) | (pair->mID1);
        HeaderMem* headerMem = GET_HEADER_FROM_USER_PTR(address);

        const ITF_AllocatorAttributes& AllocAttributes = *( ITF_AllocatorAttributes* )&headerMem->m_dwAttribute;
        if (_allocatorID==U32_INVALID || AllocAttributes.dwAllocatorId == _allocatorID)
        {
            u32 crc     = GET_CRCCHUNCK(pair->mUserData);

            //create a id ==> crc and address;
            u64 key = MAKE_MEMCHUNCK(uPtr(headerMem),crc);

            if (mTracking.size()<_countMax)
            {
                mTracking.push_back(key);
                //sort
                std::sort(mTracking.begin(), mTracking.end(),UDgreater);
                HeaderMem* pMem = (HeaderMem*) GET_SIZECHUNCK(mTracking[0]);
                minRange = pMem->m_size;
                
            }
            else
            {
                if (headerMem->m_size>minRange)
                {
                    mTracking[0] = key;
                    std::sort(mTracking.begin(), mTracking.end(),UDgreater);
                    HeaderMem* pMem = (HeaderMem*) GET_SIZECHUNCK(mTracking[0]);
                    minRange = pMem->m_size;
                }

            }

        }
    }

    char szApplicationDirectory[MAX_PATH];
    char szDumpName[MAX_PATH];
    ::GetCurrentDirectoryA(MAX_PATH, szApplicationDirectory);

    sprintf_s(szDumpName, MAX_PATH, "%s//dumpMemory.txt",szApplicationDirectory);
    FILE *file = fopen(szDumpName,"wt");

    if (file)
    {
        std::reverse(mTracking.begin(),mTracking.end());
        char tmp[256];

        for (u32 index = 0;index<mTracking.size();++index)
        {
            u64 key = mTracking[index];
            u32 crc = GET_CRCCHUNCK(key);
            HeaderMem* pMem =  (HeaderMem*) (GET_SIZECHUNCK(key));
            const ITF_AllocatorAttributes& AllocAttributes = *( ITF_AllocatorAttributes* )&pMem->m_dwAttribute;

            sprintf_s(tmp,256, "ALLOC: %zu 0x%018zX %d %d\n", pMem->m_size, pMem->m_Ptr, AllocAttributes.dwAllocatorId, crc);
            fputs(tmp,file);

            //dump the crc;
            mapMemoryTracker<u_long,TraceInfo*>::iterator iterCallstack = m_callstack.find(crc);

            if (iterCallstack != m_callstack.end())
            {
                //push all the size 
                TraceInfo* pTrace = (*iterCallstack).second;
                m_stackWalker.fillDescriptorToFile(file, pTrace->m_stack, threadAllocatorTracker::MAX_STACK_DEPTH);

                fflush(file);
            }


        }

        fclose(file);
        _blob.pushString(szDumpName);
    }

#endif // ITF_WINDOWS
}


void threadAllocatorTracker::dumpAllocatorID(Blob& _blob, u32 _allocatorID)
{
#ifdef ITF_WINDOWS //until support for other platform
    u32  countAllocation;
    UserPair* pairList = copyAllocations(countAllocation);
   
    typedef ITF_MAP<uPtr, u32> mapSizeCount;
    typedef ITF_MAP<u32, mapSizeCount > mapCRCSizeCount;
                
    mapCRCSizeCount  mCRCHeaderMem;

    for (u32 index = 0;index<countAllocation;++index)
    {
        const UserPair* pair = pairList+index;
        u32 address = ((pair->mID0)<<16) | (pair->mID1);
        HeaderMem* headerMem = GET_HEADER_FROM_USER_PTR(address);
        const ITF_AllocatorAttributes& AllocAttributes = *( ITF_AllocatorAttributes* )&headerMem->m_dwAttribute;
        if (AllocAttributes.dwAllocatorId == _allocatorID)
        {
            u64 memchk  = pair->mUserData;

            u32 crc   = GET_CRCCHUNCK(memchk);

            mapCRCSizeCount::iterator iterCRCHeaderMem = mCRCHeaderMem.find(crc);
            if (iterCRCHeaderMem != mCRCHeaderMem.end())
            {
                mapSizeCount & v = (*iterCRCHeaderMem).second;
                mapSizeCount::iterator iterSizeCount = v.find(headerMem->m_size);

                if (iterSizeCount != v.end())
                {
                    u32& count =  (*iterSizeCount).second;
                    ++count;

                }
                else
                {
                    mapSizeCount &v = (*iterCRCHeaderMem).second;
                    v[headerMem->m_size] = 1;
                }

            }
            else
            {
                mapSizeCount v;
                v[headerMem->m_size] = 1;
                mCRCHeaderMem[crc] = v;
            }
        }
    }


    //we have now a list of crc ,translate to string list

    ProcessAddressDescriptor desc;

    ITF_VECTOR<String>  filename;
    filename.reserve(MAX_STACK_DEPTH);
    ITF_VECTOR<String>  symbolname;
    symbolname.reserve(MAX_STACK_DEPTH);
    ITF_VECTOR<u32>     line;
    line.reserve(MAX_STACK_DEPTH);

    //TODO Move to common place
    ITF_VECTOR<String8> excludeFiles;
    excludeFiles.push_back("memory.cpp");
    excludeFiles.push_back("memory_win.inl");
    excludeFiles.push_back("stackwalker.cpp");

    _blob.pushUInt64(mCRCHeaderMem.size());

    for (mapCRCSizeCount::iterator iterCRC = mCRCHeaderMem.begin();iterCRC!=mCRCHeaderMem.end();++iterCRC)
    {
        u32 crc = (*iterCRC).first;

        mapMemoryTracker<u_long,TraceInfo*>::iterator iterCallstack = m_callstack.find(crc);

        if (iterCallstack != m_callstack.end())
        {

            //push all the size 

            TraceInfo* pTrace = (*iterCallstack).second;
            filename.clear();
            symbolname.clear();
            line.clear();

            //push all the allocation done for this CRC
            mapCRCSizeCount ::iterator iterCRC = mCRCHeaderMem.find(crc);
            if (iterCRC != mCRCHeaderMem.end())
            {
                const mapSizeCount &mSizeCount = (*iterCRC).second;
                const u32 count = mSizeCount.size();
                _blob.pushUInt32(count);

                for (mapSizeCount::const_iterator iter =  mSizeCount.begin();iter!=mSizeCount.end();++iter)
                {
                    uPtr size    = (*iter).first;
                    u32 count   = (*iter).second;
                    _blob.pushUInt64(size);
                    _blob.pushUInt32(count);
                }

            }


            u32 index = 0;
            while (index < MAX_STACK_DEPTH)
            {
                u64 addr = pTrace->m_stack[index++];
                if (addr)
                {
                    m_stackWalker.fillDescriptor(addr,desc);

                    if (desc.m_line>0)
                    {
                        //may we want to exclude this file?
                        if (!filename_excluded(excludeFiles,desc.m_filename))
                        {
                            filename.push_back(desc.m_filename);
                            symbolname.push_back(desc.m_symbolName);
                            line.push_back(desc.m_line);
                        }
                    }
                }
                else
                {
                    break;
                }

            }

            ITF_VECTOR<String> ::iterator iterFilename     = filename.begin();
            ITF_VECTOR<String> ::iterator itersymbolname   = symbolname.begin();
            ITF_VECTOR<u32>    ::iterator iterline         = line.begin();

            _blob.pushUInt32(filename.size());
            for (iterFilename;iterFilename!=filename.end();++iterFilename,++itersymbolname,++iterline)
            {
                _blob.pushString(*iterFilename);
                _blob.pushString(*itersymbolname);
                _blob.pushUInt32(*iterline);
            }

        }
        else
        {
            ITF_ASSERT(0);
        }


    }

    ::free(pairList);
#endif // ITF_WINDOWS
}

void threadAllocatorTracker::dumpMemoryCapture()
{
#ifdef ITF_X360
    csAutoLock cs(m_cs);
    u32 maxBackTrace  = threadAllocatorTracker::MAX_STACK_DEPTH;

    File* dumpFile = FILEMANAGER->openFile("d:\\dump.dat",ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);

    if (dumpFile)
    {
        f64 rs = SYSTEM_ADAPTER->getTime();

        HRESULT error;
        PDM_WALK_MODULES pWalkMod = NULL;
        DMN_MODLOAD modLoad;

        MEMORYSTATUS stat;
        // Get the memory status.
        GlobalMemoryStatus( &stat );

        while( XBDM_NOERR == ( error = DmWalkLoadedModules( &pWalkMod, &modLoad ) ) )
        {
            // Find the signature that describes the PDB file of the current module.
            DM_PDB_SIGNATURE signature = {0};
            DmFindPdbSignature( modLoad.BaseAddress, &signature );

            // dumpFile->write(arrayTrace,sizeof(u32)*maxBackTrace);

            DWORD modulePrefixID = 0xABABCDCD;
            dumpFile->write(&modulePrefixID, sizeof(modulePrefixID));

            // Write the data that describes this module.
            dumpFile->write(&modLoad.BaseAddress, sizeof(modLoad.BaseAddress));
            dumpFile->write(&modLoad.Size, sizeof(modLoad.Size));
            dumpFile->write(&modLoad.TimeStamp, sizeof(modLoad.TimeStamp));
            dumpFile->write(&modLoad.Name, sizeof(modLoad.Name));
            dumpFile->write(&signature, sizeof(signature));
        }

        if( error != XBDM_ENDOFLIST )
        {
            // Handle errors here...
            error = error;
        }

        DmCloseLoadedModules( pWalkMod );

        DWORD endId = 0;
        dumpFile->write(&endId,sizeof(endId));

        u32 size = m_callstack.size();
        dumpFile->write(&size,sizeof(size));

        mapMemoryTracker<u_long,TraceInfo*>::iterator iterCallstack =  m_callstack.begin();

        u32 callstackSize =m_callstack.size();

        u32 sizeRequiredLine = sizeof(u32)+sizeof(TraceInfo)+sizeof(maxBackTrace);
        u8 *byteWork =(u8*) ::malloc(sizeRequiredLine* callstackSize);
        u8 *bufferPosition = byteWork;

        for (;iterCallstack!=m_callstack.end();iterCallstack++)
        {
            TraceInfo* info = (*iterCallstack).second;
            u32 crc = (*iterCallstack).first;

            ITF_Memcpy(bufferPosition,&crc,sizeof(crc));
            bufferPosition+=sizeof(crc);

            ITF_Memcpy(bufferPosition,&maxBackTrace,sizeof(maxBackTrace));
            bufferPosition+=sizeof(maxBackTrace);

            ITF_Memcpy(bufferPosition,info,sizeof(TraceInfo));
            bufferPosition+=sizeof(TraceInfo);
        }

        dumpFile->write(byteWork,sizeRequiredLine* callstackSize);
        ::free(byteWork);

        const u32  countAllocation = m_allocationMap.GetNbActivePairs();
        dumpFile->write(&countAllocation,sizeof(countAllocation));


        u32 allocationSizeBuffer = countAllocation* (sizeof(u64)+sizeof(u32));
        byteWork =(u8*) ::malloc(allocationSizeBuffer);
        bufferPosition = byteWork;

        for (u32 index = 0;index<countAllocation;++index)
        {
            const UserPair* pair = m_allocationMap.GetActivePairs()+index;
            u32 address = ((pair->mID0)<<16) | (pair->mID1);
            u64 memchk  = pair->mUserData;
            /*
            u32 address = (*iter).first;
            u64 memchk = (*iter).second;*/
            /*
            u32 size  = GET_SIZECHUNCK(memchk);
            u32 crc   = GET_CRCCHUNCK(memchk);*/


            ITF_Memcpy(bufferPosition,&address,sizeof(u32));
            bufferPosition+=sizeof(u32);

            ITF_Memcpy(bufferPosition,&memchk,sizeof(u64));
            bufferPosition+=sizeof(u64);
        }

        dumpFile->write(byteWork,allocationSizeBuffer);
        ::free(byteWork);

        f32 timetosave = (f32)(SYSTEM_ADAPTER->getTime()-rs);
        LOG("time to save:%0.02f %ds",timetosave,sizeRequiredLine* callstackSize);

    }

    FILEMANAGER->closeFile(dumpFile);
#endif //ITF_X360
}


#ifdef _DEBUG
#pragma optimize ("g", off)
#endif //_DEBUG

void threadAllocatorTracker::dumpAllocationShutdownInternal()
{ 
    //only supported on WIN32 right now
    OutputDebugger dbg;
    if  (!m_bEnable)
    {
        dbg << "ITF leaks detector disabled" << "\n";
    }
    else
    {
        m_bEnable = bfalse;
    
#ifdef ITF_WINDOWS
        /*****************************************************************************************/
        /*****************************************************************************************/
        mapMemoryTracker<u32,u64>::iterator iter = m_map.begin();
        
        ProcessAddressDescriptor desc;

        dbg << "WARNING ITF ... detected memory leaks! " <<m_allocationMap.GetNbActivePairs()  << "\n";

        const u32  countAllocation = m_allocationMap.GetNbActivePairs();
        //for (;iter!=m_map.end();++iter)
        for (u32 index = 0;index<countAllocation;++index)
        {
            const UserPair* pair = m_allocationMap.GetActivePairs()+index;
            uPtr address = ((pair->mID0)<<16) | (pair->mID1);
            u64 memchk  = pair->mUserData;
            /*
            u32 address = (*iter).first;
            u64 memchk = (*iter).second;*/
            
            uPtr size  = GET_SIZECHUNCK(memchk);
            u32 crc   = GET_CRCCHUNCK(memchk);

            dbg << "\n\n****************************************************************************************\n";
            dbg << size << " byte(s) at 0x" << hex(address) << "\n";

            mapMemoryTracker<u_long,TraceInfo*>::iterator iterCallstack = m_callstack.find(crc);
            
            if (iterCallstack != m_callstack.end())
            {
             
                TraceInfo* pTrace = (*iterCallstack).second;

                u32 index = 0;
                while (index < MAX_STACK_DEPTH)
                {
                    u64 addr = pTrace->m_stack[index++];
                    if (addr)
                    {
                        m_stackWalker.fillDescriptor(addr,desc);

                        if (desc.m_line>0)
                        {
                           dbg <<desc.m_filename << "(" << desc.m_line <<") : " << desc.m_symbolName << "\n";
                        }
                    }
                    else
                    {
                        break;
                    }

                }
            }
            else
            {
                dbg << "Call stack not found!!\n";
                ITF_ASSERT(0);
            }

        }

        dbg << "\n\n****************************************************************************************\n";

        mapMemoryTracker<u_long,TraceInfo*>::iterator iterCallstack =  m_callstack.begin();

        for (;iterCallstack!=m_callstack.end();++iterCallstack)
        {
            TraceInfo* pTrace = (*iterCallstack).second;
            ::free(pTrace);
        }
#endif // ITF_WINDOWS
    }   
}
}
#endif //ITF_TRACKING_MEMORY_ENABLE
