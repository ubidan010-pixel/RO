#ifndef _ITF_BUNDLEBUILDER_H_
#define _ITF_BUNDLEBUILDER_H_

#pragma once

#ifdef ITF_WINDOWS

namespace ITF
{
    class FilePackMaster;

    class BundleBuilder
    {
    public:
        typedef ITF_VECTOR< String > FileStack;

        BundleBuilder(const String& _szRoot);
        virtual ~BundleBuilder();

        void addFile(const String& _szFilename);
        void generate(const String& _szFilename,u32 _uPlatform,bbool _bCompressedBundle,callbackProgressBuild _func,int _dataversion);

    private:
        void writeAllFiles(const String& _szRoot,bbool _bundleCompressed,File* _fileWrite,callbackProgressBuild _cbProgress,u32 _uPlatform);

        String          m_Root;
        FilePackMaster* m_Header;
        FileStack       m_FileStack;
    };

}
#endif //ITF_WINDOWS

#endif //_ITF_BUNDLEBUILDER_H_
