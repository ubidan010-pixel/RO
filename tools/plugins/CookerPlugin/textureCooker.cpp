#include "core/enginecommon.h"

#include "textureCooker.h"

#include <nvcore/StrLib.h>
#include <nvcore/StdStream.h>

#include <nvimage/Image.h>
#include <nvimage/DirectDrawSurface.h>

#include <nvtt/nvtt.h>

#include "core/UnicodeTools.h"

#include "engine/profiler/ProfilerMacro.h"

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

#include "3DS/textureCooker_3DS.h"

#ifndef _ITF_TEXTURECONFIGURATION_H_
#include "tools/plugins/CookerPlugin/TextureConfiguration.h"
#endif //_ITF_TEXTURECONFIGURATION_H_

#ifndef _ITF_TEXTUREOUTPUTHANDLER_H_
#include "tools/plugins/CookerPlugin/TextureOutputHandler.h"
#endif //_ITF_TEXTUREOUTPUTHANDLER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_DEP_COLLECTOR_H_
#include "engine/dependencies/DepCollector.h"
#endif //_ITF_DEP_COLLECTOR_H_

#include "engine/resources/ResourceManager.h"

namespace ITF
{

// #define XBOX_USE_TILE

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	bbool textureCooker::cookFile(const String& _vPlatform,const String& _szFilename)
    { 
        return textureCooker::compress(_vPlatform,_szFilename,bfalse);
    }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    TextureConfiguration* defaultConfiguration(const String& _szPlatform)
    {
        TextureConfiguration* pTextureConfiguration = new TextureConfiguration();
        pTextureConfiguration->setWrapMode("WrapMode_Repeat");
        pTextureConfiguration->setMipmapFilter("MipmapFilter_Kaiser");
        if (_szPlatform == "IPAD")
        {
            pTextureConfiguration->setCompressionMode("RGBA");
            pTextureConfiguration->setUseMipmap(bfalse);
        }
        else if (_szPlatform == "WII")
        {
            pTextureConfiguration->setCompressionMode("RGBA");
            pTextureConfiguration->setUseMipmap(btrue);
            pTextureConfiguration->setAutoDegrad(btrue);
            pTextureConfiguration->setLodBias(0.f);
            pTextureConfiguration->setAutoDegradAlpha(bfalse);
            pTextureConfiguration->setOneColMode(TextureConfiguration::eAUTO);
            pTextureConfiguration->setAnisotropy(TextureConfiguration::eMAX);
        }
        else
        {
            // $GB: on NX (switch), RL was forcing DXT5 here
            pTextureConfiguration->setCompressionMode("DXT3");
            pTextureConfiguration->setUseMipmap(btrue);
        }

        return pTextureConfiguration;
    }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    bbool onTextureCookFinishWII(const String& temporaryFile, const String& dstFile)
    {
        ProcessSpawner processSpawner;
        String commandline;
        String fullFilePathDST;

        // here dstFile is the temporary dds file
        FILEMANAGER->TranslatePath(fullFilePathDST, dstFile);
        StringConverter destAbsolute(fullFilePathDST);

        commandline.setTextFormat("-i %ls -o \"%s\"", temporaryFile.cStr(), destAbsolute.getChar());
        i32 res = processSpawner.create("TexPostCookWii.exe", commandline, "");
        if (res != 0)
        {
            LOG_COOKER("[TEXTURE COOKER] FAILED TO CONVERT TO WII(err=%d), %s", res, destAbsolute.getChar());

            return bfalse;
        }

        return btrue;
    }

    static String8 getCurrentDirectory()
    {
        DWORD requiredSize = ::GetCurrentDirectoryW(0, nullptr);
        if (requiredSize == 0)
        {
            return {};
        }

        std::vector<wchar_t> buffer(requiredSize);

        DWORD result = ::GetCurrentDirectoryW(requiredSize, buffer.data());
        if (result == 0 || result >= requiredSize)
        {
            return {};
        }

        return String8{ UTF16ToUTF8(buffer.data()).get() };
    }

    static bool deleteFile(String _fileToDelete)
    {
        BOOL res = 0;
        u32 retrymax = 5;
        u32 retry = 0;

        while (res == 0)
        {
            res = ::DeleteFileW((LPCWSTR)_fileToDelete.cStr());
            if (retry > retrymax)
                break;

            if (res == 0)
            {
                retry++;
                Sleep(100);
            }
        }
        if (retry > retrymax)
        {
            DWORD error = GetLastError();
            LOG_COOKER("[TEXTURE COOKER] Failed to delete file '%ls' after %d retries (error: 0x%X)",
                _fileToDelete.cStr(),
                retry,
                error);
            return false;
        }
        else if (retry > 0)
        {
            LOG_COOKER("[TEXTURE COOKER] Successfully deleted file '%ls' after %d retries", _fileToDelete.cStr(), retry);
        }
        return true;
    }

    static bool deleteFileIfExists(const String & _fileToDelete)
    {  
        if (_fileToDelete.isEmpty())  
            return false;

        // Check that file exist using GetFileAttributesW
        DWORD fileAttributes = ::GetFileAttributesW((LPCWSTR)_fileToDelete.cStr());  
        if (fileAttributes == INVALID_FILE_ATTRIBUTES)
            return true; 

        return deleteFile(_fileToDelete);  
    }


    static bool renameFile(const String & _srcFileName, const String & _newFileName, bool _deleteIfDstExists = true)
    {
        if (_srcFileName.isEmpty() || _newFileName.isEmpty())
            return false;

        if (_deleteIfDstExists)
        {
            if (!deleteFileIfExists(_newFileName))
            {
                LOG_COOKER("[TEXTURE COOKER] Failed to delete existing file '%ls' before renaming", _newFileName.cStr());
                return false;
            }
        }

        BOOL success = ::MoveFileW(
            (LPCWSTR)_srcFileName.cStr(),
            (LPCWSTR)_newFileName.cStr()
        );

        if (!success)
        {
            DWORD error = GetLastError();
            LOG_COOKER("[TEXTURE COOKER] Failed to rename file from '%ls' to '%ls' (error: 0x%X)",
                _srcFileName.cStr(),
                _newFileName.cStr(),
                error);
            return false;
        }

        return true;
    }

    bbool onTextureCookFinishNintendo(const char* tool, const String& temporaryFile, const String& dstFile)
    {
        String temporaryFileWithDDSExtension = temporaryFile + ".dds";

        // The nintendo need the effective extension
        ITF_VERIFY(renameFile(temporaryFile, temporaryFileWithDDSExtension)); 

        String fullFilePathDST;
        FILEMANAGER->TranslatePath(fullFilePathDST, dstFile);

        String fullFilePathDSTWithXTXExtension = fullFilePathDST + ".xtx";
        String commandline;
        commandline.setTextFormat("-i \"%ls\" -o \"%ls\"", temporaryFileWithDDSExtension.cStr(), fullFilePathDSTWithXTXExtension.cStr());

        String8 fullPathTool = getCurrentDirectory() + "\\" + tool;
        ProcessSpawner processSpawner;
        i32 res = processSpawner.create(fullPathTool.cStr(), commandline, "");

        // Revert the renaming to be transparent to the rest of the system
        ITF_VERIFY(renameFile(temporaryFileWithDDSExtension, temporaryFile));

        if (res != 0)
        {
            LOG_COOKER("[TEXTURE_COOKER_ERROR] FAILED TO CONVERT TO NINTENDO FORMAT %s : %d", UTF16ToUTF8(dstFile.cStr()).get(), res);
            return bfalse;
        }

        // Remove the extension to get a file terminating with .ckd
        ITF_VERIFY(renameFile(fullFilePathDSTWithXTXExtension, fullFilePathDST));
        
        LOG_COOKER("[TEXTURE_COOKER_SUCCESS] CONVERT TO NINTENDO FORMAT %s", UTF16ToUTF8(dstFile.cStr()).get());

        return btrue;
    }

    bbool onTextureCookFinishNX(const String& temporaryFile, const String& dstFile)
    {
        return onTextureCookFinishNintendo("NX\\NvnTools\\NvnTexpkg.exe", temporaryFile, dstFile);
    }

    bbool onTextureCookFinishOunce(const String& temporaryFile, const String& dstFile)
    {
        return onTextureCookFinishNintendo("Ounce\\Nvn2Tools\\NvnTexpkg2.exe", temporaryFile, dstFile);
    }

 //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	bbool onTextureCookFinishPS3(const String& temporaryFile,const String& dstFile)
	{
		ProcessSpawner processSpawner;
		String commandline;
		String fullFilePathDST;

		//here dstFile is the temporary dds file
		FILEMANAGER->TranslatePath(fullFilePathDST, dstFile);
		StringConverter destAbsolute(fullFilePathDST);

		commandline.setTextFormat("%ls -o %s",temporaryFile.cStr(),destAbsolute.getChar());
		i32 res = processSpawner.create("dds2gtf.exe",commandline,"");
		if (res)
		{
			LOG_COOKER("[TEXTURE COOKER] FAILED TO CONVERT TO PS3 GTF, %s", destAbsolute.getChar());
			return bfalse;
		}

		return btrue;
	}

    bbool onTextureCookFinishPS5(const String& temporaryFile, const String& dstFile)
    {
        ProcessSpawner processSpawner;
        String commandline;
        String fullFilePathDST;

        FILEMANAGER->TranslatePath(fullFilePathDST, dstFile);

        const char* formatStr = "Auto";
        commandline.setTextFormat("-f %s -s RLL_GLL_BLL_ALL -i %s -o %s", formatStr, UTF16ToUTF8(temporaryFile.cStr()).get(), UTF16ToUTF8(fullFilePathDST.cStr()).get());
        i32 res = processSpawner.create("image2gnf.exe", commandline, "");
        if (res)
        {
            LOG_COOKER("[TEXTURE COOKER] FAILED TO CONVERT TO PS5 GNF, %s ERROR CODE %d", UTF16ToUTF8(fullFilePathDST.cStr()).get(), res);
            return bfalse;
        }

        return btrue;
    }



//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    bbool onTextureCookFinishX360(const String& temporaryFile,const String& dstFile)
    {
        ProcessSpawner processSpawner;
        String commandline;
        String fullFilePathDST;

        //here dstFile is the temporary dds file
        FILEMANAGER->TranslatePath(fullFilePathDST, dstFile);
        StringConverter destAbsolute(fullFilePathDST);

        commandline.setTextFormat("-i %ls -o \"%s\"",temporaryFile.cStr(),destAbsolute.getChar());
        i32 res = processSpawner.create("TexPostCook360.exe",commandline,"");
        if (res!=0)
        {
            LOG_COOKER("[TEXTURE COOKER] FAILED TO CONVERT TO X360(err=%d), %s", res, destAbsolute.getChar());
            return bfalse;
        }
        
        return btrue;
    }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    bbool onTextureCookFinishXBoxSeries(const String& temporaryFile, const String& dstFile)
    {
        String temporaryFileWithDDSExtension = temporaryFile + ".dds";

        // XBTC need the effective extension
        ITF_VERIFY(renameFile(temporaryFile, temporaryFileWithDDSExtension));

        String fullFilePathDST;
        FILEMANAGER->TranslatePath(fullFilePathDST, dstFile);

        String fullFilePathDSTWithXBTCExtension = fullFilePathDST + ".xbtc";
        String commandline;
        commandline.setTextFormat("\"%ls\" --nocompression -o \"%ls\"", temporaryFileWithDDSExtension.cStr(), fullFilePathDSTWithXBTCExtension.cStr());

        String8 fullPathTool = getCurrentDirectory() + "\\XBoxSeries\\XBTC.exe";
        ProcessSpawner processSpawner;
        i32 res = processSpawner.create(fullPathTool.cStr(), commandline, "");

        // Revert the renaming to be transparent to the rest of the system
        ITF_VERIFY(renameFile(temporaryFileWithDDSExtension, temporaryFile));

        if (res != 0)
        {
            LOG_COOKER("[TEXTURE_COOKER_ERROR] FAILED TO CONVERT TO XBoxSeries FORMAT %s : %d", UTF16ToUTF8(dstFile.cStr()).get(), res);
            return bfalse;
        }

        // Remove the extension to get a file terminating with .ckd
        ITF_VERIFY(renameFile(fullFilePathDSTWithXBTCExtension, fullFilePathDST));

        LOG_COOKER("[TEXTURE_COOKER_SUCCESS] CONVERT TO NINTENDO FORMAT %s", UTF16ToUTF8(dstFile.cStr()).get());

        return btrue;
    }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    static bbool imageHasTransparency(nv::Image & _image)
    {
        struct Color32 
        {
            uint8 b, g, r, a;
        };
        const Color32 * pixels = reinterpret_cast<const Color32 *>(_image.pixels());
        // Force texture without alpha to be DXT1
        for(u32 i = 0, n = _image.width()*_image.height(); i<n; ++i)
        {
            if (pixels[i].a != u8(255))
            {
                return true;
            }
        }
        return false;
    }

    static void RGB2HSV(f32 r, f32 g, f32 b, f32& h, f32& s, f32& v)
    {
        f32 M, m, d;
        m=r;m=g<m?g:m;m=b<m?b:m;
        M=r;M=g>M?g:M;M=b>M?b:M;
        v = M;
        if(M==0.f)
            s = 0.f;
        else
            s = (M-m)/M;
        if(s==0.f)
        {
            h = -1.f;
            return;
        }

        d = M - m;
        if(r == M)      h = (g-b)/d;
        else if(g == M) h = 2 + (b-r)/d;
        else            h = 4 + (r-g)/d;
        h = 60*h;
        if(h<0) h += 360;
    }

    static bbool imageHasColors(nv::Image & _image, f32 _threshold = 0.f)
    {
        struct Color32 
        {
            uint8 b, g, r, a;
        };
        const Color32 * pixels = reinterpret_cast<const Color32 *>(_image.pixels());
        Color32 pixelBase = *pixels;

        f32 hueBase = 0.f;
        f32 h, s, v;
        f32 fThresh = _threshold * 360.f;
        
        // search a base point
        for(u32 i = 0, n = _image.width()*_image.height(); i<n; ++i)
        {
            if(pixels[i].a == 255)
            {
                pixelBase = pixels[i];
                RGB2HSV(pixelBase.r/255.f, pixelBase.g/255.f, pixelBase.b/255.f, hueBase, s, v);
                if(hueBase != -1.f)
                    break;
            }
        }

        for(u32 i = 0, n = _image.width()*_image.height(); i<n; ++i)
        {            
            RGB2HSV(pixels[i].r/255.f, pixels[i].g/255.f, pixels[i].b/255.f, h, s, v);
            if(h!=-1.f)
            {
                if(fabs(h - hueBase) > fThresh)
                {
                    return true;
                }
            }
        }

        return false;
    }

    static bool compressFileToDDS_TexConv(const String & _fileOutput, const String& _fullPathSourceFileName, const String& _platform, const TextureConfiguration& _config)
    {
        String fileOutputNormalized = FilePath::normalizePath(_fileOutput);
        String outputFileName = FilePath::getFilename(fileOutputNormalized);
        String outputPath = FilePath::getDirectory(fileOutputNormalized);
        // unfortunately texconv.exe does not support explicit setting the output file name. We play with the extension options to avoid collision.
        String texconvSuffix = outputFileName;
        String texconvFileOutput = outputPath + FilePath::getFilenameWithoutExtension(FilePath::normalizePath(_fullPathSourceFileName)) + texconvSuffix + ".dds";

        String commandLineOptions;
        commandLineOptions += _config.getTexConvFormatOption();
        commandLineOptions += _config.getTexConvWrapModeOption();
        commandLineOptions += _config.isUseMipmap() ? " -m 0" : " -m 1";
        commandLineOptions += " -pow2"; // assume equivalent to setRoundMode(nvtt::RoundMode_ToNextPowerOfTwo) in nvtt
        commandLineOptions += " -srgb"; // assume equivalent to setGamma(2.2f, 2.2f) in nvtt
        commandLineOptions += " -if FANT"; // assume equivalent to setMipmapFilter(nvtt::MipmapFilter_Kaiser) in nvtt
        commandLineOptions += " -y"; // overwrite output file if exists

        String commandLine;
        commandLine.setTextFormat("%ls -sx %ls -o %ls %ls", commandLineOptions.wcharCStr(), texconvSuffix.wcharCStr(), outputPath.wcharCStr(), _fullPathSourceFileName.wcharCStr());
        String8 fullPathTool = getCurrentDirectory() + "\\texconv.exe";
        ProcessSpawner processSpawner;
        i32 res = processSpawner.create(fullPathTool, commandLine, "");

        if (res == 0 && texconvFileOutput != _fileOutput)
        {
            return renameFile(texconvFileOutput, _fileOutput);
        }
        return res == 0;
    }

    static bool compressFileToDDS_NVTT(const String& _fileOutput, const String & _fullPathSourceFileName, const String & _platform, const TextureConfiguration & _config)
    {
        nv::Image image;

        if (!image.load(String8(_fullPathSourceFileName.cStr()).cStr()))
        {
            LOG_COOKER("[TEXTURE COOKER] The file '%ls' is not a supported image type.\n", _fullPathSourceFileName.cStr());
            return false;
        }
        else
        {
            // $GB changed minimal resolution
            if (image.height() > 16 * 1024 || image.width() > 16 * 1024)
            {
                LOG_COOKER("[TEXTURE COOKER] Resolution of '%ls' is too big : w%u h%u.\n", _fullPathSourceFileName.cStr(), image.width(), image.height());
                return bfalse;
            }
        }

        nvtt::Compressor compressor;

        // Uncomment this line to disable CUDA acceleration (if there is noticeable issues)
        // compressor.enableCudaAcceleration(false);

        nvtt::InputOptions inputOptions;
        nvtt::OutputOptions outputOptions;
        nvtt::CompressionOptions compressionOptions;
        nvtt::Format format = nvtt::Format_DXT3;
        nvtt::WrapMode wrapModeUse = nvtt::WrapMode_Repeat;
        nvtt::MipmapFilter mipFilter = nvtt::MipmapFilter_Kaiser;

        String lightFileName = FilePath::getFilename(FilePath::normalizePath(_fullPathSourceFileName));
        bbool rgbOneColor = bfalse;
        if (_platform == "WII")
        {
            switch (_config.getOneColMode())
            {
            case TextureConfiguration::eNO:
                rgbOneColor = bfalse;
                break;
            case TextureConfiguration::eYES:
                rgbOneColor = btrue;
                break;
            default:
                //rgbOneColor = !imageHasColors(image,0.01f);
                break;
            }
        }

        TextureOutputHandler* outputHandler = new TextureOutputHandler(lightFileName.wcharCStr(),
            lightFileName.wcharCStr(),
            StringToUTF8(_fileOutput).get(),
            _platform.wcharCStr(),
            _config.getLodBias(),
            _config.getAutoDegradAlpha() ? 1 : 0,
            rgbOneColor,
            u32(_config.getAnisotropy()));

        if (outputHandler->stream && outputHandler->stream->isError())
        {
            u32 error = GetLastError();
            String tmp;
            tmp.setTextFormat("Error file nvidia %ls error: 0x%X", _fileOutput.wcharCStr(), error);

            if (SYSTEM_ADAPTER)
                SYSTEM_ADAPTER->messageBox("Error Cooking", tmp, ITF_MSG_ICONERROR, ITF_IDYES);
        }

        outputOptions.setOutputHandler(outputHandler);

#ifndef XBOX_USE_TILE
        if (_platform == "X360")
            outputHandler->swapToBigEndian();
#endif
        inputOptions.setRoundMode(nvtt::RoundMode_ToNextPowerOfTwo);
        inputOptions.setTextureLayout(nvtt::TextureType_2D, image.width(), image.height());

        inputOptions.setMipmapData(image.pixels(), image.width(), image.height(), 1, 0, 0);

        inputOptions.setNormalMap(false);
        inputOptions.setConvertToNormalMap(false);
        inputOptions.setGamma(2.2f, 2.2f);
        inputOptions.setNormalizeMipmaps(false);

        format = _config.getnvttFormat();
        wrapModeUse = _config.getnvttWrapMode();
        mipFilter = _config.getnvttMipMapFilter();

        inputOptions.setMipmapGeneration((_config.isUseMipmap() == btrue), -1);

        // Adjust format -> DXT3/5 without alpha is just bits garbage, changed to DXT1
        if ((format == nvtt::Format_DXT3 || format == nvtt::Format_DXT5)
            && !imageHasTransparency(image))
        {
            format = nvtt::Format_DXT1;
        }

        if (_platform == "WII")
        {
            if (format != nvtt::Format_DXT1)
                format = nvtt::Format_RGBA;
        }

        if (_platform == "IPAD")
        {
            if (image.width() || image.height() > 1024)
            {
                inputOptions.setMaxExtents(image.width() >> 2);
            }
            else if (image.width() || image.height() >= 512)
            {
                inputOptions.setMaxExtents(image.width() >> 1);
            }
        }
        else if (_platform == "WII" || _platform == "EMUWII")
        {
            // Texture dimension
            int mindim = std::min(image.width(), image.height());
            int maxdim = std::max(image.width(), image.height());

            // Compute degradation
            int degrad = _config.getAutoDegrad() ? 1 : 0;
            degrad += _config.getMipmapStart();

            // Maximum texture size is 1024x1024
            while ((maxdim >> degrad) > 1024)
                ++degrad;

            // Minimum texture size is 8x8
            while ((degrad > 0) && ((mindim >> degrad) < 8))
                --degrad;

            // setMaxExtents if necessary
            if (degrad > 0)
                inputOptions.setMaxExtents(maxdim >> degrad);
        }
        else
        {
            if (_config.getMipmapStart() != 0)
            {
                int maxdim = std::max(image.width(), image.height());
                inputOptions.setMaxExtents(maxdim >> _config.getMipmapStart());
            }
        }

        compressionOptions.setFormat(format);
        inputOptions.setWrapMode(wrapModeUse);

        inputOptions.setMipmapFilter(mipFilter);
        compressionOptions.setQuality(nvtt::Quality_Highest);

        //all options must be setA
        outputHandler->setTotal(compressor.estimateSize(inputOptions, compressionOptions));


        if (!compressor.process(inputOptions, compressionOptions, outputOptions))
        {
            SF_DEL(outputHandler);
            return bfalse;
        }

        SF_DEL(outputHandler);
        return true;
    }

    static bool compressFileToDDS(const String& _fileOutput, const String& _fullPathSourceFileName, const String& _platform, const TextureConfiguration& _config)
    {
        if (FILESERVER->hasCookersOption("texconv"))
        {
            SimpleProfileTimer timer;
            bool res = compressFileToDDS_TexConv(_fileOutput, _fullPathSourceFileName, _platform, _config);
            LOG_COOKER("[TEXTURE COOKER] %ls cooked using DX TEXCONV in %.2f seconds", _fullPathSourceFileName.wcharCStr(), timer.getDurationSinceStart()/1000.);
            return res;
        }
        else
        {
            SimpleProfileTimer timer;
            bool res = compressFileToDDS_NVTT(_fileOutput, _fullPathSourceFileName, _platform, _config);
            LOG_COOKER("[TEXTURE COOKER] %ls cooked using NVTT in %.2f seconds", _fullPathSourceFileName.wcharCStr(), timer.getDurationSinceStart() / 1000.);
            return res;
        }
    }

    static String getTemporaryFile(const String & _filename, const String & platform)
    {
        // We use a unique ID to avoid potential collision of file names if running in parallel
        StringID uniqueId{ _filename };
        String temporaryFile;
        temporaryFile.setTextFormat("%ls\\tmpDirectory_%ls\\tempTexture_%08X.tmp", FILESERVER->getApplicationDirectory().cStr(), platform.cStr(), uniqueId.GetValue());
        return temporaryFile;
    }

  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    bbool textureCooker::compress(const String& _szPlatform, const String& _szFilename, bbool _bUseTemporaryConfig)
    {
        String baseName = FilePath::getFilename(FILESERVER->getCookedNamePlatform(_szFilename,_szPlatform));
        String szDirectory = FilePath::getDirectory(_szFilename);

        String szFinalDirectory;
        FILESERVER->getDirectoryFromCookedFolderPlatform(szFinalDirectory,szDirectory,_szPlatform);

        static Mutex mutexDirectoryCreation;

        //make the directory...
        String fullDstPath;
        if (FILEMANAGER->TranslatePath(fullDstPath, szFinalDirectory))
        {
            if (!Directory::exists(fullDstPath))
            {
                ScopeLockMutex lock(mutexDirectoryCreation); // Ensure that only one thread creates the directories at a time
                Directory::create(fullDstPath);
            }
        }

        String temporaryFile;
        String dstFile(szFinalDirectory + baseName);
        String fullDstFile(fullDstPath + baseName);
        String NVFileOutput;

        bbool useTemporaryFile = bfalse;

        if ((_szPlatform == "PS3") || (_szPlatform == "PS5") || (_szPlatform == "WII") || (_szPlatform == "NX") || (_szPlatform == "OUNCE")
        #ifdef XBOX_USE_TILE
        || (_szPlatform == "X360") || (_szPlatform == "XBOXSERIES")
        #endif
        )
        {
            String tmpDir;
            tmpDir.setTextFormat("tmpDirectory_%S", _szPlatform.cStr());

            if (!Directory::exists(tmpDir))
            {
                ScopeLockMutex lock(mutexDirectoryCreation); // Ensure that only one thread creates the directories at a time
                Directory::create(tmpDir);
            }

            useTemporaryFile = btrue;
            temporaryFile = getTemporaryFile(_szFilename, _szPlatform);
            NVFileOutput = temporaryFile;
        }
        else
        {
            temporaryFile = dstFile;
            NVFileOutput = fullDstFile;
        }

        String configurationfilename(_szFilename + ".tfi");

        TextureConfiguration* pTextureConfiguration = NULL;

        u64 textureConfigurationTimeWrite = 0;

        if ((FILEMANAGER->fileExists(configurationfilename) || _bUseTemporaryConfig) && !(_szPlatform == "IPAD"))//TODO remove last condition - !(_szPlatform == "IPAD") -, it forces config for iPad
        {
            pTextureConfiguration = TextureConfiguration::openTextureConfiguration(_szPlatform, _bUseTemporaryConfig ? "ConfigTemporary.tfi" : configurationfilename);

            textureConfigurationTimeWrite = FILEMANAGER->getLastTimeWriteAccess(configurationfilename);
        }

        if (!pTextureConfiguration)
            pTextureConfiguration = defaultConfiguration(_szPlatform);

        // $GB added support of alternative source file names for resources
        String realSourceFileName = ResourceManager::getExistingAlternativeFileNameForCooking(_szFilename, _szPlatform);
        if (realSourceFileName != _szFilename)
        {
            LOG_COOKER("[TEXTURE COOKER] %S hijacked by higher res %S", _szFilename.cStr(), realSourceFileName.cStr());
        }

        u64 srcTimeWrite = FILEMANAGER->getLastTimeWriteAccess(realSourceFileName);
        u64 dstTimeWrite = FILEMANAGER->getLastTimeWriteAccess(dstFile);

        String cookFilename = FILESERVER->getCookedNamePlatform(_szFilename, _szPlatform);
        bbool skippedSupported = !cookerFactory::skipVersioningPlatform(_szPlatform) ? btrue : !cookerFactory::fileNeedsCook(_szFilename, cookFilename, _szPlatform, Versioning::textureVer());
        if (cookerFactory::getForceMode())
            skippedSupported = bfalse;

        if (skippedSupported)
        {
            StringConverter lightfilename(FilePath::getFilename(_szFilename));
            if (_bUseTemporaryConfig)
            {
                LOG_COOKER("[TEXTURE COOKER] COOKING %s REASON: USE TEMPORARY CONFIG", lightfilename.getChar());
            }
            else if (dstTimeWrite != srcTimeWrite)
            {
                LOG_COOKER("[TEXTURE COOKER] COOKING %s REASON: Cooked file date is different than source", lightfilename.getChar());
            }
            else if (textureConfigurationTimeWrite != 0 && textureConfigurationTimeWrite != srcTimeWrite)
            {
                LOG_COOKER("[TEXTURE COOKER] COOKING %s REASON: Configuration (TFI) file date is different than source", lightfilename.getChar());
            }
            else
            {
                LOG_COOKER("[TEXTURE COOKER] %s ==> %ls//%ls SKIPPED", lightfilename.getChar(), _szPlatform.wcharCStr(), baseName.wcharCStr());
                return btrue;
            }
        }
        else
        {
            StringConverter lightfilename(FilePath::getFilename(_szFilename));
            LOG_COOKER("[TEXTURE COOKER] COOKING %s REASON: FORCE COOK", lightfilename.getChar());
        }

        String fullPathSourceFileName = realSourceFileName;
        FILEMANAGER->TranslatePath(fullPathSourceFileName, realSourceFileName);

        bool success = compressFileToDDS(NVFileOutput, fullPathSourceFileName, _szPlatform, *pTextureConfiguration);
        if (!success)
        {
            SF_DEL(pTextureConfiguration);
            return bfalse;
        }

        // flush also the configuration file.
        // flushTimeWriteAccess now change the time even on read only files (on win32)
        if (textureConfigurationTimeWrite)
            FILEMANAGER->flushTimeWriteAccess(configurationfilename, srcTimeWrite);

        if (_szPlatform == "PS3")
        {
            onTextureCookFinishPS3(temporaryFile,dstFile);
        }
        else if (_szPlatform == "PS5")
        {
            onTextureCookFinishPS5(temporaryFile, dstFile);
        }
    #ifdef XBOX_USE_TILE
        else if (_szPlatform == "X360")
        {
            onTextureCookFinishX360(temporaryFile, dstFile);
        }
        else if (_szPlatform == "XBOXSERIES")
        {
            onTextureCookFinishXBoxSeries(temporaryFile, dstFile);
        }
    #endif
        else
        if (_szPlatform == "WII")
        {
            onTextureCookFinishWII(temporaryFile, dstFile);
        }
        else if (_szPlatform == "NX")
        {
            onTextureCookFinishNX(temporaryFile, dstFile);
        }
        else if (_szPlatform == "OUNCE")
        {
            onTextureCookFinishOunce(temporaryFile, dstFile);
        }

        //delete the temporary file
        if (useTemporaryFile)
        {
            ITF_VERIFY(deleteFileIfExists(temporaryFile));
        }

        SF_DEL(pTextureConfiguration);

        if (!FILEMANAGER->fileExists(dstFile))
        {
            LOG_COOKER("[TEXTURE COOKER] \r %ls doesn't exist",dstFile.cStr());
            return bfalse;
        }

        FILEMANAGER->flushTimeWriteAccess(dstFile, srcTimeWrite);

        return btrue;
    }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    bbool textureCooker::cudaIsEnable()
    {
        nvtt::Compressor compressor;
        return compressor.isCudaAccelerationEnabled();
    }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
          return(DefWindowProc(hWnd, uMsg, wParam, lParam));
    }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    bbool textureCooker::dependencyFiles( const String& cacheKey, const String& _vPlatform,const String& _szFilename,DepCollection& _vDependencies )
    {
        if ( _vDependencies.load( cacheKey, _szFilename, _vPlatform ) )
        {
            _vDependencies.filterExt();
            return true;
        }

        ITF_ASSERT( FILEMANAGER != NULL );
        if ( FILEMANAGER == NULL )
            return bfalse;

        Path path( _szFilename );
        
        const String& directory = path.getDirectory();
        
        String  atlPath( directory );
        String  name;
        path.getBasenameWithoutExtension( name );

        atlPath += name;
        atlPath += "_atl.atl";
        if ( FILEMANAGER->fileExists( atlPath ) )
            _vDependencies.add( atlPath );

        String pbkPath = directory;
        pbkPath+= name;
        pbkPath+= ".pbk";

        if ( FILEMANAGER->fileExists( pbkPath ) )
            _vDependencies.add( pbkPath );
        _vDependencies.save( cacheKey, _szFilename, _vPlatform );
        _vDependencies.filterExt();
        return true;
    }
}
