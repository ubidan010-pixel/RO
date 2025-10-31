#ifndef _ITF_SYSTEMADAPTER_IPAD_H_
#define _ITF_SYSTEMADAPTER_IPAD_H_

#ifdef ITF_IPAD

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <CoreFoundation/CoreFoundation.h>

@interface EAGLView : UIView
{    
@private
    EAGLContext*			m_context;
	
    GLuint					m_frameBuffer;
    GLuint					m_renderBuffer;
	UIImageView*			m_virtualJoyStick;
}
- (void)swapBuffers;
- (void)addVirtualButton:(CGRect)zone :(const char*)texture;
- (void)setVirtualJoyStick:(CGRect)zone :(const char*)baseTexture :(const char*)texture;
- (void)updateVirtualJoyStickPosition:(int)x :(int)y;
@end

namespace ITF
{
	class SystemAdapter_iPad : public SystemAdapter
	{
		public:
			bbool			initialize();
			bbool			openGraphics(int, int, int, int, int, bbool= bfalse, bbool = bfalse, const String& = "", bbool _waitVBL=btrue );
			void			terminate();
			void			present();
	
			// Unit is seconds
			void			timerStart();
			void			timerStop();
			f64				getTime();
			f64				getElapsedTime();
			f32				getfPs();
		
			const char*		getPlatformName();
			String			userName() const;
			String			dateTime() const;
			void			getTime(Time& time) const;
		
			i32				messageBox(const String& _title, const String& _msg, u32 _type) const;
		
		private:
			f64 m_applicationInitTime;

			f32	m_dt;
		
			u64 m_fPs;
			u64 m_numFrames;
			f64 m_lastFPSTime;
		
			UIWindow*		m_window;
			EAGLView*		m_view;
	};
} // namespace ITF

#endif //ITF_IPAD

#endif //_ITF_SYSTEMADAPTER_PS3_H_