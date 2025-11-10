#ifndef _ITF_FONT_H_
#define _ITF_FONT_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

namespace ITF
{
#define FONT_PAGE_MAX 10
#define FONT_INITIAL_SIZE 150.0f  // size of font, calibrate with 150

    class File;
    class Texture;
    class FontLoader;
    class ITF_Mesh;
    class ResourceID;
    class GFXAdapter;

    enum EControllerType
    {
        CONTROLLER_DEFAULT = 0,
        CONTROLLER_WII,
        CONTROLLER_PS3,
        CONTROLLER_PS5,
        CONTROLLER_VITA,
        CONTROLLER_CTR,
        CONTROLLER_SWITCH,
        CONTROLLER_OUNCE,
        CONTROLLER_XBOX,
        CONTROLLER_X360,
        CONTROLLER_KEYBOARD,
        ENUM_FORCE_SIZE_32(EControllerType)
    };

    struct TAGicon
    {
        TAGicon()
            : m_pos(Vec2d::Zero)
            , m_isButton(bfalse)
            , m_index(0)
            , m_isSkipIcon(bfalse)
            , m_isMenuLogo(bfalse)
            , m_controllerType(CONTROLLER_DEFAULT)
        {}

        Vec2d m_pos;
        bbool m_isButton; // if false, GPE icon
        i32 m_index;
        bbool m_isSkipIcon;
        bbool m_isMenuLogo;
        EControllerType m_controllerType;
    };

    struct SCharDescr
    {
        SCharDescr() : srcX(0), srcY(0), srcW(0), srcH(0), xOff(0), yOff(0), xAdv(0), page(0), chnl(0) {}

        short srcX;
        short srcY;
        short srcW;
        short srcH;
        short xOff;
        short yOff;
        short xAdv;
        short page;
        u32 chnl;

        SafeArray<i32> kerningPairs;
    };

    enum EFontTextEncoding
    {
        NONE,
        UTF8,
        UTF16,
        ENUM_FORCE_SIZE_32(EFontTextEncoding)
    };

#define FONTCACHE_TIMELIFE 10

    class FontCache
    {
    private:

        i32		m_count;
        u32		m_color;
        u32     m_crcColor;
        Vec3d	m_position;
        u32     m_mode;
        String  m_text;
        ITF_Mesh *m_pMesh[FONT_PAGE_MAX];
        u32     m_countVertices[FONT_PAGE_MAX];
        u32     m_timeLife;

    public:
        FontCache(ITF_Mesh *_pMesh[FONT_PAGE_MAX],i32 _count,u32 _crcColor,u32 _color,const Vec3d& _position, u32 _mode, const String& _text,u32 _countVertices[FONT_PAGE_MAX]);
        virtual ~FontCache();

        const u32 getVerticesCount(u32 _page) {return m_countVertices[_page];}
        ITF_Mesh* getMesh(u32 _page) {return m_pMesh[_page];}
        bbool match(bbool _2d, i32 _count,u32 _crcColor,u32 _color,const Vec3d&	_position,const String& _text, u32 _mode = 0);
        void  usage()   {m_timeLife = FONTCACHE_TIMELIFE;}
        bbool unUsed()  {m_timeLife--;return (m_timeLife==0);}

    };

    class Font
    {
    public:

        struct CharInfo
        {
            CharInfo():m_charColor(0),m_charSize(.0f) {}
            u32 m_charColor;
            f32 m_charSize;
        };

        Font();
        ~Font();
        void getFontPathBaseLanguage(const char* fontFile, ITF_LANGUAGE _lanugage, String8& fontFileLocal);

        i32 init(const char *fontFile);
        void manageFontCache();

        void setTextEncoding(EFontTextEncoding _encoding) {encoding = _encoding;}
        f32 getTextWidth(const String& text, int count);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Determine the width of a text that uses different sizes
        /// @param text text
        /// @param count nb of characters of the text to calculate
        /// @param _charInfoTab tab that contain the size of the different characters
        /// @param _tabStart index where to start with _charInfoTab
        f32 getTextWidthMS(const String& text, int count, const SafeArray<CharInfo> &_charInfoTab, u32 _tabStart);

        f32 write(u32 color,f32 x, f32 y, f32 z, const String& text, i32 count, u32 mode, bbool _write );
        f32 writeML(u32 color,f32 x, f32 y, f32 z, const String& text, i32 count, u32 mode, bbool _write );
        void writeBox(u32 color,f32 x, f32 y, f32 z, bbool _isRender2D, const Vec3d &_boxPos, const Vec3d &_boxPivot, const Vec3d &_boxScale, f32 _boxRotation, bbool _useTransBox, f32 width, const SafeArray<CharInfo> &_charInfoTab, const String& text, i32 count, u32 mode, u32 modeY, f32 height = -1.f, bbool _write = btrue, bbool _cut = bfalse, TextActorInfoList* _actorInfo = NULL );

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Write a text in a box and cut the text that doesn't fit in
        /// @param x X-coord of the first line of the text
        /// @param y Y-coord of the first line of the text
        /// @param z Z-coord of the first line of the text
        /// @param width width of the box
        /// @param height height of the box
        /// @param count number of characters to write (0: write all the characters)
        /// @param mode mode of the text (0:left, 1:center, 2:right, 3:justify)
        /// @param textTab text to write
        /// @param scale true if the text must be scale to fit with the box
        void writeBoxWithStyle(f32 x, f32 y, f32 z, bbool _isRender2D, const Vec3d &_boxPos, const Vec3d &_boxPivot, const Vec3d &_boxScale,  f32 _boxRotation, bbool _useTransBox, f32 width, f32 height, const Vec2d& _shadowOffset, u32 _shadowColor, i32 count, u32 mode, u32 modeY, const ITF_VECTOR<TextInfo> &textTab, bbool isUseScale = bfalse, u32 _textTabStart = 0, u32 _textTabStop = U32_INVALID, bbool _write = btrue );

		Vec3d m_boxPos;
		Vec3d m_boxPivot;
		f32	m_boxRotation;
		Vec3d m_boxScale;
		bbool m_boxTransform;
        bbool m_autoScale;
        bbool m_is2d;
        f32 m_lineSpacingFactor;

        Vec2d m_shadowOffset;
        Color m_shadowcolor;

        bbool texturePageIsPhysicalReady() const;

        void setHeight(f32 h)	{scale = h / f32(fontHeight);}

        f32 getHeight()			{return scale * f32(fontHeight);}

        f32 getBottomOffset()	{return scale * (base - fontHeight);}

        f32 getTopOffset()		{return scale * (base - 0);}

        void cleanFontCache();

        f32 getScreenRatio()
        {
            return GFX_ADAPTER->getScreenWidth() / 1280.0f;
        }

        void prepareEffect();

#ifndef ITF_FINAL
        //void drawDebug();
#endif // ITF_FINAL

        const f32 getOutlineThickness() const {return outlineThickness;}

#if defined(ITF_SUPPORT_EDITOR)
        static void dependenciesFile(const String& _filename,DepCollection& _collection);
#endif //defined (ITF_WINDOWS) && !defined (ITF_CONSOLE)


    protected:
        friend class FontLoader;


        void internalWrite(unsigned int colorOriginal,f32 x, f32 y, f32 z, bbool _isRender2D, const String& text, i32 count, u32 _mode, f32 spacing = 0, const SafeArray<CharInfo> *_charInfoTab = NULL, i32 startTabColor = 0);
        f32 adjustForKerningPairs(i32 first, i32 second);
        f32 adjustForKerningPairsDescr(const SCharDescr *ch, i32 second);
        SCharDescr *getChar(i32 id);

        i32 getTextLength(const String& text);
        i32 getTextChar(const String& text, i32 pos, i32 *nextPos = 0);
        i32 findTextChar(const String& text, i32 start, i32 length, i32 ch);
        void getNextChar(const String& text, i32 & pos);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Store the style of the characters of the text in m_textWithStyleTab
        /// Each element of m_textWithStyleTab corresponds to a block of text (m_text)
        /// and to the color and size of each character of this text (m_colorTab and m_sizeTab)
        /// @param textTab text
        void getTextWithStyleTab (const ITF_VECTOR<TextInfo> &textTab, u32 _textTabStart, u32 _textTabStop);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Return the highest size used with this text
        /// @param _charInfoTab tab that contain the size of the different characters
        /// @param _tabStart index where to start with _charInfoTab
        /// @param count nb of characters of the text to calculate
        f32 getHighestSize (const SafeArray<CharInfo> &_charInfoTab, u32 _startTabColor, u32 _count);

        i32 fontHeight; // total height of the font
        i32 base;       // y of base line
        i32 scaleW;
        i32 scaleH;
        SCharDescr defChar;
        bbool hasOutline;

        bbool m_textAdjustement;
        f32 m_yNextLine;
        f32 m_totalTextHeight;
        f32 m_totalTextHeightComputed;
        f32 outlineThickness;

        struct TextWithStyle
        {
            String m_text;
            SafeArray<CharInfo> m_charInfoTab;
        };
        ITF_VECTOR<TextWithStyle> m_textWithStyleTab;
		u32 m_textWithStyleTabNb;

        f32 scale; // Current scale
        f32 globalScale;
        EFontTextEncoding encoding;

        SafeArray<FontCache*> m_vFontCache;

        ITF_MAP<i32, SCharDescr*> chars;
        ITF_VECTOR<ResourceID> pages;
        String8 fxFile;
    };
}

#endif // _ITF_FONT_H_
