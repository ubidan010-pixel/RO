#ifndef ITF_VERTEX_SHADER_CACHE_GCM_H_
#define ITF_VERTEX_SHADER_CACHE_GCM_H_

#include "GCM_API.h"

#include "ShaderProgram_GCM.h"

namespace ITF
{
    namespace GCM
    {
        // this class goal is to manage the vertex shader instruction cache
        // in order to avoid excessive shader changes.
        // Note that right now, no branch instructions are allowed int the shader
        // (cellGcmCgSetInstructionSlot or cellGcmCgSetInstructionSlotUCode is NOT called)
        // Also, user clip and point size attrib output mask is not handled
        class VertexShaderCache
        {
            enum 
            {
                NbInstructionSlotsInCache = 512u
            };

        public:
            enum 
            {
                InvalidInstructionSlot = u16(-1)
            };

            VertexShaderCache();

            inline void setShader(cell::Gcm::Inline::CellGcmContext * _gcmCtxt, const VertexProgram & _program);

            // empty the cache
            void flush();

        private:
            u32 allocProgramSlot(const VertexProgram & _program); // return slot location
            void checkValidity(); // for debug
            void loadProgram(cell::Gcm::Inline::CellGcmContext * _gcmCtxt, const VertexProgram & _program, u32 _loadSlot); // put the program in the slot

            struct CacheEntry
            {
                const VertexProgram * program; // Or NULL if free
                // Dup from what is in program (for cache miss efficiency)
                u16 firstSlot;
                u16 nbSlot;
            };
            typedef Vector<CacheEntry> CacheEntries;
            CacheEntries m_cacheEntries;
            u32 m_olderCacheEntry; // start to search from a new place from this position
        };

        inline void VertexShaderCache::setShader(cell::Gcm::Inline::CellGcmContext * _gcmCtxt, const VertexProgram & _program)
        {
            u32 slot = _program.instructionSlot;
            if (slot==InvalidInstructionSlot)
            {
                slot = allocProgramSlot(_program);
                ITF_ASSERT(_program.instructionSlot!=InvalidInstructionSlot);
                loadProgram(_gcmCtxt,_program, slot);
            }
            _gcmCtxt->SetVertexProgramRegisterCount(_program.registerCount);
            _gcmCtxt->SetVertexAttribInputMask(_program.attributeInputMask);
            // The following is not useful unless using clip planes or point size -> set by SetFragmentProgram or SetFragmentProgramLoad
            // _gcmCtxt->SetVertexAttribOutputMask(_program.attrOutputMask);
            ITF_ASSERT(slot!=InvalidInstructionSlot);
            ITF_ASSERT(slot==_program.instructionSlot);
            _gcmCtxt->SetVertexProgramStartSlot(slot);
        }
    }
}

#endif // ITF_VERTEX_SHADER_CACHE_GCM_H_
