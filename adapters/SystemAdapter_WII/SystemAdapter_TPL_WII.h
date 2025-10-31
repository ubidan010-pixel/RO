#ifdef ITF_WII

#ifndef _ITF_SYSTEMADAPTER_TPL_WII_H_
#define _ITF_SYSTEMADAPTER_TPL_WII_H_

#ifndef TPL_H
#include <revolution/tpl.h>
#endif //TPL_H

namespace ITF
{

	class SystemAdapter_TPL
	{
	  public:
	  	
		static void unpack(TPLPalettePtr _pal);
		static bbool readAndAlloc(TPLPalettePtr* _pal, const String& _path, u32 * _readBytes = NULL);
        static void draw(TPLPalettePtr _pal, Vec2d _pos, Vec2d _size, u8 _alpha = 255, bbool _drawStripe = bfalse);
	};
} // namespace ITF


#endif //_ITF_SYSTEMADAPTER_TPL_WII_H_

#endif //ITF_WII