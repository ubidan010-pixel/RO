#include "precompiled_engine.h"

#ifndef _ITF_ANIMATION_SERIALIZE_H_
#include "engine/animation/AnimationSerialize.h"
#endif //_ITF_ANIMATION_SERIALIZE_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_DIRECTORY_H_
#include  "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_ 

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif // _ITF_FILESERVER_H_

namespace ITF
{

    AnimationSerialize::AnimationSerialize()
    : m_cookCallback(NULL)
    {

    }

    AnimationSerialize::~AnimationSerialize()
    {

    }

    bbool AnimationSerialize::openBinary( String _cookFile, bbool _castWarning)
    {
        if (!_castWarning &&
            !FILEMANAGER->fileExists(_cookFile))
            return bfalse;

        File*  binaryFile = FILEMANAGER->openFile(_cookFile ,ITF_FILE_ATTR_READ);
        if (binaryFile)
        {
            u32 lenght = 0;

            lenght = (u32) binaryFile->getLength();
            ITF_ASSERT(lenght);
            if (lenght == 0)
            {
                FILEMANAGER->closeFile(binaryFile);
                return bfalse;
            }

            u32 length = (u32) binaryFile->getLength();

            ArchiveMemory rd(length,length,btrue);
            binaryFile->read(rd.getPtrForWrite(),length);
            rd.rewindForReading();
            FILEMANAGER->closeFile(binaryFile);

            rd.createLinker();
            serialize(rd, bfalse);
            link();

            return btrue;
        }

        return bfalse;
    }

    bbool AnimationSerialize::saveBinary( String _cookFile)
    {
        ArchiveMemory amw;
        amw.createLinker();
        serialize(amw, bfalse);

        File* writeBinary = FILEMANAGER->openFile(_cookFile,ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);

        ITF_WARNING(NULL, amw.getSize(), "Trying to save %s with corrupted data !", StringToUTF8(_cookFile).get() );
        if (writeBinary)
        {
            writeBinary->write(amw.getData(),amw.getSize());
            FILEMANAGER->closeFile(writeBinary);
            return btrue;
        }

        ITF_ASSERT(0);
        return bfalse;
    }

    bbool AnimationSerialize::loadFile( const String& _szFilename, const String& _szPlatform, bbool _tryLoadingBin, bbool _raiseError )
    {
        String               cookFile   = FILESERVER->getCookedNamePlatform(_szFilename,_szPlatform);

#ifdef ITF_SUPPORT_COOKING
        // If we are on cook, force file cooking
        if (m_cookCallback)
        {
            m_cookCallback(_szPlatform, _szFilename, _raiseError, !_tryLoadingBin);
        }
#endif

        if (!openBinary(cookFile,btrue))
            return bfalse;

#ifdef ITF_SUPPORT_COOKING
        // If we are on cook, force file cooking
        if (m_cookCallback && needRecook())
        {
            clear();
            m_cookCallback(_szPlatform,_szFilename, _raiseError, btrue);
            if (!openBinary(cookFile,btrue))
                return bfalse;
        }
#endif

        postProcess(_szFilename,_szPlatform,_tryLoadingBin);
        return btrue;
    }
};
