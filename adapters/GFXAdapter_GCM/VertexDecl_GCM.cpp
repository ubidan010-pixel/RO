#include "precompiled_GFXAdapter_GCM.h"
#include "VertexDecl_GCM.h"
#include "core/Math/MathTools.h"

namespace ITF
{
    namespace GCM
    {

        u32 VertexDecl::lastUsageUsed = 0;

        VertexDecl::VertexDecl()
            : nbElements(0)
        {
            cleanAllElements();
        }

        void VertexDecl::addElement(u32 _stream, u32 _offset, const Element::Type & _type, Usage::Enum _usage)
        {
            ITF_ASSERT(nbElements < NbMaxVertexInputAttributes);
            Element & elt = elementArray[nbElements];
            elt.stream = _stream;
            elt.offset = _offset;
            elt.declType = _type;
            elt.usage = _usage;
            nbElements++;
            lastUsageUsed = Max(u32(_usage), lastUsageUsed);
        }

        void VertexDecl::cleanAllElements()
        {
            for(u32 i = 0; i<NbMaxVertexInputAttributes; ++i)
            {
                Element & elt = elementArray[i];
                elt.stream = 0;
                elt.offset = 0;
                elt.declType.baseType = 0;
                elt.declType.size = 0;
                elt.usage = Usage::Position;
            }
        }

        struct SemanticToUsage
        {
            const char * semantic;
            u32 usage;
        };

        static SemanticToUsage semanticToUsage[] =
        {
            {"ATTR0",  0},
            {"ATTR1",  1},
            {"ATTR2",  2},
            {"ATTR3",  3},
            {"ATTR4",  4},
            {"ATTR5",  5},
            {"ATTR6",  6},
            {"ATTR7",  7},
            {"ATTR8",  8},
            {"ATTR9",  9},
            {"ATTR10", 10},
            {"ATTR11", 11},
            {"ATTR12", 12},
            {"ATTR13", 13},
            {"ATTR14", 14},
            {"ATTR15", 15},
            {"POSITION", 0},
            {"BLENDWEIGHT", 1},
            {"NORMAL", 2},
            {"COLOR0", 3},
            {"COLOR1", 4},
            {"FOG", 5},
            {"PSIZE", 6},
            {"BLENDINDICES", 7},
            {"TEXCOORD0", 8},
            {"TEXCOORD1", 9},
            {"TEXCOORD2", 10},
            {"TEXCOORD3", 11},
            {"TEXCOORD4", 12},
            {"TEXCOORD5", 13},
            {"TEXCOORD6", 14},
            {"TEXCOORD7", 15}
        };

        // Tool method -> get a usage from the semantic as a name ("POSITION0", "TEXCOORD2", etc)
        VertexDecl::Usage::Enum VertexDecl::getUsageFromSemantic(const char * _semantic)
        {
            for(u32 i = 0; i<sizeof(semanticToUsage)/sizeof(semanticToUsage[0]); ++i)
            {
                if (!_strcmpi(semanticToUsage[i].semantic, _semantic))
                {
                    return VertexDecl::Usage::Enum(semanticToUsage[i].usage);
                }
            }
            ITF_ASSERT(0); // unknown semantic
            return VertexDecl::Usage::Position;
        }


        const char * usageToSemantic[] =
        {
            "POSITION",
            "BLENDWEIGHT",
            "NORMAL",
            "COLOR0",
            "COLOR1",
            "FOG",
            "PSIZE",
            "BLENDINDICES",
            "TEXCOORD0",
            "TEXCOORD1",
            "TEXCOORD2",
            "TEXCOORD3",
            "TEXCOORD4",
            "TEXCOORD5",
            "TEXCOORD6",
            "TEXCOORD7" // 15
        };


        const char * VertexDecl::getSemanticNameFromUsage(Usage::Enum _usage)
        {
            ITF_ASSERT(Usage::Count == sizeof(usageToSemantic)/sizeof(usageToSemantic[0])); // unknown semantic
            return usageToSemantic[_usage];
        }

    }
}
