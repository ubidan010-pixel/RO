#ifndef ITF_CORE_PURSJOB_H_
#define ITF_CORE_PURSJOB_H_

#include "Purs_PS3.h"
#include "PursUtil_PS3.h"

#include <cell/spurs/types.h>
#include <cell/spurs/job_descriptor.h>
#include <cell/spurs/job_commands.h>
#include <cell/spurs/job_chain.h>
#include <cell/spurs/workload.h>

#define PURS_DECLARE_JOB_CODE(ProjectName) \
    extern char _binary_job_##ProjectName##_bin_start[]; \
    extern char _binary_job_##ProjectName##_bin_size[]; \
    extern const Purs::JobCode ProjectName

#define PURS_IMPLEMENT_JOB_CODE(ProjectName) \
    const Purs::JobCode ProjectName(#ProjectName, \
                                    _binary_job_##ProjectName##_bin_start, \
                                    CELL_SPURS_GET_SIZE_BINARY(_binary_job_##ProjectName##_bin_size));

#define ASSIGN_JOB_CODE(jobCode, ProjectName) \
    {                                       \
    extern char _binary_job_##ProjectName##_bin_start[]; \
    extern char _binary_job_##ProjectName##_bin_size[]; \
    jobCode = Purs::JobCode (#ProjectName, \
    _binary_job_##ProjectName##_bin_start, \
    CELL_SPURS_GET_SIZE_BINARY(_binary_job_##ProjectName##_bin_size)); \
    }


namespace ITF
{

namespace Purs
{
    enum { MaxSizeOfDmaListElt = 16u*1024u };

    // C++ object style mapping of spurs job concepts

    //////////////////////////////////////////////////////////////////////////    
    // To handle code binary of a job
    //////////////////////////////////////////////////////////////////////////
    void RegisterJobCode(const char * jobName, struct JobCode * jobCode);
    void UpdateJobCode(u32 spuBinCrc, char * spuBin, u32 newBinSize);    

    struct JobCode
    {
        u64 m_eaStart;
        u16 m_size;

        JobCode(char* _pBinStart, u32 _binSize)
        : m_eaStart(u64(_pBinStart)),
          m_size(_binSize)
        {
        }

        JobCode(const char * jobName, char* _pBinStart, u32 _binSize)
        : m_eaStart(u64(_pBinStart)),
          m_size(_binSize)
        {
            RegisterJobCode(jobName, this);
        }
        
        JobCode()
        {}        

        void UpdateCode(char* _pBinStart, u32 _binSize)
        {
            m_eaStart = u64(_pBinStart);
            m_size    = _binSize;
        }

		bool operator ==(const JobCode& rhs) const
		{
			return (m_eaStart == rhs.m_eaStart);
		}

        bool operator !=(const JobCode& rhs) const
        {
            return (m_eaStart != rhs.m_eaStart);
        }
    };
    
    //////////////////////////////////////////////////////////////////////////
    // Different kind of streams
    //////////////////////////////////////////////////////////////////////////
    template <typename T> // T is used only to differentiate types
    class JobStream
    {
    public:
        JobStream(const void * addr, u32 sizeOfBuffer)
            :   m_addr(const_cast<void *>(addr)), m_size(sizeOfBuffer)
        {
            CheckValidity();
        }

        u32 GetSize() const { return m_size; }
        void * GetAddr() const { return m_addr; }

        void CheckValidity()
        {
            ITF_ASSERT((0xF & u32(m_addr)) == 0); // correct ptr alignment
            ITF_ASSERT((0xF & m_size) == 0); // correct size alignment
        }

    private:
        void * m_addr;
        u32 m_size;
    };

    // dummy class to create unique types from the template
    class  JobInputType {};
    class  JobInputOutputType {};
    class  JobOutputType {};
    class  JobReadOnlyInputType {};

    typedef JobStream<JobInputType> JobInput;
    typedef JobStream<JobInputOutputType> JobInputOutput;
    typedef JobStream<JobInputOutputType> JobOutput;
    typedef JobStream<JobReadOnlyInputType> JobReadOnlyInput;

    // template version of the job to avoid copy paste of code for 64/128/256 jobs
    template<class SpursJobStruct>
    class JobTpl : public SpursJobStruct
    {
    public:
        using SpursJobStruct::header;
        using SpursJobStruct::workArea;

        JobTpl() { Reset(); }

        JobTpl(const JobCode & jobCode)
        {
            ITF_ASSERT((u32(this) & 0xF)==0); // correct alignment
            ITF_ASSERT(PursUtil::IsHeapArea((void *)this)); // not stack allocated !

            memset(reinterpret_cast<void *>(this), 0, sizeof(SpursJobStruct));
            SetCode(jobCode);
        }
        
        inline void PatchStream(const u16 offset1stDmaListEntry, void * newAddr, const u32 streamSize)
        {
            // Patch DMA list entries with the combination of the new address and the new size
            u64 *   firstDmalistEntry = workArea.dmaList + offset1stDmaListEntry;
            SplitInDMAListElt(newAddr, streamSize, firstDmalistEntry);
        }

        inline void PatchStream(const u16 offset1stDmaListEntry, const u16 nbDmaListEntries, void * newAddr, const u32 newSize)
        {
            // Make sure the new size do not take more dma list entries than the original stream size
            ITF_ASSERT((newSize / (16 * 1024u)) <= nbDmaListEntries);

            // Patch DMA list entries with the combination of the new address and the new size
            u64 *   firstDmalistEntry = workArea.dmaList + offset1stDmaListEntry;
            const u32 nbEntriesFilled   = SplitInDMAListElt(newAddr, newSize, firstDmalistEntry);

            // Invalidate any dma entry that might be left empty when the new size is at leat 16Kb smaller than the original stream size            
            u64 *             dmaListEntryToInvalidate = firstDmalistEntry + nbEntriesFilled;
            const u64 *       dmaListEnd               = firstDmalistEntry + nbDmaListEntries;
            
            while (dmaListEntryToInvalidate < dmaListEnd) 
            {
                *dmaListEntryToInvalidate = 0;
                ++dmaListEntryToInvalidate;
            }
        }

        void Reset()
        {
            memset(reinterpret_cast<void *>(this), 0, sizeof(SpursJobStruct));
        }

        void SetCode(const JobCode & jobCode)
        {
            header.eaBinary     = jobCode.m_eaStart;
            header.sizeBinary   = jobCode.m_size;
        }

        u32 SetOutputData(u32 numOfEntry, const JobOutput & jobOutput)
        {
            CheckValidity(numOfEntry, jobOutput.GetAddr(), jobOutput.GetSize());
			ITF_ASSERT(PursUtil::IsHeapArea(jobOutput.GetAddr()));	// Not in stack
            u32 nbEltAdded = SplitInDMAListElt(jobOutput.GetAddr(), jobOutput.GetSize(), workArea.dmaList+numOfEntry);
            header.sizeOut += jobOutput.GetSize();
            return numOfEntry+nbEltAdded;
        }

        u32 SetInputData(u32 numOfEntry, const JobInput & jobInput)
        {
            CheckValidity(numOfEntry, jobInput.GetAddr(), jobInput.GetSize());
            ITF_ASSERT(PursUtil::IsHeapArea(jobInput.GetAddr()));	// Not in vram or stack
            ITF_ASSERT(header.useInOutBuffer == 0); // Input not compatible with output
            u32 nbEltAdded = SplitInDMAListElt(jobInput.GetAddr(), jobInput.GetSize(), workArea.dmaList+numOfEntry);
            header.sizeDmaList += nbEltAdded*sizeof(workArea.dmaList[0]);
            header.sizeInOrInOut += jobInput.GetSize();
            return numOfEntry+nbEltAdded;
        }

        u32 SetInputOutputData(u32 numOfEntry, const JobInputOutput & jobInputOutput)
        {
            CheckValidity(numOfEntry, jobInputOutput.GetAddr(), jobInputOutput.GetSize());
			ITF_ASSERT(PursUtil::IsHeapArea(jobInputOutput.GetAddr()));	// Not in vram or stack
            ITF_ASSERT(header.useInOutBuffer!=0 || header.sizeInOrInOut==0); // we already got input and trying to add input/output !
            u32 nbEltAdded = SplitInDMAListElt(jobInputOutput.GetAddr(), jobInputOutput.GetSize(), workArea.dmaList+numOfEntry);
            header.useInOutBuffer = 1;
            header.sizeDmaList += nbEltAdded*sizeof(workArea.dmaList[0]);
            header.sizeInOrInOut += jobInputOutput.GetSize();
            return numOfEntry+nbEltAdded;
        }

        u32 SetReadOnlyInputData(u32 numOfEntry, const JobReadOnlyInput & jobROInput)
        {
            ITF_ASSERT(numOfEntry<sizeof(workArea.dmaList)/sizeof(workArea.dmaList[0]));
            ITF_ASSERT(header.sizeCacheDmaList<sizeof(workArea.dmaList[0]) * 4); // only 4 RO input data allowed
            // no split for cached data
            header.sizeCacheDmaList += sizeof(workArea.dmaList[0]); 
            u64 dmaListEntry =    u64(u32(jobROInput.GetAddr()))
                                +   ((u64(jobROInput.GetSize())) << 32ull);
            workArea.dmaList[numOfEntry] = dmaListEntry;
            return numOfEntry+1;
        }

        u32 SetUserData(u32 numOfEntry, u64 usrData)
        {
            ITF_ASSERT(numOfEntry>=0 && numOfEntry<sizeof(workArea.userData)/sizeof(workArea.userData[0]));
            workArea.userData[numOfEntry] = usrData;
            return numOfEntry+1;
        }

        void AddScratchSize(u32 scratchSize)
        {
            header.sizeScratch += scratchSize;
        }

        static u32 GetNbEntry() { return (GetJobSize() - sizeof(CellSpursJobHeader))/sizeof(u64); }
        static u32 GetJobSize() { return sizeof(SpursJobStruct); }

        // client responsability to have enough entry in dmaList array
        static u32 SplitInDMAListElt(void * addr, u32 size, u64 * dmaList)
        {
            u32 nbChunk = 0;
            char * byteAddr = reinterpret_cast<char *>(addr);
            u64 dmaListEntry;

            // make the loop in 64 bits
            u64 lMaxSizeOfDmaListElt = u64(MaxSizeOfDmaListElt);
            u64 lSize = u64(size);

            u64 shiftedMaxSizeOfDmaListElt = lMaxSizeOfDmaListElt << 32ull;

            for(
                ;
                lSize > lMaxSizeOfDmaListElt;
                lSize -= lMaxSizeOfDmaListElt, byteAddr += lMaxSizeOfDmaListElt, ++nbChunk
               )
            {
                dmaList[nbChunk] = u64(u32(byteAddr)) + shiftedMaxSizeOfDmaListElt;
            }
            // ok here we have to set the last chunk
            ITF_ASSERT(BitTweak::GetAboveAlign(u32(lSize), 16) == lSize);
            dmaList[nbChunk] = u64(u32(byteAddr)) + (lSize << 32ull);
            return nbChunk+1;
        }


        void CheckValidity(u32 numEntry, void * pData, u32 size)
        {
            ITF_ASSERT(numEntry<sizeof(workArea.userData)/sizeof(workArea.userData[0]));
            ITF_ASSERT((0xF & u32(pData)) == 0); // correct ptr alignment
            ITF_ASSERT(numEntry + BitTweak::GetAboveAlign(size, MaxSizeOfDmaListElt)/(MaxSizeOfDmaListElt)
                        < sizeof(workArea.dmaList)/sizeof(workArea.dmaList[0])); // enough entry available for splitting
        }

    };

    // The 3 kind of jobs

    class Job64 : public JobTpl<CellSpursJob64>
    {
    public:
        PURS_DECLARE_MEMORY_OPERATORS_ALIGNED(16);

        Job64()
        {
            ITF_ASSERT((u32(this)&15u) == 0); // Spurs Job not aligned
        }

        Job64(const JobCode & jobCode)
            : JobTpl<CellSpursJob64>(jobCode)
        {
            ITF_ASSERT((u32(this)&15u) == 0); // Spurs Job not aligned
        }
    }  __attribute__((aligned(16)));

    class Job128 : public JobTpl<CellSpursJob128>
    {
    public:
        PURS_DECLARE_MEMORY_OPERATORS_ALIGNED(128);

        Job128()
        {
            ITF_ASSERT((u32(this)&127u) == 0); // Spurs Job not aligned
        }

        Job128(const JobCode & jobCode)
            : JobTpl<CellSpursJob128>(jobCode)
        {
            ITF_ASSERT((u32(this)&127u) == 0); // Spurs Job not aligned
        }
    }  __attribute__((aligned(128)));

    class Job256 : public JobTpl<CellSpursJob256>
    {
    public:
        PURS_DECLARE_MEMORY_OPERATORS_ALIGNED(128);

        Job256()
        {
            ITF_ASSERT((u32(this)&127u) == 0); // Spurs Job not aligned
        }

        Job256(const JobCode & jobCode)
            : JobTpl<CellSpursJob256>(jobCode)
        {
            ITF_ASSERT((u32(this)&127u) == 0); // Spurs Job not aligned
        }
    }   __attribute__((aligned(128)));

    // job list, templated by one of the kind of job above
    template <class JobN>
    class JobListPtr : public CellSpursJobList
    {
    public :
        PURS_DECLARE_MEMORY_OPERATORS_ALIGNED(16);

        JobListPtr(JobN * arrayOfJob, u32 nbJob) 
        {
            ITF_ASSERT((u32(this) & 0xF)==0); // correct alignment
            ITF_ASSERT(PursUtil::IsHeapArea((void *)this)); // not stack allocated !
            numJobs = nbJob;
            sizeOfJob = GetJobSize();
            eaJobList = reinterpret_cast<u64>(arrayOfJob);
        }

        static u32 GetJobSize() { return JobN::GetJobSize(); }

    };

    // job list, templated by one of the kind of job above
    template <class JobN>
    class JobList : public CellSpursJobList
    {
    public :
        PURS_DECLARE_MEMORY_OPERATORS_ALIGNED(16);

        JobList(u32 nbJob)
        {
            ITF_ASSERT((u32(this) & 0xF)==0); // correct alignment
            ITF_ASSERT(PursUtil::IsHeapArea((void *)this)); // not stack allocated !
            numJobs = nbJob;
            sizeOfJob = GetJobSize();
            JobN * jobs = reinterpret_cast<JobN *>(Memory::alignedMalloc(nbJob*sizeof(JobN), 128));
            for(u32 i = 0; i<numJobs; ++i)
            {
                new(jobs+i) JobN(); // placement new
            }
            eaJobList = u64(jobs);
        }

        JobList(u32 nbJob, const JobCode & jobCode)
        {
            ITF_ASSERT((u32(this) & 0xF)==0); // correct alignment
            ITF_ASSERT(PursUtil::IsHeapArea((void *)this)); // not stack allocated !
            numJobs = nbJob;
            sizeOfJob = GetJobSize();
            JobN * jobs = reinterpret_cast<JobN *>(Memory::alignedMalloc(nbJob*sizeof(JobN), 128));
            for(u32 i = 0; i<numJobs; ++i)
            {
                new(jobs+i) JobN(jobCode); // placement new
            }
            eaJobList = u64(jobs);
        }

        ~JobList()
        {
            JobN * jobs = reinterpret_cast<JobN *>(eaJobList);
            for(u32 i = 0; i<numJobs; ++i)
            {
                (jobs+(numJobs-1))->~JobN();
            }
            Memory::alignedFree(jobs);
        }

        JobN & operator [] (unsigned int index)
        {
            return (reinterpret_cast<JobN *>(eaJobList))[index];
        }

        static u32 GetJobSize() { return JobN::GetJobSize(); }

    };



    // job list dynamic version
    template <class JobN>
    class JobListDyn : public CellSpursJobList // dynamic version
    {
        void IncreaseArray()
        {
            // we multiply size by 2 every time we reach a pow2
            u32 newSize = numJobs*2;
            if (numJobs==0)
            {
                newSize = 1;
            }
            JobN * newArray = reinterpret_cast<JobN *>(Memory::alignedMalloc(newSize*sizeof(JobN), 128));
            JobN * oldArray = reinterpret_cast<JobN *>(eaJobList);
            memcpy(newArray, oldArray, numJobs*sizeof(JobN));
            Memory::alignedFree(oldArray);
            eaJobList = u64(newArray);
        }

    public :
        PURS_DECLARE_MEMORY_OPERATORS_ALIGNED(16);

        JobListDyn() 
        {
            ITF_ASSERT((u32(this) & 0xF)==0); // correct alignment
            ITF_ASSERT(PursUtil::IsHeapArea((void *)this)); // not stack allocated !
            numJobs = 0;
            sizeOfJob = GetJobSize();
            eaJobList = 0;
        }

        ~JobListDyn()
        {
            JobN * jobArray = reinterpret_cast<JobN *>(eaJobList);

            // In the case of a user error, the SPURS JOB system is still reading stuff,
            // so we set everything to 0 to get more immediate crash
            numJobs = 0; 
            eaJobList = 0;
            sizeOfJob = 0;
            
            // call destructors
            for(u32 i = 0; i<numJobs; ++i)
            {
                (jobArray+i)->~JobN();
            }

            Memory::alignedFree((void *)jobArray);
        }

        void Add(const JobN & job) // do not add jobs while the list is being processed
        {
            if ((numJobs&(numJobs-1))==0) // pow2 ?
                IncreaseArray();
            numJobs++;
            JobN * jobArray = reinterpret_cast<JobN *>(eaJobList);
            jobArray[numJobs-1] = job;
        }

        JobN & Add(const JobCode & jobCode) // beware that the return job ref is invalidated as soon as another Add is called
        {
            if ((numJobs&(numJobs-1))==0) // pow2 ?
                IncreaseArray();
            JobN * newJob = reinterpret_cast<JobN *>(eaJobList) + numJobs;
            numJobs++;
            new(newJob) JobN(jobCode); // placement new
            return *newJob;
        }

        static u32 GetJobSize() { return JobN::GetJobSize(); }

    };

    // job command
    template <class JobN>
    class JobCommand
    {
    public:

        PURS_DECLARE_MEMORY_OPERATORS_ALIGNED(8);

        JobCommand()
            : m_jobCmd(CELL_SPURS_JOB_COMMAND_NOP)
        {}

        JobCommand(const JobCommand & jobCmd)
            : m_jobCmd(jobCmd.m_jobCmd)
        {}

        JobCommand(u64 cmd)
            : m_jobCmd(cmd)
        {}

        JobCommand(const JobList<JobN> & jobList)
            : m_jobCmd(CELL_SPURS_JOB_COMMAND_JOBLIST(&jobList))
        {}

        JobCommand(const JobListDyn<JobN> & jobListDyn)
            : m_jobCmd(CELL_SPURS_JOB_COMMAND_JOBLIST(&jobListDyn))
        {}

        JobCommand(const JobN & job)
            : m_jobCmd(CELL_SPURS_JOB_COMMAND_JOB(&job))
        {}

        JobCommand(JobCommand * newCmd)
            : m_jobCmd(CELL_SPURS_JOB_COMMAND_NEXT(newCmd))
        {}


        static const JobCommand<JobN> Sync;
        static const JobCommand<JobN> SetLabel;
        static const JobCommand<JobN> SyncLabel;
        static const JobCommand<JobN> End;
        static const JobCommand<JobN> Flush;
        static const JobCommand<JobN> Nop;
        static const JobCommand<JobN> Ret;

    protected:
        u64 m_jobCmd;
    };

    template<class JobN>
        const JobCommand<JobN> JobCommand<JobN>::Sync = CELL_SPURS_JOB_COMMAND_SYNC;
    template<class JobN>
        const JobCommand<JobN> JobCommand<JobN>::SetLabel = CELL_SPURS_JOB_COMMAND_SET_LABEL(0);
    template<class JobN>
        const JobCommand<JobN> JobCommand<JobN>::SyncLabel = CELL_SPURS_JOB_COMMAND_SYNC_LABEL(0);
    template<class JobN>
        const JobCommand<JobN> JobCommand<JobN>::End = CELL_SPURS_JOB_COMMAND_END;
    template<class JobN>
        const JobCommand<JobN> JobCommand<JobN>::Flush = CELL_SPURS_JOB_COMMAND_FLUSH;
    template<class JobN>
        const JobCommand<JobN> JobCommand<JobN>::Nop = CELL_SPURS_JOB_COMMAND_NOP;
    template<class JobN>
        const JobCommand<JobN> JobCommand<JobN>::Ret = CELL_SPURS_JOB_COMMAND_RET;


    // dynamic job command list
    template <class JobN>
    class JobCommandList
    {
    public:

        JobCommandList()
        {
            m_vecCmd.reserve(10);
        }

        void AddCommand(const JobCommand<JobN> & jobCmd)
        {
            m_vecCmd.push_back(jobCmd);
        }

        void SetCommandAt(u32 index, const JobCommand<JobN> & jobCmd)
        {
            m_vecCmd[index] = jobCmd;
        }

        const u64 * GetPtr() const { return reinterpret_cast<const u64 *>(&(*m_vecCmd.Begin())); } // be ware to not trust it between AddCommand

    protected:
        typedef Vector<JobCommand<JobN> > CommandList;
        CommandList m_vecCmd;
    };

    class JobChainDesc // use this class if you want to set complex JobChain parameters
    {
    public:
        enum { ms_nbCharsInName = 32 };

        JobChainDesc(SpursInstance * spursInstance, char chainName[ms_nbCharsInName])
        :   maxGrabJob(1), 
            autoReadyCount(true), 
            tag1(0), 
            tag2(1), 
            isFixedMemAlloc(false), 
            maxSizeJob(256), 
            m_spursInstance(spursInstance)
        {
            ITF_ASSERT(m_spursInstance != NULL);             
            maxContention = m_spursInstance->GetNbSpuAssigned();
            readyCount    = m_spursInstance->GetNbSpuAssigned();

            for (u32 i = 0; i < 8; ++i)
            {
                priorityTable[i] = 8;
            }

            strncpy(m_name, chainName, ms_nbCharsInName);
            m_name[31] = 0;
        }

        JobChainDesc(const JobChainDesc & jobChainBasicDesc, char chainName[ms_nbCharsInName])
        : maxGrabJob(jobChainBasicDesc.maxGrabJob), 
          maxContention(jobChainBasicDesc.maxContention), 
          autoReadyCount(jobChainBasicDesc.autoReadyCount), 
          tag1(jobChainBasicDesc.tag1), 
          tag2(jobChainBasicDesc.tag2), 
          isFixedMemAlloc(jobChainBasicDesc.isFixedMemAlloc), 
          maxSizeJob(jobChainBasicDesc.maxSizeJob), 
          readyCount(jobChainBasicDesc.readyCount),
          m_spursInstance(jobChainBasicDesc.m_spursInstance)
        {
            memcpy(priorityTable, jobChainBasicDesc.priorityTable, sizeof(priorityTable));

            strncpy(m_name, chainName, ms_nbCharsInName);
            m_name[31] = 0;
        }

        u16         maxGrabJob;
        u8          priorityTable[8];
        u32         maxContention;
        bool        autoReadyCount;
        u32         tag1;
        u32         tag2;
        bool        isFixedMemAlloc;
        u32         maxSizeJob;
        u32         readyCount;
        SpursInstance *    m_spursInstance;

        char        m_name[ms_nbCharsInName];
    };

    template <class JobN>
    class JobChain : public CellSpursJobChain
    {
    private:
        JobChain(const JobChain<JobN> & jobChain)
        {
            // copy not allowed
        }

    public:
        PURS_DECLARE_MEMORY_OPERATORS_ALIGNED(128);

        JobChain(const JobCommand<JobN> * jobCmdList, const JobChainDesc & desc)
        {
            ITF_ASSERT((u32(this) & 0x7F)==0); // correct alignment
            ITF_ASSERT(PursUtil::IsHeapArea((void *)this)); // not stack allocated !

            CellSpursJobChainAttribute attr;
            SYS_ERROR_CHECK(cellSpursJobChainAttributeInitialize(&attr,
                                                                reinterpret_cast<u64 *>(jobCmdList),
                                                                JobN::GetJobSize(),
                                                                desc.maxGrabJob,
                                                                desc.priorityTable,
                                                                desc.maxContention,
                                                                desc.autoReadyCount,
                                                                desc.tag1, desc.tag2,
                                                                desc.isFixedMemAlloc,
                                                                desc.maxSizeJob,
                                                                desc.readyCount));

            strncpy(m_name, desc.m_name, CJobChainDesc::ms_nbCharsInName);
            m_name[CJobChainDesc::ms_nbCharsInName-1u] = 0;
            SYS_ERROR_CHECK(cellSpursJobChainAttributeSetName(&attr, m_name));

            // call with JobChainDesc params
            SYS_ERROR_CHECK(
                cellSpursCreateJobChainWithAttribute(const_cast<CellSpurs*>(desc.m_spursInstance->GetSpurs()),
                                                    this,
                                                    &attr));
        }

        JobChain(const JobCommandList<JobN> & cmdList, SpursInstance * spursInstance)
        {
            ITF_ASSERT(spursInstance != NULL);


            static const u8 spuWorkloadPriorities[8] = {8,8,8,8,8,8,8,8}; // middle of priority range for everyone
            const u16 maxGrabbedJob = 16;
            const u32 maxContention = spursInstance->GetNbSpuAssigned();

            CellSpursJobChainAttribute attr;
            SYS_ERROR_CHECK(cellSpursJobChainAttributeInitialize(&attr,
                                                                cmdList.GetPtr(),
                                                                JobN::GetJobSize(),
                                                                maxGrabbedJob,
                                                                spuWorkloadPriorities,
                                                                maxContention,
                                                                true,
                                                                0, 1,
                                                                false,
                                                                256,
                                                                spursInstance->GetNbSpuAssigned()));

            // call with JobChainDesc params
            SYS_ERROR_CHECK(cellSpursCreateJobChainWithAttribute(spursInstance->GetSpurs(),
                                                                this,
                                                                &attr));
        }

        void Kick()
        {
            SYS_ERROR_CHECK(cellSpursRunJobChain(this));
        }

        void Shutdown() // call shutdown before destruction
        {
            SYS_ERROR_CHECK(cellSpursShutdownJobChain(this));
            SYS_ERROR_CHECK(cellSpursJoinJobChain(this));
        }

        char        m_name[JobChainDesc::ms_nbCharsInName];
    };
}
}

#endif // #ifndef _CORE_PURSJOB_H_
