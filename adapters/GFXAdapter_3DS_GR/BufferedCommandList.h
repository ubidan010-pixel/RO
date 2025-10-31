#ifndef _ITF_3DS_BUFFEREDCOMMANDLIST_H_
#define _ITF_3DS_BUFFEREDCOMMANDLIST_H_

#ifdef ITF_CTR
#include <nn/gx.h>

namespace ITF
{

class BufferedCommandList
{
public :
	void Initialize();
	void Finalize();


public:
	void Wait();
	void Run();

private:
	static const u32	COMMANDLIST_SIZE	= 1024 * 1024 + 256 * 1024;
	static const u32	COMMANDLIST_REQUEST_NUMBER = 128;

	GLuint m_currentCommandListIndex;
	GLuint m_commandListIdArray[2];
};

inline void BufferedCommandList::Initialize()
{
	nngxGenCmdlists(2, &m_commandListIdArray[0]);
	for (u32 index = 0; index < 2; index++)
	{        
		nngxBindCmdlist(m_commandListIdArray[index]);
		nngxCmdlistStorage(COMMANDLIST_SIZE, COMMANDLIST_REQUEST_NUMBER);
		nngxSetCmdlistParameteri(NN_GX_CMDLIST_RUN_MODE, NN_GX_CMDLIST_SERIAL_RUN);
		nngxClearCmdlist();
	}

	m_currentCommandListIndex = 0;
	nngxBindCmdlist( m_commandListIdArray[m_currentCommandListIndex] );
}

inline void BufferedCommandList::Finalize()
{
	nngxDeleteCmdlists(2, &m_commandListIdArray[0]);
}

inline void BufferedCommandList::Wait()
{
	nngxSplitDrawCmdlist();

	nngxBindCmdlist( m_commandListIdArray[1 - m_currentCommandListIndex] );
	nngxWaitCmdlistDone();

	nngxClearCmdlist();
}

inline void BufferedCommandList::Run()
{
	nngxBindCmdlist( m_commandListIdArray[m_currentCommandListIndex] );
	nngxRunCmdlist();

	m_currentCommandListIndex = 1 - m_currentCommandListIndex;

	nngxBindCmdlist( m_commandListIdArray[m_currentCommandListIndex] );
}

}

#endif // ITF_CTR
#endif // _ITF_3DS_BUFFEREDCOMMANDLIST_H_