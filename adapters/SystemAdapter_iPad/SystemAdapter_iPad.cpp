#include "precompiled_SystemAdapter_IPAD.h"

#ifndef _ITF_SYSTEMADAPTER_IPAD_H_
#include "adapters/SystemAdapter_iPad/SystemAdapter_iPad.h"
#endif //_ITF_SYSTEMADAPTER_IPAD_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_INPUTADAPTER_IPAD_H_
#include "adapters/InputAdapter_iPad/InputAdapter_iPad.h"
#endif //_ITF_INPUTADAPTER_IPAD_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#define MOVE_FACTOR .002f

@implementation EAGLView

+(Class) layerClass
{
    return [CAEAGLLayer class];
}

-(id) initWithFrame : (CGRect) frame
{
	self = [super initWithFrame:frame];
    if(self)
    {
        CAEAGLLayer *glLayer = (CAEAGLLayer*)super.layer;
        glLayer.opaque = YES;
		
        [self setMultipleTouchEnabled:YES];
		
        m_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
        [EAGLContext setCurrentContext:m_context];
		
        glGenFramebuffersOES(1, &m_frameBuffer);
        glGenRenderbuffersOES(1, &m_renderBuffer);
		
        glBindFramebufferOES(GL_FRAMEBUFFER_OES, m_frameBuffer);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, m_renderBuffer);
		
        [m_context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable: glLayer];
		
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, m_renderBuffer);
    }
	
    return self;
}

-(void) swapBuffers
{
    [m_context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

-(void) dealloc
{
    glDeleteFramebuffersOES(1, &m_frameBuffer);
    glDeleteRenderbuffersOES(1, &m_renderBuffer);
	
    [EAGLContext setCurrentContext:nil];
    [m_context release];
    [super dealloc];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	static_cast<ITF::InputAdapter_iPad*>(ITF::INPUT_ADAPTER)->onTouch(touches, self);
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	static_cast<ITF::InputAdapter_iPad*>(ITF::INPUT_ADAPTER)->onTouch(touches, self);
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	static_cast<ITF::InputAdapter_iPad*>(ITF::INPUT_ADAPTER)->onTouch(touches, self);
}

- (void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration
{  
	static_cast<ITF::InputAdapter_iPad*>(ITF::INPUT_ADAPTER)->onAccelerometer(acceleration.x, acceleration.y, acceleration.z);
}

- (void)addVirtualButton:(CGRect)zone :(const char*)texture
{
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	NSString* path = [[NSBundle mainBundle] bundlePath];
	NSString* fullpath = [path stringByAppendingString:@"/"];
	NSString* tmpfilename = [[NSString alloc] initWithCString:texture]; 
	NSString* pathFileName = [fullpath stringByAppendingString:tmpfilename];
	
	UIImageView* button = [[UIImageView alloc] initWithFrame:zone];
	button.image = [UIImage imageWithContentsOfFile: pathFileName];
	button.transform = CGAffineTransformMakeRotation(1.57079633);
	
	[self addSubview:button];
	[pool release];
}

- (void)setVirtualJoyStick:(CGRect)zone :(const char*)baseTexture :(const char*)texture
{
	[self addVirtualButton:zone :baseTexture];
	
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	NSString* path = [[NSBundle mainBundle] bundlePath];
	NSString* fullpath = [path stringByAppendingString:@"/"];
	NSString* tmpfilename = [[NSString alloc] initWithCString:texture]; 
	NSString* pathFileName = [fullpath stringByAppendingString:tmpfilename];
	
	zone.origin.x += zone.size.width * .25f;
	zone.origin.y += zone.size.height * .25f;
	zone.size.width *= .5f;
	zone.size.height *= .5f;
	
	m_virtualJoyStick = [[UIImageView alloc] initWithFrame:zone];
	m_virtualJoyStick.image = [UIImage imageWithContentsOfFile: pathFileName];
	m_virtualJoyStick.transform = CGAffineTransformMakeRotation(1.57079633);
	
	[self addSubview:m_virtualJoyStick];
	[pool release];
}

- (void)updateVirtualJoyStickPosition:(int)x :(int)y
{
	[m_virtualJoyStick setCenter:CGPointMake(x, y)];
}
@end

namespace ITF
{
	bbool SystemAdapter_iPad::initialize()
	{
		m_applicationInitTime = getTime();
		m_dt = 0.f;
		
		m_fPs = 0;
		m_numFrames = 0;
		m_lastFPSTime = getTime();
		return btrue;
	}
	
	bbool SystemAdapter_iPad::openGraphics(int, int, int, int, int, bbool, bbool, const String&, bbool)
	{
		CGRect	rect = [[UIScreen mainScreen] applicationFrame];
		
		m_window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
		[m_window setBackgroundColor:[UIColor blackColor]];
		
		m_view = [[EAGLView alloc] initWithFrame:CGRectMake(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height)];
		[m_window addSubview:m_view];
		
		[m_window makeKeyAndVisible];  
		
		InputAdapter_iPad* inputAdapter = new InputAdapter_iPad();
		inputAdapter->setVirtualButtons(m_view);
		Singletons::get().setInputAdapter(inputAdapter);
		
		GFX_ADAPTER->setResolution(1024, 768);
		GFX_ADAPTER->init();

		return btrue;
	}
	
	void SystemAdapter_iPad::terminate()
	{
	}
	
	void SystemAdapter_iPad::present()
	{
        GFX_ADAPTER->setfPs(getfPs());
		
        // Present frame buffer.
        GFX_ADAPTER->endRendering();
        GFX_ADAPTER->present();
		
		[m_view swapBuffers];
		
		static_cast<InputAdapter_iPad*>(INPUT_ADAPTER)->update();
		m_numFrames++;
	}
	
	// Unit is seconds
	void SystemAdapter_iPad::timerStart()
	{
	}
	
	void SystemAdapter_iPad::timerStop()
	{
	}
	
	f64 SystemAdapter_iPad::getTime()
	{
		return CFAbsoluteTimeGetCurrent();
	}
	
	f64 SystemAdapter_iPad::getElapsedTime()
	{
		return getTime() - m_applicationInitTime;
	}
	
	f32 SystemAdapter_iPad::getfPs()
	{
        double time = getTime();
		
        if( time - m_lastFPSTime > 1.0 )
        {
            m_fPs = m_numFrames / ( time - m_lastFPSTime );
            m_lastFPSTime = time;
            m_numFrames  = 0;
        }
		
        return (f32)m_fPs;		
	}
	
	const char* SystemAdapter_iPad::getPlatformName()
	{
		return "iPad";
	}

	String SystemAdapter_iPad::userName() const
	{
		return String("iPad");
	}
	
	String SystemAdapter_iPad::dateTime() const
	{
		return String("dateTime");
	}
	
	void SystemAdapter_iPad::getTime(Time& time) const
	{
		CFGregorianDate date = CFAbsoluteTimeGetGregorianDate(CFAbsoluteTimeGetCurrent(), NULL);
		time.m_year = date.year;
		time.m_month = date.month;
		time.m_day = date.day;
		time.m_hour = date.hour;
		time.m_minute = date.minute;
		time.m_second = date.second;
	}
	
	i32 SystemAdapter_iPad::messageBox(const String& _title, const String& _msg, u32 _type)
	{
		NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
		NSString *title = [NSString stringWithFormat:@"%S", _title.cStr()];
		NSString *msg = [NSString stringWithFormat:@"%S\n", _msg.cStr()];
		
		NSLog(@"%@:%@", title, msg);
		[pool release];
		
		return 0;
	}
} // namespace ITF



