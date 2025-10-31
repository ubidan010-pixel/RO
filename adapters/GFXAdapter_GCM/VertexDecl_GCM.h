#if defined (ITF_PS3)

#ifndef ITF_VERTEX_DECL_GCM_H_
#define ITF_VERTEX_DECL_GCM_H_


namespace ITF
{
    namespace GCM
    {
        // Vertex Declaration
        class VertexDecl
        {
        public:

            struct Usage
            {
                enum Enum
                {
                    Position = 0,
                    BlendWeight,
                    Normal,
                    Color0,
                    Color1,
                    Fog,
                    PSize,
                    BlendIndices,
                    TexCoord0,
                    TexCoord1,
                    TexCoord2,
                    TexCoord3,
                    TexCoord4,
                    TexCoord5,
                    TexCoord6,
                    TexCoord7,

                    Count
                };
            };

            struct Element
            {
                struct Type
                {
                    u8 size; // 1-4 (or 0 to deactivate)
                    u8 baseType; // CELL_GCM_VERTEX_*
                };

                u32 stream; // unused, always 0
                u32 offset; // byte offset in the stream
                Type declType;
                Usage::Enum usage; // usage, ie. N in ATTRN
            };

            VertexDecl();
            void addElement(u32 _stream, u32 _offset, const Element::Type & _type, Usage::Enum _usage);
            void cleanAllElements();
            // Tool method -> get a usage from the semantic as a name ("POSITION0", "TEXCOORD2", etc)
            static Usage::Enum getUsageFromSemantic(const char * _semantic);
            // get the classic semantic ("POSITION0", "TEXCOORD2", etc) from the usage
            static const char * getSemanticNameFromUsage(Usage::Enum _usage);

            enum
            {
                NbMaxVertexInputAttributes = 16 // CG constant
            };

            u32 nbElements;
            Element elementArray[NbMaxVertexInputAttributes];
            static u32 lastUsageUsed;
        };
    }
}

#endif // ITF_VERTEX_DECL_GCM_H_

#endif // ITF_PS3
