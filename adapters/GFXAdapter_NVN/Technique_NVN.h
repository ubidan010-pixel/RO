#ifndef ITF_TECHNIQUE_NVN_H_
#define ITF_TECHNIQUE_NVN_H_

#pragma once

#include "core/types.h"
#include "core/itfstring.h"
#include "ShaderProgram_NVN.h"

namespace ITF
{
    class FxParameterDB;
    class VertexProgram;
    class PixelProgram;
    struct VertexFormatDescriptor;
    class PugiXMLWrap;
    class NVNUniformBufferPool;

    namespace NVN
    {
        class Technique;

        class ShaderProgramCache
        {
        public:
            // This class is used to cache the last Technique set in the command buffer.
            ShaderProgramCache() = default;
            ~ShaderProgramCache() = default;

            void setIfChanged(nvn::CommandBuffer& _rdrCtxt, Technique* _technique);

            // reset the cache so that the next call to setIfChanged will reassign the program in the cmd buffer.
            // Call reset each time a shader program is changed without using this cache.
            void reset(); 
        private:
            Technique* m_lastTechnique = nullptr;
        };

        class Technique
        {
        public:

            Technique() = default;
            Technique(const Technique &) = delete;
            ~Technique() = default; // not owning the programs (Shader repository role)

            Technique & operator = (const Technique&) = delete;

            i32 initFromXMLNode(nvn::Device& _device, PugiXMLWrap & _xmlParser, const String & _fxPathAndName); // return technique number

            bool bind(const FxParameterDB & _paramDB); // return false if parameters are missing

            // the following method is checking validity of the technique for a vertex decl
            void checkVertexDeclCompatibility(const VertexFormatDescriptor& _vertexFormatDesc);

            bool isValid() const { return m_nvnProgram != nullptr; }
            const VertexProgram * getVertexProgram() const { return m_vertexProg; }
            const PixelProgram* getPixelProgram() const { return m_pixelProg; }

            nvn::Program * getProgram() const { return m_nvnProgram.get(); }

            void set(nvn::CommandBuffer& _rdrCtxt, ShaderProgramCache & shaderCache); // set the technique in the command buffer, using the cache
            void apply(nvn::CommandBuffer& _rdrCtxt, NVNUniformBufferPool & _uniformBufferPool, const FxParameterDB & paramDB); // set the parameters in the command buffer (call it after the set)

            const String & getName() const { return m_techFullName; }
            const String8 & getName8() const { return m_techFullName8; }

        private:
            void loadVertexProgram(const String& _fxPathAndName, const char* _entry);
            void loadFragmentProgram(const String& _fxPathAndName, const char* _entry);
            void createNVNProgram(nvn::Device& _device); // create the nvn::Program from the vertex and pixel programs

            NVN::UniquePtr<nvn::Program> m_nvnProgram;
            VertexProgram * m_vertexProg = nullptr;
            PixelProgram * m_pixelProg = nullptr;

            NVN::SimpleVRAMRelocHandle m_shaderScratch;

            String m_techFullName;
            String8 m_techFullName8;
        };
    } // NVN
} // ITF

#endif // ITF_TECHNIQUE_NVN_H_
