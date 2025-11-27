#pragma once


#include <msclr\marshal.h>


using namespace System;
//using namespace msclr::interop;
//using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;


#include <windows.h>
#include <vcclr.h>



public delegate void onProgressBundleDelegate(float _progress);
public delegate void onLogDelegate(String^ _progress);

public ref class cookerWrapper
{
public:

    cookerWrapper();
    ~cookerWrapper();

    void destroy();

    bool cook(String^ platform,String^ file);

    void getDependencies(String^ platform, String^ file, List<String^>^ _dependencies);

    void getTopLevelDependencies(String^ platform, String^ file, List<String^>^ _dependencies);

    void getGameDependencies(String^ platform,List<String^>^ _dependencies);

    void getCookedName(String^ source,String^% destination);

    void getCookedNamePlatform(String^ source,String^ platform ,String^% destination);

    void getCookedDirectory(String^ source,String^ platform,String^% destination);

    void buildAtlasContainer(String^ dstFile,String^ platform,List<String^>^ _listFiles);

    void enableCacheFiles();

    String^ getAliasName(String^ _alias);

    void getPattern(String^% destination);

    bool isCookable(String^ file);

    String^ getBundleName(String^ _name, String^ _platform);

    void init(String^ _directoryRoot,String^ _directoryApplication);

    bool bundle(String^ _directoryDestination,String^ platform,List<String^>^ _listFiles,int _version);
    bool buildGlobalFat( String^ destinationFile, String^ platform, String^ bundleOutput, List< String^ >^ bundleFiles );

    onProgressBundleDelegate ^onProgress; 

    onLogDelegate ^onLog; 

    String^ getDataRoot() {return m_DataRoot;}

    void    setCacheKey( String^ key );
    String^ getCacheKey();

    void    addCookersOption(String^ option);
    void    removeCookersOption(String^ option);

    ITF::StringID::StringIdValueType getStringCRC( String^ str );

    static cookerWrapper^ m_instance;

protected:

    IntPtr funcPtr;
    String^ m_DataRoot;
};
