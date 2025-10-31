#include "core/enginecommon.h"

#include "textureCookerHRes.h"

#include <nvcore/StrLib.h>
#include <nvcore/StdStream.h>

#include <nvimage/Image.h>
#include <nvimage/DirectDrawSurface.h>

#include <nvtt/nvtt.h>

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_COOKERFACTORY_H_
#include "tools/plugins/CookerPlugin/CookerFactory.h"
#endif //_ITF_COOKERFACTORY_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif //_ITF_VERSIONING_H_

#ifndef _ITF_DDSFORMAT_H_
#include "engine/display/ddsformat.h"
#endif //_ITF_DDSFORMAT_H_

#ifndef _ITF_PROCESSSPAWNER_H_
#include "core/system/ProcessSpawner.h"
#endif //_ITF_PROCESSSPAWNER_H_

#ifndef _ITF_TEXTUREOUTPUTHANDLER_H_
#include "tools/plugins/CookerPlugin/TextureOutputHandler.h"
#endif //_ITF_TEXTUREOUTPUTHANDLER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

namespace ITF
{
    
#ifdef MODE_HRESTEXTURE_SUPPORTED
   
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	bbool textureCookerHRes::cookFile(const String& _vPlatform,const String& _szFilename)
    { 
        return textureCookerHRes::compress(_vPlatform,_szFilename);
    }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    bbool textureCookerHRes::compress(const String& _szPlatform,const String& _szFilename)
    {
        const String& sourceFile        = FILESERVER->getHResVersionName(_szFilename);
        const String& destinationFile   = FILESERVER->getCookedHResVersionName(_szFilename);

        Directory::create(FilePath::getDirectory(destinationFile));

        TextureOutputHandler*  outputHandler = new TextureOutputHandler((wchar_t*)sourceFile.cStr(), 
            (wchar_t*)"hresCook",String8(destinationFile.cStr()).cStr(),(wchar_t*)_szPlatform.cStr());

        if (outputHandler->stream && outputHandler->stream->isError() && SYSTEM_ADAPTER)
        {
            u32 error = GetLastError();
            String tmp;
            tmp.setTextFormat("Error file nvidia %ls / fullpath:%ls error:%d",destinationFile.cStr(),sourceFile.cStr(),error);
            SYSTEM_ADAPTER->messageBox("Error Cooking",tmp,ITF_MSG_ICONERROR,ITF_IDYES);
        }

        nv::Image image;

        if (!image.load(String8(sourceFile.cStr()).cStr()))
        {
            SF_DEL(outputHandler);
            fprintf(stderr, "The file '%ls' is not a supported image type.\n", sourceFile.wcharCStr());
            return bfalse;
        }

        nvtt::Compressor compressor;
        nvtt::InputOptions inputOptions;
        nvtt::OutputOptions outputOptions;
        nvtt::CompressionOptions compressionOptions;
        nvtt::Format format = nvtt::Format_RGBA;
        nvtt::WrapMode wrapModeUse = nvtt::WrapMode_Repeat;
        nvtt::MipmapFilter mipFilter = nvtt::MipmapFilter_Box;
        outputOptions.setOutputHandler(outputHandler);

        inputOptions.setRoundMode(nvtt::RoundMode_ToNextPowerOfTwo);
        inputOptions.setTextureLayout(nvtt::TextureType_2D, image.width(), image.height());

        inputOptions.setMipmapData(image.pixels(), image.width(), image.height(),1,0,0);

        inputOptions.setNormalMap(false);
        inputOptions.setConvertToNormalMap(false);
        inputOptions.setGamma(2.2f, 2.2f);
        inputOptions.setNormalizeMipmaps(false);

        inputOptions.setMipmapGeneration(btrue,-1);


        compressionOptions.setFormat(format);
        inputOptions.setWrapMode(wrapModeUse);

        inputOptions.setMipmapFilter(mipFilter);
        compressionOptions.setQuality(nvtt::Quality_Normal);

        //all options must be setA
        outputHandler->setTotal(compressor.estimateSize(inputOptions, compressionOptions));

        compressor.enableCudaAcceleration(bfalse);

        if (!compressor.process(inputOptions, compressionOptions, outputOptions))
        {
            SF_DEL(outputHandler);
            return bfalse;
        }

        SF_DEL(outputHandler);

        u64 timeWrite   = FILEMANAGER->getLastTimeWriteAccess(sourceFile);
        FILEMANAGER->flushTimeWriteAccess(destinationFile, timeWrite);

        return btrue;
    }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    bbool textureCookerHRes::cudaIsEnable()
    {
        nvtt::Compressor compressor;
        return compressor.isCudaAccelerationEnabled();
    }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif //MODE_HRESTEXTURE_SUPPORTED

}
