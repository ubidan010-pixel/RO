#ifndef _ITF_ANIMPACKERCONFIGURATION_H_
#define _ITF_ANIMPACKERCONFIGURATION_H_

namespace ITF
{
    namespace AnimTexPacker
    {
        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Configuration of animation packer
        class AnimPackerConfiguration
        {
        public:
            ///////////////////////////////////////////////////////////////////////////////////////////
            /// Description of size constraint on important bone
            struct ImportantBone
            {
                String8 m_boneName;
                int m_sizeInPixel;
            };
            ///////////////////////////////////////////////////////////////////////////////////////////
            ///A PBK merge list
            struct PbkConcatDesc
            {
                ITF_VECTOR<String8> m_pbkFilter;
                ///////////////////////////////////////////////////////////////////////////////////////////
                ///Parsing of PbkConcatDesc (pbk names separated by '+')
                void makeFromString(String8 _desc);
                ///////////////////////////////////////////////////////////////////////////////////////////
                /// does filename match?
                bbool checkFileMatch(const String8 &_pbkBaseName /*WithoutExtension*/) const;
            };

            ///////////////////////////////////////////////////////////////////////////////////////////
            ///Several PBK merge lists
            typedef ITF_LIST<PbkConcatDesc> PbkConcatDescList;

            ///////////////////////////////////////////////////////////////////////////////////////////
            AnimPackerConfiguration()
            : m_maxElementSize(0)
            , m_maxTextureSize(2048)
            , m_mustExportGenAnim(bfalse)
            , m_processBankChange(bfalse)
            , m_useSimpleMerge(bfalse) {}

            ///////////////////////////////////////////////////////////////////////////////////////////
            ///Open configuration file and setup members
            bbool openConfigurationFile(const String &_platform_w, const String &_path);

            ///////////////////////////////////////////////////////////////////////////////////////////
            ITF_VECTOR<ImportantBone>   m_importantBones;
            PbkConcatDescList           m_concatenatedPbkList;
            int                         m_maxElementSize;
            int                         m_maxTextureSize;
            String                      m_mainPath;
            bbool                       m_mustExportGenAnim;
            String                      m_outputPath;
            bbool                       m_processBankChange;
            bbool                       m_useSimpleMerge;

        private:

            //Display error message
            void incorrectConfigurationFile();
            //parse list of pbk concatenations
            void parsePbkConcatList(String8 _desc);
            //Parse important bone declaration : bone=size
            bbool parseImportantBone(const String8 &_importantBoneDecl);
            //
            bbool parseImportantBoneList(String8 _importantBoneList);

        };
    }
}


#endif
