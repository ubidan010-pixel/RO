#ifndef _ITF_FILE_IPAD_H_
#define _ITF_FILE_IPAD_H_

//////////////////////////////////////////////////////////////////////////
///
/// File manipulation class
///

#import <Foundation/Foundation.h>

#define FILE_BEGIN           0
#define FILE_CURRENT         1
#define FILE_END             2

namespace ITF
{
    class hwFile : public File
	{
		public:
			hwFile();
			~hwFile();
			bbool			open(const String& path, u32 attribute,u32 _flagsAttribute = 0);
			void			close();
			u64				getLength();
			bbool			read(void *pBuffer, u32 size,u32* sizeRead);
			bbool			read(void *pBuffer, u32 size);
			bbool			write(const void *pBuffer, u32 size,u32* sizeWritten);
			bbool			write(const void *pBuffer, u32 size);
			u64				seek(i64 offset, u16 from);
			FILE_HANDLE		getHandle();
			
			static bbool	CopyFile(const String& szExistingFilename,const String& szNewFilename);
			static bbool	MoveFile(const String& szExistingFilename,const String& szNewFilename);
			static u64		getLastTimeWriteAccess(const String& szExistingFilename);
			static void		flushTimeWriteAccess(const String& szExistingFilename, const u64& _lastWrite);
			static bbool	exists(const String& _fullfileName);
			static bbool	deleteFile(const String& _fullfilePath);
			static u64		getFileSize(const String& _fullfilePath);
			
		protected:
			FILE_HANDLE		m_Handle;
		
		private:
			friend class Directory;
			static NSString* GetApplicationBunddleFullName(const String& path);
			static NSString* GetSettingsBunddleFullName(const String& path);
    };

} // namespace ITF



#endif // _ITF_FILE_IPAD_H_
