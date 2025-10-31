#include "precompiled_Adapter_WWISE.h"
#ifdef ITF_SUPPORT_WWISE
#include <math.h>
#include "adapters/Adapter_WWISE/AudioSDK/Win32/SoundRecorder_win.h"
#include "adapters/Adapter_WWISE/AudioSDK/error.h"
#include "adapters/Adapter_WWISE/AudioSDK/SoundInputDeviceManager.h"

#define BUFFER_STATUS_NOT_READY 0
#define BUFFER_STATUS_READY		1

namespace	AudioSDK
{

	//
	// ===================================================================================================================
	//    Writes data from output into given buffer
	// ===================================================================================================================
	//
	SoundRecorder_win32::SoundRecorder_win32() :
		m_fileHandle(NULL),
		m_successfullyInitOnceAndMustSleepOnExit(false),
		m_waveInHandle(NULL),
		m_dataSizeSeek(0),
		m_dataSize(0),
		m_headerSize(0),
        m_captureListener(NULL)
	{
		memset(&m_waveFormatEx, 0, sizeof(m_waveFormatEx));
		memset(m_waveHeader, 0, sizeof(m_waveHeader));
		m_waveHeader[0].lpData = NULL;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	SoundRecorder_win32::~SoundRecorder_win32()
	{
		stopRecording();

		// Avoid deadlock in waveIn
		if(m_successfullyInitOnceAndMustSleepOnExit) Sleep(100);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void CALLBACK SoundRecorder_win32::s_captureCallback(HWAVEIN /* hwi */, UINT _msg, DWORD_PTR _instance, DWORD_PTR _param1, DWORD_PTR /* _param2 */ )
	{
		SoundRecorder_win32 *pthis = reinterpret_cast < SoundRecorder_win32 * > (_instance);
		switch(_msg)
		{
		case WIM_DATA:
			{
				WAVEHDR *hdr = (WAVEHDR *)_param1;
				pthis->m_dataSize += fwrite(hdr->lpData, 1, hdr->dwBytesRecorded, pthis->m_fileHandle);
                if(pthis->m_captureListener) pthis->m_captureListener->onBuffer(hdr->lpData, hdr->dwBytesRecorded);

				MMRESULT	result = waveInPrepareHeader(pthis->m_waveInHandle, hdr, sizeof(WAVEHDR));
				AUDIO_ASSERT(MMSYSERR_NOERROR == result, "waveInPrepareHeader failed");

				result = waveInAddBuffer(pthis->m_waveInHandle, hdr, sizeof(WAVEHDR));
				AUDIO_ASSERT(MMSYSERR_NOERROR == result, "waveInAddBuffer failed");
			}
			break;

		case WIM_CLOSE:
			//
			// Notification meaning that the WaveInput was closed. Could be - A disconnected
			// device - An error in the driver - Normal closing. No use for this at the
			// moment. If you add something here, make sure you don’t call anything on the
			// system directly from the callback.
			//
			break;

		default:
			break;
		}
	}

    static const char*mmresToString(MMRESULT _mmresult)
    {
        switch(_mmresult)
        {
        case MMSYSERR_NOERROR      : return "";
        case MMSYSERR_ERROR        : return "unspecified error";
        case MMSYSERR_BADDEVICEID  : return "device ID out of range";
        case MMSYSERR_NOTENABLED   : return "driver failed enable";
        case MMSYSERR_ALLOCATED    : return "device already allocated";
        case MMSYSERR_INVALHANDLE  : return "device handle is invalid";
        case MMSYSERR_NODRIVER     : return "no device driver present";
        case MMSYSERR_NOMEM        : return "memory allocation error";
        case MMSYSERR_NOTSUPPORTED : return "function isn't supported";
        case MMSYSERR_BADERRNUM    : return "error value out of range";
        case MMSYSERR_INVALFLAG    : return "invalid flag passed";
        case MMSYSERR_INVALPARAM   : return "invalid parameter passed";
        case MMSYSERR_HANDLEBUSY   : return "handle being used simultaneously on another thread (eg callback)";
        case MMSYSERR_INVALIDALIAS : return "specified alias not found";
        case MMSYSERR_BADDB        : return "bad registry database";
        case MMSYSERR_KEYNOTFOUND  : return "registry key not found";
        case MMSYSERR_READERROR    : return "registry read error";
        case MMSYSERR_WRITEERROR   : return "registry write error";
        case MMSYSERR_DELETEERROR  : return "registry delete error";
        case MMSYSERR_VALNOTFOUND  : return "registry value not found";
        case MMSYSERR_NODRIVERCB   : return "driver does not call DriverCallback";
        case MMSYSERR_MOREDATA     : return "more data to be returned";
        default:                     return "unknown MMRESULT code";
        }
    }

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	bool SoundRecorder_win32::startRecording(const char *_filePath, u32 _deviceID, u32 _channel, u32 _sampleRate)
	{
		// stop before starting any one
		if(m_waveInHandle) stopRecording();

		// clear wave hdr
		ZeroMemory(&m_waveHeader, sizeof(m_waveHeader));

		// get waveformatex
		SoundInputDeviceManager::s_getSingleton()->getCompatibleFormat(&m_waveFormatEx, _deviceID, _channel, _sampleRate);

        if(m_captureListener) m_captureListener->onStart(m_waveFormatEx);

		// open device
		if(waveInOpen(&m_waveInHandle, _deviceID, (LPCWAVEFORMATEX) & m_waveFormatEx, (DWORD_PTR) s_captureCallback, (DWORD_PTR) this, CALLBACK_FUNCTION) == MMSYSERR_NOERROR)
		{
			MMRESULT	mmresult=MMSYSERR_NOERROR;

			// allocate buffers
			m_waveHeader[0].dwBufferLength = getFullBufferSize();
			m_waveHeader[0].lpData = (i8 *)malloc(m_waveHeader[0].dwBufferLength * AUDIO_INPUT_NUM_BUFFERS);
			if(m_waveHeader[0].lpData)
			{
				memset(m_waveHeader[0].lpData, 0, getFullBufferSize() * AUDIO_INPUT_NUM_BUFFERS);
				if(0 == fopen_s(&m_fileHandle, _filePath, "wb"))
				{
					u32 chunkID = 'FFIR';
					u32 chunkSize = 0;
					fwrite(&chunkID, 1, sizeof(chunkID), m_fileHandle);
					fwrite(&chunkSize, 1, sizeof(chunkSize), m_fileHandle);

					m_headerSize = 0;
					chunkID = 'EVAW';
					m_headerSize += fwrite(&chunkID, 1, sizeof(chunkID), m_fileHandle);

					chunkID = ' tmf';
                    if(m_waveFormatEx.Format.formatTag == AudioStructs::FORMAT_EXTENSIBLE)
                    {
                        chunkSize = sizeof(m_waveFormatEx);
                        m_headerSize += fwrite(&chunkID, 1, sizeof(chunkID), m_fileHandle);
                        m_headerSize += fwrite(&chunkSize, 1, sizeof(chunkSize), m_fileHandle);
                        m_headerSize += fwrite(&m_waveFormatEx, 1, sizeof(m_waveFormatEx), m_fileHandle);
                    }
                    else
                    {
                        chunkSize = sizeof(m_waveFormatEx.Format);
                        m_headerSize += fwrite(&chunkID, 1, sizeof(chunkID), m_fileHandle);
                        m_headerSize += fwrite(&chunkSize, 1, sizeof(chunkSize), m_fileHandle);
                        m_headerSize += fwrite(&m_waveFormatEx.Format, 1, sizeof(m_waveFormatEx.Format), m_fileHandle);
                    }

					chunkID = 'atad';
					chunkSize = 0;
					m_headerSize += fwrite(&chunkID, 1, sizeof(chunkID), m_fileHandle);
                    const i32 tell = (i32)ftell(m_fileHandle);
					m_dataSizeSeek = tell>0 ? (u32)tell : 0;
					m_dataSize = 0;
					m_headerSize += fwrite(&chunkSize, 1, sizeof(chunkSize), m_fileHandle);

					// Prepare the WAVEHDR's
					for(i32 i = 0; i < AUDIO_INPUT_NUM_BUFFERS; ++i)
					{
						if(i > 0)
						{
							m_waveHeader[i].dwBufferLength = getFullBufferSize();
							m_waveHeader[i].lpData = m_waveHeader[i - 1].lpData + getFullBufferSize();
						}

						if((mmresult = waveInPrepareHeader(m_waveInHandle, &m_waveHeader[i], sizeof(WAVEHDR))) != MMSYSERR_NOERROR) break;
						if((mmresult = waveInAddBuffer(m_waveInHandle, &m_waveHeader[i], sizeof(WAVEHDR))) != MMSYSERR_NOERROR) break;
					}
				}
				else
				{
					mmresult = MMSYSERR_WRITEERROR;
				}
			}
			else
			{
				mmresult = MMSYSERR_NOMEM;
			}

			if(mmresult == MMSYSERR_NOERROR)
			{
				// start getting input
				if(waveInStart(m_waveInHandle) == MMSYSERR_NOERROR)
				{
					m_successfullyInitOnceAndMustSleepOnExit = true;
					return true;
				}
			}

            AUDIO_WARNING("Capture failed : \"%s\" (code %d)", mmresToString(mmresult), mmresult);

			stopRecording();
		}

		return false;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	bool SoundRecorder_win32::stopRecording()
	{
        if(m_captureListener) m_captureListener->onStop();

		// stop getting input
		if(m_waveInHandle)
		{
			waveInStop(m_waveInHandle);
			waveInClose(m_waveInHandle);
			m_waveInHandle = NULL;
		}

		// Free the recording buffers
		if(m_waveHeader[0].lpData)
		{
			free(m_waveHeader[0].lpData);
			m_waveHeader[0].lpData = NULL;
		}

		if(m_fileHandle)
		{
			// data chunk size
			if(0==fseek(m_fileHandle, m_dataSizeSeek, SEEK_SET))
                fwrite(&m_dataSize, 1, sizeof(m_dataSize), m_fileHandle);

            // RIFF chunk size
            m_headerSize += m_dataSize;
            if(0==fseek(m_fileHandle, sizeof(u32), SEEK_SET))
                fwrite(&m_headerSize, 1, sizeof(m_headerSize), m_fileHandle);
                
            fclose(m_fileHandle);
            m_fileHandle = NULL;
		}

		return true;
	}
}
#endif
