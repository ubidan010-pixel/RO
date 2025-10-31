#ifndef ITF_TECHNIQUE_PS5_H_
#define ITF_TECHNIQUE_PS5_H_

#pragma once

#include "core/types.h"
#include "core/itfstring.h"
#include "ShaderProgram_PS5.h"

namespace ITF
{
    class FxParameterDB;
    class VertexProgram;
    class PixelProgram;
    struct VertexFormatDescriptor;
    class PugiXMLWrap;

    namespace Gnmp
    {
        class ShaderProgramCache
        {
        public:
            // This class is used to cache the last shader program set in the command buffer.
            ShaderProgramCache() = default;
            ~ShaderProgramCache() = default;

            void setIfChanged(Gnmx::GfxContext& _rdrCtxt, VertexProgram* vertexProg);
            void setIfChanged(Gnmx::GfxContext& _rdrCtxt, PixelProgram* vertexProg);

            // reset the cache so that the next call to setIfChanged will reassign the program in the cmd buffer.
            // Call reset each time a shader program is changed without using this cache.
            void reset(); 
        private:
            VertexProgram* m_vertexProg = nullptr;
            PixelProgram* m_pixelProg = nullptr;
        };

        class Technique
        {
        public:

            Technique() = default;
            ~Technique() = default; // not owning the programs (Shader repository role)

            i32 readNode(PugiXMLWrap & _xmlParser, const String & _fxPathAndName); // return technique number

            void loadVertexProgram(const String & _fxPathAndName, const char * _entry);
            void loadFragmentProgram(const String & _fxPathAndName, const char * _entry);
            bool bind(const FxParameterDB & _paramDB); // return false if parameters are missing

            // the following method is checking validity of the technique for a vertex decl
            void checkVertexDeclCompatibility(const VertexFormatDescriptor& _vertexFormatDesc);

            bool isValid() const { return m_vertexProg!=nullptr && m_pixelProg!=nullptr; }
            const VertexProgram * getVertexProgram() const { return m_vertexProg; }
            const PixelProgram* getPixelProgram() const { return m_pixelProg; }

            void set(Gnmx::GfxContext& _rdrCtxt, ShaderProgramCache & shaderCache); // set the technique in the command buffer, using the cache
            void apply(Gnmx::GfxContext & _rdrCtxt, const FxParameterDB & paramDB); // set the parameters in the command buffer (call it after the set)

            const String & getName() const { return m_techFullName; }

        private:

            VertexProgram * m_vertexProg = nullptr;
            PixelProgram * m_pixelProg = nullptr;

            String m_techFullName;
        };
    } // Gnmp
} // ITF

#endif // ITF_TECHNIQUE_PS5_H_
