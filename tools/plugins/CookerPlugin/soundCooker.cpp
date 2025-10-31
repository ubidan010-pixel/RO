#include "core/enginecommon.h"

#include "soundCooker.h"

#ifndef _ITF_PUGIXMLWRAP_H_
#include "core/XML/PugiXMLWrap.h"
#endif //_ITF_PUGIXMLWRAP_H_

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

#ifndef _ITF_PROCESSSPAWNER_H_
#include "core/system/ProcessSpawner.h"
#endif //_ITF_PROCESSSPAWNER_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_RAKIINCLUDES_
#include "engine/sound/raki/RakiIncludes.h"
#endif // _ITF_RAKIINCLUDES_

#include "RakiEngine/Specific/_Win/Specific_WIN.h"

#ifndef _ITF_READFILESTREAM_H_
#include "engine/sound/raki/ReadFileStream.h"
#endif //_ITF_READFILESTREAM_H_

#ifndef _ITF_WRITEFILESTREAM_H_
#include "engine/sound/raki/WriteFileStream.h"
#endif //_ITF_WRITEFILESTREAM_H_

#ifndef RAKI_RIFFCHUNKMANAGER_H
#include "RakiEngine/Data/WaveFile/RiffChunkManager.h"
#endif // RAKI_RIFFCHUNKMANAGER_H

#ifndef RAKI_WAVEFILE_H
#include "RakiEngine/Data/WaveFile/WaveFile.h"
#endif // RAKI_WAVEFILE_H

#ifndef RAKI_WAVEDATAPITCHCONVERTER_H
#include "RakiEngine/Data/WaveFile/WaveDataPitchConverter.h"
#endif // RAKI_WAVEDATAPITCHCONVERTER_H

#ifndef RAKI_MEMORYBLOCK_H
#include "RakiEngine/Data/MemoryBlock/MemoryBlock.h"
#endif // RAKI_MEMORYBLOCK_H

#ifndef ITF_BINFILEREADER_H
#include "engine/sound/raki/BinFileReader.h"
#endif ITF_BINFILEREADER_H





#ifndef _ITF_DEP_COLLECTOR_H_
#include "engine/dependencies/DepCollector.h"
#endif //_ITF_DEP_COLLECTOR_H_

namespace ITF
{
    bbool soundCooker::useBatch = btrue;


    // wii 


    typedef struct
    {
        // for header generation during decode
        u32 num_samples; // total number of RAW samples
        u32 num_adpcm_nibbles; // number of ADPCM nibbles (including frame headers)
        u32 sample_rate; // Sample rate, in Hz
        // DSP addressing and decode context
        u16 loop_flag; // 1=LOOPED, 0=NOT LOOPED
        u16 format; // Always 0x0000, for ADPCM
        u32 sa; // Start offset address for looped samples (zero for non-looped)
        u32 ea; // End offset address for looped samples
        u32 ca; // always zero
        u16 coef[16]; // decode coefficients (eight pairs of 16-bit words)
        // DSP decoder initial state
        u16 gain; // always zero for ADPCM
        u16 ps; // predictor/scale
        u16 yn1; // sample history
        u16 yn2; // sample history
        // DSP decoder loop context
        u16 lps; // predictor/scale for loop context
        u16 lyn1; // sample history (n-1) for loop context
        u16 lyn2; // sample history (n-2) for loop context
        u16 pad[11]; // reserved
    } DSPADPCM;


#ifdef ITF_SUPPORT_RAKI

    bbool WiiCompressOneFileToDspadcpm( const String& _exeDir, const char * _inputPath, const char * _outputPath, u32 _sampleSize )
    {
        ProcessSpawner processSpawner;
        String commandline;

        char txtDumpPath[512];
        sprintf( txtDumpPath, "%s.txt", _inputPath );

        commandline.setTextFormat("-encode %s %s -l0-%d -c%s", _inputPath, _outputPath, _sampleSize - 1, txtDumpPath );
        String exePath = _exeDir + "\\wii\\dspadpcm.exe";

        i32 res = processSpawner.create( exePath, commandline, "" );

        DeleteFileA( txtDumpPath );

        if ( res )
            return bfalse;

        return btrue;
    }



    bbool WiiWriteWaves( const String& _exeDir, const raki::WaveDataPitchConverter * _waveData, const WAVEFORMATEX  * _originalFormat, 
        const char * _destinationPath, const bbool _interleaveStereo )
    {
        raki::RiffChunkManager::singleton().setIsWritingToBigEndians( false );

        switch( _originalFormat->nChannels )
        {
        case 1:
            {
                char inputPath[512];
                sprintf( inputPath, "%s_%d.wav", _destinationPath, _waveData->getSamplingRate() );
                char outputPath[512];
                sprintf( outputPath, "%s_%d.wii", _destinationPath, _waveData->getSamplingRate() );

                raki::WaveFile output;
                WAVEFORMATEX waveformatPcm;
                waveformatPcm = *_originalFormat;
                waveformatPcm.nSamplesPerSec = _waveData->getSamplingRate();
                waveformatPcm.nAvgBytesPerSec = 2 * waveformatPcm.nSamplesPerSec;
                output.createEmptyRiffChunk();
                output.addChunk( "fmt ", sizeof( WAVEFORMATEX ), &waveformatPcm );
                output.addChunk( "data", waveformatPcm.nBlockAlign * _waveData->getSampleSize(), _waveData->getData() );
                output.recalculateRiffAndListSizes();

                WriteFileStream wave;
                if ( wave.open( inputPath ) )
                {
                    raki::Serializer writeSerializer( &wave );
                    output.write( writeSerializer );
                }
                else 
                    return bfalse;

                wave.close();
                if ( !WiiCompressOneFileToDspadcpm( _exeDir, inputPath, outputPath, _waveData->getSampleSize() ) )
                    return bfalse;

                // now make bin

                BinFileReader binFileReader;
                binFileReader.readFrom( outputPath );
                if ( !binFileReader.getData() || !binFileReader.getSize() )
                    return bfalse;

                WAVEFORMATEX waveformatAdpcm = waveformatPcm;
                waveformatAdpcm.wFormatTag = WAVE_FORMAT_ADPCM;
				waveformatAdpcm.nSamplesPerSec = _waveData->getSamplingRate();

                raki::WaveFile outputBin;
                outputBin.createEmptyRiffChunk();
                outputBin.addChunk( "fmt ", sizeof( waveformatAdpcm ), &waveformatAdpcm );
                outputBin.addChunk( "dspL", sizeof( DSPADPCM ), binFileReader.getData() );
                outputBin.addChunk( "datL", (u32)( binFileReader.getSize() - sizeof( DSPADPCM ) ), (void*)( sizeof( DSPADPCM ) + (u64)binFileReader.getData() ) );
                outputBin.recalculateRiffAndListSizes();
                outputBin.changeToBigEndian();

                raki::RiffChunkManager::singleton().setIsWritingToBigEndians( true );

                WriteFileStream bin;
                if ( bin.open( _destinationPath ) )
                {
                    raki::Serializer writeSerializer( &bin );
                    outputBin.write( writeSerializer );
                }
                else 
                    return bfalse;

                bin.close();

                DeleteFileA( inputPath );
                DeleteFileA( outputPath );

            }
            break;

        case 2:
            {
                WAVEFORMATEX waveformatPcm;
                waveformatPcm = *_originalFormat;
                waveformatPcm.nChannels = 1;
                waveformatPcm.nBlockAlign = 2;
                waveformatPcm.nSamplesPerSec = _waveData->getSamplingRate();
                waveformatPcm.nAvgBytesPerSec = 2 * waveformatPcm.nSamplesPerSec;

                // left data 

                char inputPathL[512];
                sprintf( inputPathL, "%s_L_%d.wav", _destinationPath, _waveData->getSamplingRate() );
                char outputPathL[512];
                sprintf( outputPathL, "%s_L_%d.wii", _destinationPath, _waveData->getSamplingRate() );

                raki::WaveFile outputL;
                outputL.createEmptyRiffChunk();
                outputL.addChunk( "fmt ", sizeof( WAVEFORMATEX ), &waveformatPcm );
                outputL.addChunk( "data", waveformatPcm.nBlockAlign * _waveData->getSampleSize(), _waveData->getData() );
                outputL.recalculateRiffAndListSizes();

                WriteFileStream waveL;
                if ( waveL.open( inputPathL ) )
                {
                    raki::Serializer writeSerializer( &waveL );
                    outputL.write( writeSerializer );
                }
                else 
                    return bfalse;

                waveL.close();
                if ( !WiiCompressOneFileToDspadcpm( _exeDir, inputPathL, outputPathL, _waveData->getSampleSize() ) )
                    return bfalse;

                // right data 

                char inputPathR[512];
                sprintf( inputPathR, "%s_R_%d.wav", _destinationPath, _waveData->getSamplingRate() );
                char outputPathR[512];
                sprintf( outputPathR, "%s_R_%d.wii", _destinationPath, _waveData->getSamplingRate() );

                raki::WaveFile outputR;
                outputR.createEmptyRiffChunk();
                outputR.addChunk( "fmt ", sizeof( WAVEFORMATEX ), &waveformatPcm );
                outputR.addChunk( "data", waveformatPcm.nBlockAlign * _waveData->getSampleSize(), _waveData->getRightData() );
                outputR.recalculateRiffAndListSizes();

                WriteFileStream waveR;
                if ( waveR.open( inputPathR ) )
                {
                    raki::Serializer writeSerializer( &waveR );
                    outputR.write( writeSerializer );
                }
                else 
                    return bfalse;
                
                waveR.close();
                if ( !WiiCompressOneFileToDspadcpm( _exeDir, inputPathR, outputPathR, _waveData->getSampleSize() ) )
                    return bfalse;


                // now make bin

                BinFileReader binFileReaderL;
                binFileReaderL.readFrom( outputPathL );

                if ( !binFileReaderL.getData() || !binFileReaderL.getSize() )
                    return bfalse;

                BinFileReader binFileReaderR;
                binFileReaderR.readFrom( outputPathR );

                if ( !binFileReaderR.getData() || !binFileReaderR.getSize() )
                    return bfalse;

                WAVEFORMATEX waveformatAdpcm = waveformatPcm;
                waveformatAdpcm.wFormatTag = WAVE_FORMAT_ADPCM;
                waveformatAdpcm.nChannels = 2;
				waveformatAdpcm.nSamplesPerSec = _waveData->getSamplingRate();

                raki::WaveFile outputBin;
                outputBin.createEmptyRiffChunk();
                outputBin.addChunk( "fmt ", sizeof( waveformatAdpcm ), &waveformatAdpcm );
                outputBin.addChunk( "dspL", sizeof( DSPADPCM ), binFileReaderL.getData() );
                outputBin.addChunk( "dspR", sizeof( DSPADPCM ), binFileReaderR.getData() );

                if ( _interleaveStereo )
                {
                    // interleave on aligned size...

                    u32 channelSize = (u32)binFileReaderL.getSize() - sizeof( DSPADPCM );
                    RAKI_ASSERT( channelSize == ( binFileReaderR.getSize() - sizeof( DSPADPCM ) ) );

                    if ( channelSize % 8 )
                        channelSize = ( channelSize + 8 ) & 0xfffffff8;

                    raki::MemoryBlock blockL( channelSize, raki::MemoryBlock::standard, raki::Memory::data );
                    raki::Memory::memcpy( blockL.getBuffer(), (void*)( sizeof( DSPADPCM ) + (u64)binFileReaderL.getData() ), (u32)binFileReaderL.getSize() - sizeof( DSPADPCM ) );

                    raki::MemoryBlock blockR( channelSize, raki::MemoryBlock::standard, raki::Memory::data );
                    raki::Memory::memcpy( blockR.getBuffer(), (void*)( sizeof( DSPADPCM ) + (u64)binFileReaderR.getData() ), (u32)binFileReaderR.getSize() - sizeof( DSPADPCM ) );

                    raki::MemoryBlock block( 2 * channelSize, raki::MemoryBlock::standard, raki::Memory::data );

                    u32 * srcL = (u32*) blockL.getBuffer();
                    u32 * srcR = (u32*) blockR.getBuffer();
                    u32 * dst = (u32*) block.getBuffer();

                    for ( u32 u = 0 ; u < ( channelSize / 8 ) ; ++u )
                    {
                        *dst++ = *srcL++;
                        *dst++ = *srcL++;
                        *dst++ = *srcR++;
                        *dst++ = *srcR++;
                    }

                    outputBin.addChunk( "datS", (u32)block.getSize(), block.getBuffer() );
                }
                else
                {
                    outputBin.addChunk( "datL", (u32)( binFileReaderL.getSize() - sizeof( DSPADPCM ) ), (void*)( sizeof( DSPADPCM ) + (u64)binFileReaderL.getData() ) );
                    outputBin.addChunk( "datR", (u32)( binFileReaderR.getSize() - sizeof( DSPADPCM ) ), (void*)( sizeof( DSPADPCM ) + (u64)binFileReaderR.getData() ) );
                }

                outputBin.recalculateRiffAndListSizes();
                outputBin.changeToBigEndian();

                raki::RiffChunkManager::singleton().setIsWritingToBigEndians( true );

                WriteFileStream bin;
                if ( bin.open( _destinationPath ) )
                {
                    raki::Serializer writeSerializer( &bin );
                    outputBin.write( writeSerializer );
                }
                else 
                    return bfalse;

                bin.close();

                DeleteFileA( inputPathL );
                DeleteFileA( outputPathL );
                DeleteFileA( inputPathR );
                DeleteFileA( outputPathR );
            }
            break;
        }

        raki::RiffChunkManager::singleton().setIsWritingToBigEndians( false );

        return btrue;
    }




    bbool WiiCompressBufferToDspadcpm( const String& _exeDir, const char * _src, const char * _dst, u32 _destSamplingRate )
    {
        raki::RiffChunkManager::singleton().setIsWritingToBigEndians( false );
        raki::RiffChunkManager::singleton().setShouldReadDataBlockForWaveData( true );

        u32 destSamplingRate = _destSamplingRate;

        if ( ( destSamplingRate != 32000 ) && ( destSamplingRate != 24000 ) && ( destSamplingRate != 16000 ) )
            destSamplingRate = 32000;

        ReadFileStream inputStream;
        if ( !inputStream.open( _src ) )
            return bfalse;

        const char fmtType[] = "fmt ";
        raki::WaveFile inputWaveFile;
        raki::Serializer readSerializer( &inputStream );

        inputWaveFile.serialize( readSerializer );

        if ( !inputWaveFile.getChunkData( fmtType ) )
            return bfalse;

        WAVEFORMATEX * format = (WAVEFORMATEX*)inputWaveFile.getChunkData( fmtType );

        if ( format->nSamplesPerSec != 48000 )
            return bfalse;

        void * data = inputWaveFile.getChunkData( "data" );

        u32 sampleSize = inputWaveFile.getChunkSize( "data" ) / (u32) format->nBlockAlign;

        raki::WaveDataPitchConverter converted( data, sampleSize, 48000, destSamplingRate, format->nChannels );
        
        return WiiWriteWaves( _exeDir, &converted, format, _dst, bfalse );
    }




    bbool WiiCompressStreamToDspadcpm( const String& _exeDir, const char * _src, const char * _dst, u32 _destSamplingRate )
    {
        raki::RiffChunkManager::singleton().setIsWritingToBigEndians( false );
        raki::RiffChunkManager::singleton().setShouldReadDataBlockForWaveData( true );

        u32 destSamplingRate = _destSamplingRate;

        if ( ( destSamplingRate != 32000 ) && ( destSamplingRate != 24000 ) && ( destSamplingRate != 16000 ) )
            destSamplingRate = 32000;

        ReadFileStream inputStream;
        if ( !inputStream.open( _src ) )
            return bfalse;

        const char fmtType[] = "fmt ";
        raki::WaveFile inputWaveFile;
        raki::Serializer readSerializer( &inputStream );

        inputWaveFile.serialize( readSerializer );

        if ( !inputWaveFile.getChunkData( fmtType ) )
            return bfalse;

        WAVEFORMATEX * format = (WAVEFORMATEX*)inputWaveFile.getChunkData( fmtType );

        if ( format->nSamplesPerSec != 48000 )
            return bfalse;

        void * data = inputWaveFile.getChunkData( "data" );

        u32 sampleSize = inputWaveFile.getChunkSize( "data" ) / (u32) format->nBlockAlign;

        raki::WaveDataPitchConverter converted( data, sampleSize, 48000, destSamplingRate, format->nChannels );

        return WiiWriteWaves( _exeDir, &converted, format, _dst, btrue );
    }



    bool convertTo32AndByteSwap( const char * _inputPath, const char * _outputPath )
    {
        raki::RiffChunkManager::singleton().setIsWritingToBigEndians( false );
        raki::RiffChunkManager::singleton().setShouldReadDataBlockForWaveData( true );

        ReadFileStream inputStream;

        if ( !inputStream.open( _inputPath ) )
            return false;

        const char fmtType[] = "fmt ";
        raki::WaveFile inputWaveFile;
        raki::Serializer readSerializer( &inputStream );

        inputWaveFile.serialize( readSerializer );

        if ( !inputWaveFile.getChunkData( fmtType ) )
            return false;

        WAVEFORMATEX * format = (WAVEFORMATEX*)inputWaveFile.getChunkData( fmtType );

        if ( format->nSamplesPerSec != 48000 )
            return false;

        void * data = inputWaveFile.getChunkData( "data" );

        u32 sampleSize = inputWaveFile.getChunkSize( "data" ) / (u32) format->nBlockAlign;

        WAVEFORMATEX * _originalFormat = ( WAVEFORMATEX * ) inputWaveFile.getChunkData( fmtType );

        raki::WaveDataPitchConverter convert( data, sampleSize, 48000, 32000, _originalFormat->nChannels, false );

        WAVEFORMATEX waveformat = *_originalFormat;
        waveformat.nSamplesPerSec = 32000;
        waveformat.nAvgBytesPerSec = waveformat.nBlockAlign * waveformat.nSamplesPerSec;

        raki::RiffChunkManager::singleton().setIsWritingToBigEndians( true );

        raki::WaveFile outputBin;
        outputBin.createEmptyRiffChunk();
        outputBin.addChunk( "fmt ", sizeof( WAVEFORMATEX ), &waveformat );
        outputBin.addChunk( "data", convert.getSampleSize() * 4 , convert.getData() );
        outputBin.recalculateRiffAndListSizes();
        outputBin.changeToBigEndian();

        WriteFileStream bin;
        if ( bin.open( _outputPath ) )
        {
            raki::Serializer writeSerializer( &bin );
            outputBin.write( writeSerializer );
        }
        else 
            return false;

        bin.close();

        return true;

    }

#endif

    class SoundConfiguration 
    {
        public:

            SoundConfiguration() 
            : m_adpcmBlockSize(0)
            , m_xma2Compression(0)
            , m_isMusic(bfalse)
            {};


            const String& getFormat() const                             {return m_format;} 
            void setFormat(const String& _format)                       {m_format = _format;}

            ITF_INLINE u32 getAdpcmBlockSize() const { return m_adpcmBlockSize; }
            ITF_INLINE void setAdpcmBlockSize(u32 val) { m_adpcmBlockSize = val; }

            ITF_INLINE u32 getXma2Compression() const { return m_xma2Compression; }
            ITF_INLINE void setXma2Compression(u32 val) { m_xma2Compression = val; }

            ITF_INLINE bbool getIsMusic() const { return m_isMusic; }
            ITF_INLINE void setIsMusic(bbool val) { m_isMusic = val; }


        protected:
                 

            String  m_format;
            u32     m_adpcmBlockSize;
            u32     m_xma2Compression;
            bbool   m_isMusic;
    };
    

    SoundConfiguration* openSoundConfiguration( const String& _szFilename, const String& _szPlatform )
    {
        PugiXMLWrap XML;
        if (!XML.openFile(_szFilename,bfalse, PugiXMLWrap::RaiseError))
            return bfalse;

        String targetName;
        String desiredTargetName = _szPlatform;
        desiredTargetName.toLower();

        String compressionMode;
 
        {
            if (XML.enterElement("SoundConfiguration"))
                do
                {
                    targetName = XML.readAttribute("TargetName","");
                    targetName.toLower();
                    if (targetName == desiredTargetName)
                    {
                        SoundConfiguration* pSoundConfiguration = new SoundConfiguration();
                        pSoundConfiguration->setFormat(XML.readAttribute("Format",""));
                        pSoundConfiguration->setAdpcmBlockSize(XML.readIntAttribute("AdpcmBlockSize",128));
                        pSoundConfiguration->setXma2Compression(XML.readIntAttribute("Xma2Compression",90));
                        int isMusic = XML.readIntAttribute("IsMusic", 0);
                        pSoundConfiguration->setIsMusic( isMusic ? btrue : bfalse );
                        return pSoundConfiguration;
                    }

                } while (XML.nextSameElement());
                XML.exitElement();
        }

        return NULL;
    }


    bbool compressMusic( const SoundConfiguration * _configurationFile, const String& _szPlatform, const StringConverter& _lightfilename, const StringConverter& _shortName,
        const StringConverter& _sourceAbsolute, const StringConverter& _destAbsolute, const String& _exeDir )
    {
//      Need to check if sample size is multiple of 1152

        if (_szPlatform == "PC" || _szPlatform == "FINALPC" || _szPlatform == "EMUWII")
        { 
            if ( _configurationFile->getFormat() == "PCM" )
            {
                DeleteFileA( _destAbsolute.getChar() );

                if ( !CopyFileA( _sourceAbsolute.getChar(), _destAbsolute.getChar(), TRUE) )
                {
                    LOG("\r%s ==> %s FAILED TO COPY FILE", _lightfilename.getChar(), _shortName.getChar());
                    SF_DEL( _configurationFile );
                    return bfalse;
                }

                SetFileAttributesA( _destAbsolute.getChar(), 0 );
            }
            else if ( _configurationFile->getFormat() == "ADPCM" )
            {
                if (_configurationFile->getAdpcmBlockSize() != 512 &&
                    _configurationFile->getAdpcmBlockSize() != 256 &&
                    _configurationFile->getAdpcmBlockSize() != 128 &&
                    _configurationFile->getAdpcmBlockSize() != 64 &&
                    _configurationFile->getAdpcmBlockSize() != 32 )
                {
                    LOG_COOKER("[SOUND COOKER] %s ==> %s FAILED TO CONVERT TO ADPCM invalid block size %d", _lightfilename.getChar(), _shortName.getChar(), _configurationFile->getAdpcmBlockSize());
                    return bfalse;
                }
                ProcessSpawner processSpawner;
                String commandline;

                String exePath;
                if ( soundCooker::useBatch )
                {
                    commandline.setTextFormat("%ls %d %s %s", _exeDir.cStr(), _configurationFile->getAdpcmBlockSize(), _sourceAbsolute.getChar(), _destAbsolute.getChar());
                    exePath = _exeDir + "\\adpcmencode.bat";
                }
                else
                {
                    commandline.setTextFormat("-b %d %s %s", _configurationFile->getAdpcmBlockSize(), _sourceAbsolute.getChar(), _destAbsolute.getChar());
                    exePath = _exeDir + "\\adpcmencode.exe";
                }

                i32 res = processSpawner.create(exePath ,commandline,"");
                if (res)
                {
                    LOG_COOKER("[COOKER] %s ==> %s FAILED TO CONVERT TO ADPCM, %s", _lightfilename.getChar(), _shortName.getChar(), StringConverter( processSpawner.getLog()).getChar() );
                    return bfalse;
                }
                else    
                {
                    LOG_COOKER("[SOUND COOKER] %s ==> %s COOKED", _lightfilename.getChar(), _shortName.getChar());
                }
            }
        }
        else if ( (_szPlatform == "X360") || (_szPlatform == "PS3") )
        {
            if ( _configurationFile->getFormat() == "PCM" )
            {
                DeleteFileA( _destAbsolute.getChar() );

                if ( !CopyFileA( _sourceAbsolute.getChar(), _destAbsolute.getChar(), TRUE) )
                {
                    LOG_COOKER("[SOUND COOKER] \r%s ==> %s FAILED TO COPY FILE", _lightfilename.getChar(), _shortName.getChar());
                    SF_DEL( _configurationFile );
                    return bfalse;
                }

                SetFileAttributesA( _destAbsolute.getChar(), 0 );
            }
            else return false;
        }
        else return false;

        return btrue;
    }

    bbool soundCooker::cookFile(const String& _vPlatform,const String& _szFilename)
    {
        return soundCooker::compress(_vPlatform,_szFilename,bfalse);
    }

    bbool soundCooker::compress(const String& _szPlatform,const String& _sourceRelative, bbool _bUseTemporaryConfig)
    {
        String baseName = FilePath::getFilename(FILESERVER->getCookedNamePlatform(_sourceRelative,_szPlatform));
        StringConverter shortName(baseName);

        String szDirectory = FilePath::getDirectory(_sourceRelative);
        String szFinalDirectory;
        FILESERVER->getDirectoryFromCookedFolderPlatform(szFinalDirectory,szDirectory,_szPlatform);
        String destRelative = szFinalDirectory+baseName;

        String fullpath;
        if(FILEMANAGER->TranslatePath(fullpath, szFinalDirectory))
            Directory::create(fullpath);
        String fullFilePathSRC , fullFilePathDST;

        FILEMANAGER->TranslatePath(fullFilePathSRC, _sourceRelative);
        FILEMANAGER->TranslatePath(fullFilePathDST, destRelative);

        StringConverter destAbsolute(fullFilePathDST);
        StringConverter sourceAbsolute(fullFilePathSRC);//source name texture
        StringConverter lightfilename(FilePath::getFilename(_sourceRelative));
        String fileConfigurationfilename(_sourceRelative + ".tfi");
        String directoryConfigurationfilename = szDirectory + "Config.tfi";

#ifdef ITF_SUPPORT_RAKI
        // check wave is correct format 
        ReadFileStream checkStream;
        if ( checkStream.open( sourceAbsolute.getChar() ) )
        {
            raki::WaveFile waveFile;
            raki::Serializer readSerializer( &checkStream );

            raki::RiffChunkManager::singleton().setShouldReadDataBlockForWaveData( false );

            waveFile.serialize( readSerializer );

            const char fmtType[] = "fmt ";
            WAVEFORMATEX * waveformat = (WAVEFORMATEX*) waveFile.getChunkData( fmtType );

            if ( waveformat )
            {
                if ( waveformat->wFormatTag != WAVE_FORMAT_PCM )
                {
                    ITF_ASSERT_MSG("SoundCooker ERROR: %s ==> %s INVALID WAVE FORMAT", lightfilename.getChar(),shortName.getChar() );
                    LOG_COOKER("[SOUND_COOKER] ERROR: %s ==> %s INVALID WAVE FORMAT", lightfilename.getChar(),shortName.getChar() );
                    return bfalse;
                }

                if ( ( waveformat->nChannels < 1 ) || ( waveformat->nChannels > 2 ) )
                {
                    ITF_ASSERT_MSG("SoundCooker ERROR: %s ==> %s INVALID NB CHANNELS", lightfilename.getChar(),shortName.getChar() );
                    LOG_COOKER("[SOUND_COOKER] ERROR: %s ==> %s INVALID NB CHANNELS", lightfilename.getChar(),shortName.getChar() );
                    return bfalse;
                }

                if ( waveformat->nSamplesPerSec != 48000 )
                {
                    ITF_ASSERT_MSG( 0, "SoundCooker ERROR: %s ==> %s INVALID SAMPLING RATE", lightfilename.getChar(),shortName.getChar() );
                    LOG_COOKER("[SOUND_COOKER] ERROR: %s ==> %s INVALID SAMPLING RATE", lightfilename.getChar(),shortName.getChar() );
                    return bfalse;
                }

                if ( waveformat->wBitsPerSample != 16 )
                {
                    ITF_ASSERT_MSG("SoundCooker ERROR: %s ==> %s INVALID BITS PER SAMPLE", lightfilename.getChar(),shortName.getChar() );
                    LOG_COOKER("[SOUND_COOKER] ERROR: %s ==> %s INVALID BITS PER SAMPLE", lightfilename.getChar(),shortName.getChar() );
                    return bfalse;
                }

            }
            else
            {
                ITF_ASSERT_MSG("SoundCooker ERROR: %s ==> %s FAILED TO FIND FORMAT", lightfilename.getChar(),shortName.getChar() );
                LOG_COOKER("[SOUND_COOKER] ERROR: %s ==> %s FAILED TO FIND FORMAT", lightfilename.getChar(),shortName.getChar() );
                return bfalse;
            }

            checkStream.close();
        }
        else
        {
            ITF_ASSERT_MSG("SoundCooker ERROR: %s ==> %s FAILED TO OPEN FILE", lightfilename.getChar(),shortName.getChar() );
            LOG_COOKER("[SOUND_COOKER] ERROR: %s ==> %s FAILED TO OPEN FILE", lightfilename.getChar(),shortName.getChar() );
            return bfalse;
        }

#endif


        SoundConfiguration* pSoundConfiguration = NULL;
        
        u64 timeConfiguration = 0;

        enum Config
        { 
            defaultConfig, temporaryConfig, fileConfig, directoryConfig 
        };

        Config config = defaultConfig;
             

        if ( _bUseTemporaryConfig )
        {
            pSoundConfiguration = openSoundConfiguration("ConfigTemporary.tfi", _szPlatform);
            timeConfiguration = FILEMANAGER->getLastTimeWriteAccess("ConfigTemporary.tfi");
            config = temporaryConfig;
        }

        if ( ( !pSoundConfiguration ) && FILEMANAGER->fileExists( fileConfigurationfilename ) )
        {
            pSoundConfiguration = openSoundConfiguration( fileConfigurationfilename, _szPlatform );
            timeConfiguration = FILEMANAGER->getLastTimeWriteAccess( fileConfigurationfilename );
            config = fileConfig;
        }

        if ( ( !pSoundConfiguration ) && FILEMANAGER->fileExists( directoryConfigurationfilename ) )
        {
            pSoundConfiguration = openSoundConfiguration( directoryConfigurationfilename, _szPlatform );
            timeConfiguration = FILEMANAGER->getLastTimeWriteAccess( directoryConfigurationfilename );
            config = directoryConfig;
        }

        if ( !pSoundConfiguration )
        {
            pSoundConfiguration = new SoundConfiguration();
            if (_szPlatform == "PC" || _szPlatform == "FINALPC" || _szPlatform == "EMUWII")
            { 
                pSoundConfiguration->setFormat("ADPCM");
                pSoundConfiguration->setAdpcmBlockSize(128);
            }
            else if (_szPlatform == "X360")
            {
                pSoundConfiguration->setFormat("XMA2");
                pSoundConfiguration->setXma2Compression(90);
            }
            else if (_szPlatform == "PS3")
            {
                pSoundConfiguration->setFormat("PCM");
            }
            else if (_szPlatform == "WII")
            {
                pSoundConfiguration->setFormat("ADPCM");
            }
            else if (_szPlatform == "PS5")
            {
                pSoundConfiguration->setFormat("PCM");
            }
            else if (_szPlatform == "NX")
            {
                pSoundConfiguration->setFormat("PCM");
            }
            else if (_szPlatform == "OUNCE")
            {
                pSoundConfiguration->setFormat("PCM");
            }
            else
            {
                ITF_ASSERT(0);
            }
        }

        // temp... force formats...
        if ( _szPlatform == "PS3" )
        {
            pSoundConfiguration->setFormat("MP3");
            pSoundConfiguration->setIsMusic(false);
        }
        else if (_szPlatform == "X360")
        {
            pSoundConfiguration->setFormat("XMA2");
            pSoundConfiguration->setXma2Compression(90);
            pSoundConfiguration->setIsMusic(false);
        }

        u64 timeWrite   = FILEMANAGER->getLastTimeWriteAccess(_sourceRelative);
        u64 dst         = FILEMANAGER->getLastTimeWriteAccess(destRelative);

        if (_bUseTemporaryConfig == bfalse && dst == timeWrite && ((timeConfiguration == 0) || timeConfiguration == timeWrite))
        {
            LOG_COOKER("[SOUND COOKER] %s ==> %s SKIPPED", lightfilename.getChar(),shortName.getChar());
            SF_DEL(pSoundConfiguration);
            return btrue;
        } 
  

		const String& exeDir =  FILESERVER->getApplicationDirectory().cStr();

        bool swap = false;

        if ( pSoundConfiguration->getIsMusic() )
        {
            if ( _szPlatform == "PS3" )
            {
                pSoundConfiguration->setFormat( "PCM" );
                swap = true;
            }

            if ( _szPlatform == "WII" )
            {
#ifdef ITF_SUPPORT_RAKI
                return WiiCompressStreamToDspadcpm( exeDir, sourceAbsolute.getChar(), destAbsolute.getChar(), 32000 );
#else
                return false;
#endif
            }

            if ( !compressMusic( pSoundConfiguration, _szPlatform, lightfilename, shortName, sourceAbsolute, destAbsolute, exeDir ) )
                return bfalse;
        }
        else
        {
            
            if ( _szPlatform == "WII" )
            {
#ifdef ITF_SUPPORT_RAKI
                return WiiCompressBufferToDspadcpm( exeDir, sourceAbsolute.getChar(), destAbsolute.getChar(), 32000/*24000*/ );
#else
                return false;
#endif
            }
            else if (pSoundConfiguration->getFormat() == "PCM")
            {
                DeleteFileA( destAbsolute.getChar() );

                if (!CopyFileA(sourceAbsolute.getChar(), destAbsolute.getChar(),TRUE))
                {
                    LOG_COOKER("[SOUND COOKER] %s ==> %s FAILED TO COPY FILE", lightfilename.getChar(),shortName.getChar());
                    SF_DEL(pSoundConfiguration);
                    return bfalse;
                }

                SetFileAttributesA( destAbsolute.getChar(), 0 );
            }
            else if (pSoundConfiguration->getFormat() == "ADPCM")
            {
                if (pSoundConfiguration->getAdpcmBlockSize() != 512 &&
                    pSoundConfiguration->getAdpcmBlockSize() != 256 &&
                    pSoundConfiguration->getAdpcmBlockSize() != 128 &&
                    pSoundConfiguration->getAdpcmBlockSize() != 64 &&
                    pSoundConfiguration->getAdpcmBlockSize() != 32 )
                {
                    LOG_COOKER("[SOUND COOKER] %s ==> %s FAILED TO CONVERT TO ADPCM invalid block size %d", lightfilename.getChar(),shortName.getChar(),pSoundConfiguration->getAdpcmBlockSize());
                    SF_DEL(pSoundConfiguration);
                    return bfalse;
                }
                ProcessSpawner processSpawner;
                String commandline;

                String exePath;
                if (useBatch)
                {
                    commandline.setTextFormat("%ls %d %s %s",exeDir.cStr(),pSoundConfiguration->getAdpcmBlockSize(),sourceAbsolute.getChar(),destAbsolute.getChar());
                    exePath = exeDir + "\\adpcmencode.bat";
                }
                else
                {
                    commandline.setTextFormat("-b %d %s %s",pSoundConfiguration->getAdpcmBlockSize(),sourceAbsolute.getChar(),destAbsolute.getChar());
                    exePath = exeDir + "\\adpcmencode.exe";
                }

                i32 res = processSpawner.create(exePath ,commandline,"");
                if (res)
                {
                    LOG_COOKER("[SOUND COOKER] %s ==> %s FAILED TO CONVERT TO ADPCM, %s", lightfilename.getChar(),shortName.getChar(),StringConverter(processSpawner.getLog()).getChar());
                    SF_DEL(pSoundConfiguration);
                    return bfalse;
                }
                else
                {
                    LOG_COOKER("[SOUND COOKER] %s ==> %s COOKED", lightfilename.getChar(),shortName.getChar());
                }
            }
            else if (pSoundConfiguration->getFormat() == "XMA2")
            {
                if (pSoundConfiguration->getXma2Compression() < 1 || pSoundConfiguration->getXma2Compression() > 100)
                {
                     LOG_COOKER("[SOUND COOKER] %s ==> %s FAILED TO CONVERT TO XMA2 invalid compression value %d", lightfilename.getChar(),shortName.getChar(),pSoundConfiguration->getXma2Compression());
                    SF_DEL(pSoundConfiguration);
                    return bfalse;
                }
                ProcessSpawner processSpawner;
                String commandline;

                commandline.setTextFormat( "%s /TargetFile %s /Blocksize 2 /LoopWholeFile /Quality %d", sourceAbsolute.getChar(), destAbsolute.getChar(), pSoundConfiguration->getXma2Compression() );
                String exePath = String(exeDir)+"\\x360\\xma2encode.exe";
                i32 res = processSpawner.create(exePath,commandline,"");
                if (res)
                {
                    LOG_COOKER("[SOUND COOKER] %s ==> %s FAILED TO CONVERT TO XMA, %s", lightfilename.getChar(),shortName.getChar(),StringConverter(processSpawner.getLog()).getChar());
                    SF_DEL(pSoundConfiguration);
                    return bfalse;
                }
                else
                {
                   LOG_COOKER("[SOUND COOKER] %s ==> %s COOKED", lightfilename.getChar(),shortName.getChar());
                }
            }
            else if (pSoundConfiguration->getFormat() == "XWMA")
            {
                ProcessSpawner processSpawner;
                String commandline;
                commandline.setTextFormat("%s /TargetFile %s /Quality %d",sourceAbsolute.getChar(),destAbsolute.getChar(),pSoundConfiguration->getXma2Compression());
                i32 res = processSpawner.create("xwmaencode.exe",commandline,"");
                if (res)
                {
                    LOG_COOKER("[SOUND COOKER] %s ==> %s FAILED TO CONVERT TO XWMA, %s", lightfilename.getChar(),shortName.getChar(),StringConverter(processSpawner.getLog()).getChar());
                    SF_DEL(pSoundConfiguration);
                    return bfalse;
                }
                else
                {
                    LOG_COOKER("[SOUND COOKER] %s ==> %s COOKED", lightfilename.getChar(),shortName.getChar());
                }

            }
            else if (pSoundConfiguration->getFormat() == "MP3")
            {
                if ( _szPlatform == "PS3" )
                {
                    ProcessSpawner processSpawner;
                    char pathTempMSF[1024];
                    sprintf( pathTempMSF, "%s.tmp", destAbsolute.getChar() );

                    String commandline;
                    commandline.setTextFormat("-in %s -out %s -mp3 %d -x -loop -ignorewavloop", 
                        sourceAbsolute.getChar(),
                        pathTempMSF,
                        192 );//pSoundConfiguration->getMP3Compression() );
                    String exePath = String(exeDir)+"\\ps3\\MSEnc.exe";

                    i32 res = processSpawner.create(exePath, commandline, "");
                    if (res)
                    {
                        LOG_COOKER("[SOUND COOKER] %s ==> %s FAILED TO CONVERT TO PS3 DATA TO MP3, %s", lightfilename.getChar(),shortName.getChar(),StringConverter(processSpawner.getLog()).getChar());
                        SF_DEL(pSoundConfiguration);
                        return bfalse;
                    }
                    else
                    {
                        bool done = false;
#ifdef ITF_SUPPORT_RAKI
                        raki::RiffChunkManager::singleton().setIsWritingToBigEndians( false );
                        raki::RiffChunkManager::singleton().setShouldReadDataBlockForWaveData( false );

                        ReadFileStream inputStream;
                        if ( inputStream.open( sourceAbsolute.getChar() ) )
                        {
                            const char fmtType[] = "fmt ";
                            raki::WaveFile inputWaveFile;
                            raki::Serializer readSerializer( &inputStream );

                            inputWaveFile.serialize( readSerializer );

                            if ( inputWaveFile.getChunkData( fmtType ) )
                            {
                                BinFileReader msEncFile;
                                if ( msEncFile.readFrom( String( pathTempMSF ) ) && msEncFile.getData() 
                                    && ( msEncFile.getSize() > 64 ) ) 
                                {
                                    raki::WaveFile outputWaveFile;
                                    outputWaveFile.createEmptyRiffChunk();

                                    WAVEFORMATEX * format = (WAVEFORMATEX*) inputWaveFile.getChunkData( fmtType );

                                    WAVEFORMATEX mp3Format = { 0 };
                                    mp3Format.wFormatTag       = WAVE_FORMAT_MPEGLAYER3;
                                    mp3Format.nChannels        = format->nChannels;
                                    mp3Format.nSamplesPerSec   = format->nSamplesPerSec;
                                    mp3Format.nAvgBytesPerSec  = format->nAvgBytesPerSec;
                                    mp3Format.nBlockAlign      = format->nBlockAlign;
                                    mp3Format.wBitsPerSample   = format->wBitsPerSample;

                                    // add mp3 fmt chunk 
                                    outputWaveFile.addChunk( fmtType, sizeof( WAVEFORMATEX), (void*)&mp3Format );

                                    // add MSEnc data 
                                    void * msEncData = (void*)( 64 + (u64) msEncFile.getData() );
                                    outputWaveFile.addChunk( "msf ", ( (u32)msEncFile.getSize()) - 64, msEncData );

                                    outputWaveFile.recalculateRiffAndListSizes();

                                    WriteFileStream outputFileStream;
                                    if ( outputFileStream.open( String( destAbsolute.getChar() ) ) )
                                    {
                                        outputWaveFile.changeToBigEndian();

                                        raki::RiffChunkManager::singleton().setIsWritingToBigEndians( true );

                                        raki::Serializer writeSerializer( &outputFileStream );

                                        outputWaveFile.serialize( writeSerializer );

                                        DeleteFileA( pathTempMSF ); 

                                        done = true;
                                    }
                                }
                            }
                        }


                        if ( done )
                        {
                            LOG_COOKER("[SOUND COOKER] %s ==> %s COOKED", lightfilename.getChar(),shortName.getChar());
                        }
                        else
                        {
                            LOG_COOKER("[SOUND COOKER] %s ==> %s FAILED CONVERTING PS3 DATA TO COOKED, %s", lightfilename.getChar(),shortName.getChar(),StringConverter(processSpawner.getLog()).getChar());
                            SF_DEL(pSoundConfiguration);
                            return bfalse;
                        }

#else
                        return false;
#endif // endif
                    }

                }

            }
            else if (pSoundConfiguration->getFormat() == "ATRAC")
            {

            }
        }

        if ( _szPlatform == "X360" )
            swap = true;

        if ( swap )
        {
            // endian swap for 360
#ifdef ITF_SUPPORT_RAKI
            raki::Specific::RewriteRiffFile rewriteRiffFile( true );

            ReadFileStream readStream;
            if ( !readStream.open( destAbsolute.getChar() ) )
                return bfalse;
            rewriteRiffFile.read( &readStream );
            readStream.close();

            WriteFileStream writeStream;
            if ( !writeStream.open( destAbsolute.getChar() ) )
                return bfalse;
            rewriteRiffFile.write( &writeStream );
#endif
        }
        
        
        //flush also the configuration file
        if ( (timeConfiguration) && ( config == fileConfig ) )
            FILEMANAGER->flushTimeWriteAccess( fileConfigurationfilename, timeWrite );

        FILEMANAGER->flushTimeWriteAccess(destRelative, timeWrite);

        SF_DEL(pSoundConfiguration);

        return btrue;
    }

    bbool soundCooker::dependencyFiles( const String& cacheKey, const String& platform, const String& filename, DepCollection& dependencies )
    {
        if ( platform != "WII" && platform != "PS3" )
            return btrue;

        String altPath;

        altPath = FilePath::getDirectory( filename );
        altPath += FilePath::getFilenameWithoutExtension( filename );
        altPath += "_50fps.wav";
        if ( FILEMANAGER->fileExists( altPath ) )
            dependencies.add( altPath );
        dependencies.filterExt();
        return btrue;
    }

}
