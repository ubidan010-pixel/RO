#ifndef _ITF_FILEPACKER_H_
#define _ITF_FILEPACKER_H_

namespace ITF
{

class FilePacker
{
public:

    static bbool FilePacker::make(const String& _szRoot,const String& _szPlatform,const ITF_VECTOR<String>& _vFiles);
    //new version that support only isc ,bin and cooked files,the other one will be deprecated soon
    static bbool FilePacker::makeForDLL(const String& _szRoot,const String& _absoluteDstFilename,const String& _szPlatform,int _count,const String** _vFiles,callbackProgressBuild _func,int _dataversion);

    static bbool FilePacker::makeGlobalFatForDll( const String& root, const String& platform, const String& filename, const String& bundlesOutput, const String** bundleNames, u32 count );
  };

}

#endif //_ITF_SCENECOOKER_H_