#include "precompiled_GFXAdapter_GCM.h"

#include "GCM_API.h"

#ifndef _ITF_PUGIXMLWRAP_H_
#include "core/XML/PugiXMLWrap.h"
#endif //_ITF_PUGIXMLWRAP_H_

#ifndef _ITF_WARNINGMANAGER_H_
    #include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
    #include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#include "VRAMAllocator_GCM.h"
#include "RenderContext_GCM.h"
#include "VertexDecl_GCM.h"
#include "ShaderProgram_GCM.h"
#include "Technique_GCM.h"

namespace ITF
{
    namespace GCM
    {

        Technique::Technique()
            : m_vertexProg(NULL)
            , m_fragmentProg(NULL)
        {}

        Technique::~Technique()
        {
            // TODO : nothing to do, the repository is freeing shaders ?
        }

        i32 Technique::readNode(PugiXMLWrap & _xmlParser, const String & _fxPathAndName)
        {
            const char * techName = _xmlParser.readAttribute("name");
            String techFullName;
            techFullName.setTextFormat("%ls_%s", _fxPathAndName.cStr(), techName);
            m_techFullName = techFullName;

            if (_strnicmp("Tech_", techName, strlen("Tech_")))
            {
                ITF_WARNING(NULL, bfalse, "Cannot get an ordinal number for Technique %ls", m_techFullName.cStr());
                return -1; // unknown technique
            }
            i32 techNum = atoi(techName+strlen("Tech_"));

            //vertex program
            if (_xmlParser.enterElement("pass"))
            {
                u32 vtxCount = 0, fragCount = 0;
                if (_xmlParser.enterElement("entry"))
                {
                    do 
                    {
                        // the first (and supposedly only) element is our entry point
                        _xmlParser.enterFirstElement();
                        const char * entry = _xmlParser.readElementValue();
                        _xmlParser.exitElement();

                        if (!_strcmpi(_xmlParser.readAttribute("type", "Vertex"), "Fragment"))
                        {
                            loadFragmentProgram(_fxPathAndName, entry);
                            fragCount++;
                        }
                        else
                        {
                            loadVertexProgram(_fxPathAndName, entry);
                            vtxCount++;
                        }
                    } while (_xmlParser.nextSameElement(true));
                }
                if (vtxCount!=1 && fragCount!=1)
                {
                    ITF_WARNING(NULL, bfalse, "Entries in fx file %ls ill formed, last taken", _fxPathAndName.cStr());
                }

                // check for a second pass ?
                if (_xmlParser.nextSameElement(false))
                {
                    ITF_WARNING(NULL, bfalse, "Too much pass used in fx file %ls (additionals ignored)", _fxPathAndName.cStr());
                }
                _xmlParser.exitElement();
            }
            else
            {
                ITF_WARNING(NULL, bfalse, "No pass found in a technique in fx file %ls", _fxPathAndName.cStr());
            }
            return techNum;
        }

#ifdef CHECK_VERTEXDECL_SHADER_COMPATIBILITY
        void Technique::checkVertexDeclCompatibility(const GCM::VertexDecl * _vertexDecl)
        {
            if (m_vertexProg)
            {
                m_vertexProg->checkVertexDeclCompatibility(_vertexDecl);
            }
        }
#endif

        void Technique::loadVertexProgram(const String & _fxPathAndName, const char * _entry)
        {
            String vpName;
            vpName.setTextFormat("%ls_%s.vpo", _fxPathAndName.cStr(), _entry);

            m_vertexProg = ShaderProgramRepository::get().getVPO(vpName);
        }

        void Technique::loadFragmentProgram(const String & _fxPathAndName, const char * _entry)
        {
            String fpName;
            fpName.setTextFormat("%ls_%s.fpo", _fxPathAndName.cStr(), _entry);
            m_fragmentProg = ShaderProgramRepository::get().getFPO(fpName);
        }

        bool Technique::bind(const FxParameterDB & _paramDB)
        {
            bool successVtx = m_vertexProg->bind(_paramDB);
            bool successFrag = m_fragmentProg->bind(_paramDB);
            return successVtx && successFrag;
        }
    }
}
