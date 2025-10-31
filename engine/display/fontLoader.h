#ifndef _ITF_FONTLOADER_H_
#define _ITF_FONTLOADER_H_

namespace ITF
{

	class File;
	class Font;
    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    class FontLoader
	{
	public:
		FontLoader(File* file, Font *font, const char *fontFile);

		virtual ~FontLoader() = default;

		virtual i32 Load() = 0; // Must be implemented by derived class

	protected:
		void LoadPage(i32 id, const char *pageFile, const char *fontFile);
		void SetFontInfo(i32 outlineThickness);
		void SetCommonInfo(i32 fontHeight, i32 base, i32 scaleW, i32 scaleH, i32 pages, bbool isPacked);
		void AddChar(i32 id, i32 x, i32 y, i32 w, i32 h, i32 xoffset, i32 yoffset, i32 xadvance, i32 page, i32 chnl);
		void AddKerningPair(i32 first, i32 second, i32 amount);

		File* m_File;
		Font *font;
		const char *fontFile;

		i32 outlineThickness;
	};

    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	class FontLoaderTextFormat : public FontLoader
	{
	public:
		FontLoaderTextFormat(File *_file, Font *_font, const char *_fontFile);

		i32 Load();

		i32 SkipWhiteSpace(String8 &str, i32 start);
		i32 FindEndOfToken(String8 &str, i32 start);

		void InterpretInfo(String8 &str, i32 start);
		void InterpretCommon(String8 &str, i32 start);
		void InterpretChar(String8 &str, i32 start);
		void InterpretSpacing(String8 &str, i32 start);
		void InterpretKerning(String8 &str, i32 start);
		void InterpretPage(String8 &str, i32 start, const char *fontFile);
	};

    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	class FontLoaderBinaryFormat : public FontLoader
	{
	public:
		FontLoaderBinaryFormat(File *_file, Font *font, const char *fontFile);

		i32 Load();

		void ReadInfoBlock(i32 size);
		void ReadCommonBlock(i32 size);
		void ReadPagesBlock(i32 size);
		void ReadCharsBlock(i32 size);
		void ReadKerningPairsBlock(i32 size);
	};

    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
}

#endif //_ITF_FONTLOADER_H_