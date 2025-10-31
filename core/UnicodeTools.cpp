#include "precompiled_core.h"

#include "UnicodeTools.h"

namespace ITF
{

#if !defined(ITF_FINAL)

	// Unit test
	namespace UnitTest::UnicodeTools
	{
		struct UnicodeCharInfo
		{
			UnicodeChar unicode;
			char nearestAscii;
			ux nbUtf8;
			u8 utf8[4];
			ux nbUtf16;
			UTF16Char utf16[4];
		};

		#define ITF_UNICODE_TEST(c) ITF_AUTO_TEST(c)

		enum SampleUnicodeChars
		{
			// ASCII codes
			SAMPLE_UNICODE_NULL = 0x0000,
			SAMPLE_UNICODE_SPACE = 0x0020,
			SAMPLE_UNICODE_A = 0x0041,
			SAMPLE_UNICODE_TILDE = 0x007E,
			SAMPLE_UNICODE_DEL = 0x007F,

			// UTF8 in 2 bytes
			SAMPLE_UNICODE_PAD = 0x0080,
			SAMPLE_UNICODE_ECUTE = 0x00E9,
			SAMPLE_UNICODE_7FF = 0x07FF,

			// UTF8 in 3 bytes
			SAMPLE_UNICODE_EURO = 0x20AC,
			SAMPLE_UNICODE_CHINESE = 0xF900,
			SAMPLE_UNICODE_FDF0 = 0xFDF0,

			// UTF8 in 4 bytes
			SAMPLE_UNICODE_20000 = 0x20000,
			SAMPLE_UNICODE_2FFFD = 0x2FFFD,
		};

		static UnicodeCharInfo sampleUnicodeChars[] = {
			{   SAMPLE_UNICODE_NULL, 0, 1, { 0x00, 0x00, 0x00, 0x00 }, 0, { 0, 0, 0, 0 }}, // UTF16 part is not filled (todo)
			{  SAMPLE_UNICODE_SPACE, 0, 1, { 0x20, 0x00, 0x00, 0x00 }, 0, { 0, 0, 0, 0 }},
			{      SAMPLE_UNICODE_A, 0, 1, { 0x41, 0x00, 0x00, 0x00 }, 0, { 0, 0, 0, 0 }},
			{  SAMPLE_UNICODE_TILDE, 0, 1, { 0x7E, 0x00, 0x00, 0x00 }, 0, { 0, 0, 0, 0 }},
			{    SAMPLE_UNICODE_DEL, 0, 1, { 0x7F, 0x00, 0x00, 0x00 }, 0, { 0, 0, 0, 0 }},

			{    SAMPLE_UNICODE_PAD, 0, 2, { 0xC2, 0x80, 0x00, 0x00 }, 0, { 0, 0, 0, 0 }},
			{  SAMPLE_UNICODE_ECUTE, 0, 2, { 0xC3, 0xA9, 0x00, 0x00 }, 0, { 0, 0, 0, 0 }},
			{    SAMPLE_UNICODE_7FF, 0, 2, { 0xDF, 0xBF, 0x00, 0x00 }, 0, { 0, 0, 0, 0 }},

			{   SAMPLE_UNICODE_EURO, 0, 3, { 0xE2, 0x82, 0xAC, 0x00 }, 0, { 0, 0, 0, 0 }},
			{SAMPLE_UNICODE_CHINESE, 0, 3, { 0xEF, 0xA4, 0x80, 0x00 }, 0, { 0, 0, 0, 0 }},
			{   SAMPLE_UNICODE_FDF0, 0, 3, { 0xEF, 0xB7, 0xB0, 0x00 }, 0, { 0, 0, 0, 0 }},

			{  SAMPLE_UNICODE_20000, 0, 4, { 0xF0, 0xA0, 0x80, 0x80 }, 0, { 0, 0, 0, 0 }},
			{  SAMPLE_UNICODE_2FFFD, 0, 4, { 0xF0, 0xAF, 0xBF, 0xBD }, 0, { 0, 0, 0, 0 }},
		};

		static const UnicodeCharInfo* findSampleUnicodeCharInfo(SampleUnicodeChars _sampleCode)
		{
			for (ux i = 0; i < sizeof(sampleUnicodeChars) / sizeof(sampleUnicodeChars[0]); ++i)
			{
				UnicodeCharInfo& unicodeCharInfo = sampleUnicodeChars[i];
				if (unicodeCharInfo.unicode == u32(_sampleCode))
					return &unicodeCharInfo;
			}
			return nullptr;
		}

		static void buildSampleUTF8Char(SampleUnicodeChars _sampleCode, ux& _nbByte, char _utf8[4])
		{
			const UnicodeCharInfo* unicodeCharInfo = findSampleUnicodeCharInfo(_sampleCode);
			if (unicodeCharInfo)
			{
				_nbByte = unicodeCharInfo->nbUtf8;
				u8* byteRes = reinterpret_cast<u8*>(_utf8);
				for (ux i = 0; i < _nbByte; ++i)
				{
					byteRes[i] = unicodeCharInfo->utf8[i];
				}
			}
			else
			{
				_nbByte = 0;
			}
		}

		template<ux nbUnicodeChar>
		void buildSampleUTF8(const SampleUnicodeChars (&_sampleCodes)[nbUnicodeChar], String8& _str)
		{
			char utf8Code[5];
			for (ux i = 0; i < nbUnicodeChar; ++i)
			{
				ux _nbByte = 0;
				buildSampleUTF8Char(_sampleCodes[i], _nbByte, utf8Code);
				ITF_UNICODE_TEST(_nbByte != 0);
				ITF_UNICODE_TEST(_nbByte <= 4);
				utf8Code[_nbByte] = 0;
				_str += utf8Code;
			}
		}

		template<ux nbUnicodeChar>
		void checkUnicodeExtract(const SampleUnicodeChars (&_sampleCodes)[nbUnicodeChar], const char* _szUtf8Str)
		{
			// check advance (ie. nb char)
			ITF_UNICODE_TEST(nbUnicodeChar == getNbUnicodeChar(_szUtf8Str));

			// check decoder
			u32 unicodeChar = 0;
			for (ux i = 0; i < nbUnicodeChar; ++i)
			{
				ITF_UNICODE_TEST(*_szUtf8Str != 0); // unexpected end of string
				if (*_szUtf8Str == 0)
					break;
				unicodeChar = getNextUnicodeChar(_szUtf8Str);
				ITF_UNICODE_TEST(unicodeChar == u32(_sampleCodes[i]));
			}
			ITF_UNICODE_TEST(*_szUtf8Str == 0);
		}

		void testSpecial();
		void testUTF16();

		void runTest()
		{
			SampleUnicodeChars str1Codes[] = {
				SAMPLE_UNICODE_SPACE, SAMPLE_UNICODE_EURO, SAMPLE_UNICODE_DEL, SAMPLE_UNICODE_20000, SAMPLE_UNICODE_A, SAMPLE_UNICODE_ECUTE,
			};
			String8 str1; // general purpose UTF8 string
			buildSampleUTF8(str1Codes, str1);
			checkUnicodeExtract(
				str1Codes,
				str1.cStr()); // check coherency between hardcoded values and extracted ones (testing string traversal and unicode extraction)

			SampleUnicodeChars str2Codes[] = {
				SAMPLE_UNICODE_2FFFD, SAMPLE_UNICODE_TILDE, SAMPLE_UNICODE_CHINESE, SAMPLE_UNICODE_PAD,
				SAMPLE_UNICODE_A,     SAMPLE_UNICODE_7FF,   SAMPLE_UNICODE_FDF0,
			};
			String8 str2; // general purpose UTF8 string
			buildSampleUTF8(str2Codes, str2);
			checkUnicodeExtract(
				str2Codes,
				str2.cStr()); // check coherency between hardcoded values and extracted ones (testing string traversal and unicode extraction)

			String8 strPureAscii = "Test\nAscii"; // containing only ascii chars

			const char* szStrAscii = strPureAscii.cStr();
			u32 unicodeChar = 0;
			do
			{
				char curChar = *szStrAscii;
				unicodeChar = getNextUnicodeChar(szStrAscii);
				ITF_UNICODE_TEST(unicodeChar == u32(curChar));
			} while (unicodeChar != 0);

			// string crafted with special cases
			testSpecial();

			testUTF16();
		}

		void testUTF16()
		{
			static const u8 unsignedKosme[] = { 'k',  'o',  's',  'm',  'e', // kosme in ascii
												0xF0, 0xA4, 0xAD, 0xA2, // additionnal chinese character (to test UTF16 surrogate)
												0xCE, 0xBA, 0xE1, 0xBD, 0xB9, 0xCF, 0x83, 0xCE, 0xBC, 0xCE, 0xB5, // kosme in greek
												0x00 };

			const char* kosme = reinterpret_cast<const char*>(unsignedKosme);

			ux nbU16Word = getNbUTF16Word(kosme);
			UTF16Char* utf16Kosme = newAlloc(mId_Temporary, UTF16Char[nbU16Word]);
			buildUTF16FromUTF8(kosme, utf16Kosme, nbU16Word);

			ux nbU8 = getNbUTF8Byte(utf16Kosme) + 1u;
			char* utf8Kosme = newAlloc(mId_Temporary, char[nbU8]);

			buildUTF8FromUTF16(utf16Kosme, utf8Kosme, nbU8);

			ITF_UNICODE_TEST(!strcmp(utf8Kosme, kosme));

			delete[] utf8Kosme;
			delete[] utf16Kosme;
		}

		enum SpecialSeq
		{
			SPECIAL_SEQ_BOUND_FIRST_1BYTE,
			SPECIAL_SEQ_BOUND_FIRST_2BYTES,
			SPECIAL_SEQ_BOUND_FIRST_3BYTES,
			SPECIAL_SEQ_BOUND_FIRST_4BYTES,
			SPECIAL_SEQ_BOUND_FIRST_5BYTES,
			SPECIAL_SEQ_BOUND_FIRST_6BYTES,

			SPECIAL_SEQ_BOUND_LAST_1BYTE,
			SPECIAL_SEQ_BOUND_LAST_2BYTES,
			SPECIAL_SEQ_BOUND_LAST_3BYTES,
			SPECIAL_SEQ_BOUND_LAST_4BYTES,
			SPECIAL_SEQ_BOUND_LAST_5BYTES,
			SPECIAL_SEQ_BOUND_LAST_6BYTES,

			SPECIAL_SEQ_BOUND_OTHER_D7FF,
			SPECIAL_SEQ_BOUND_OTHER_E000,
			SPECIAL_SEQ_BOUND_OTHER_FFFD,
			SPECIAL_SEQ_BOUND_OTHER_10FFFF,
			SPECIAL_SEQ_BOUND_OTHER_110000,

			SPECIAL_SEQ_CONT_FIRST,
			SPECIAL_SEQ_CONT_LAST,

			SPECIAL_SEQ_CONT_2BYTES,
			SPECIAL_SEQ_CONT_3BYTES,
			SPECIAL_SEQ_CONT_4BYTES,
			SPECIAL_SEQ_CONT_5BYTES,
			SPECIAL_SEQ_CONT_6BYTES,

			SPECIAL_SEQ_IMPOSSIBLE_FE,
			SPECIAL_SEQ_IMPOSSIBLE_FF,
			SPECIAL_SEQ_IMPOSSIBLE_FEFF,

			SPECIAL_SEQ_OVERLONG_2,
			SPECIAL_SEQ_OVERLONG_3,
			SPECIAL_SEQ_OVERLONG_4,
			SPECIAL_SEQ_OVERLONG_5,
			SPECIAL_SEQ_OVERLONG_6,

			SPECIAL_SEQ_COUNT,
		};

		struct SpecialSeqInfo
		{
			SpecialSeq seqID;
			u8 utf8[7];
			UnicodeChar unicodeSeqExpected[7];
		};

		static SpecialSeqInfo sampleSpecialSeq[] = {
			{ SPECIAL_SEQ_BOUND_FIRST_1BYTE,{ 0x00 },																			 { 0x0000 }																				  },
			{SPECIAL_SEQ_BOUND_FIRST_2BYTES,             { 0xC2, 0x80, 0x00 },																									   { 0x0080, 0x0000 }},
			{SPECIAL_SEQ_BOUND_FIRST_3BYTES,       { 0xE0, 0xA0, 0x80, 0x00 },																									   { 0x0800, 0x0000 }},
			{SPECIAL_SEQ_BOUND_FIRST_4BYTES, { 0xF0, 0x90, 0x80, 0x80, 0x00 },																								   { 0x00010000, 0x0000 }},
			{SPECIAL_SEQ_BOUND_FIRST_5BYTES,
                { 0xF8, 0x88, 0x80, 0x80, 0x80, 0x00 },
                { UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                0x0000 }																																												  },
			{SPECIAL_SEQ_BOUND_FIRST_6BYTES,
                { 0xFC, 0x84, 0x80, 0x80, 0x80, 0x80, 0x00 },
                { UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                0x0000 }																																												  },

			{  SPECIAL_SEQ_BOUND_LAST_1BYTE,                   { 0x7F, 0x00 },																									   { 0x007F, 0x0000 }},
			{ SPECIAL_SEQ_BOUND_LAST_2BYTES,             { 0xDF, 0xBF, 0x00 },																									   { 0x07FF, 0x0000 }},
			{ SPECIAL_SEQ_BOUND_LAST_3BYTES,       { 0xEF, 0xBF, 0xBF, 0x00 },																									   { 0xFFFF, 0x0000 }},
			{ SPECIAL_SEQ_BOUND_LAST_4BYTES,
                { 0xF7, 0xBF, 0xBF, 0xBF, 0x00 },
                { UNICODE_TOOLS_REPLACEMENT_CHAR, UNICODE_TOOLS_REPLACEMENT_CHAR, UNICODE_TOOLS_REPLACEMENT_CHAR, UNICODE_TOOLS_REPLACEMENT_CHAR, 0x0000 }                                                },
			{ SPECIAL_SEQ_BOUND_LAST_5BYTES,
                { 0xFB, 0xBF, 0xBF, 0xBF, 0xBF, 0x00 },
                { UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                0x0000 }																																												  },
			{ SPECIAL_SEQ_BOUND_LAST_6BYTES,
                { 0xFD, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0x00 },
                { UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                0x0000 }																																												  },

			{  SPECIAL_SEQ_BOUND_OTHER_D7FF,       { 0xED, 0x9F, 0xBF, 0x00 },																									   { 0xD7FF, 0x0000 }},
			{  SPECIAL_SEQ_BOUND_OTHER_E000,       { 0xEE, 0x80, 0x80, 0x00 },																									   { 0xE000, 0x0000 }},
			{  SPECIAL_SEQ_BOUND_OTHER_FFFD,       { 0xEF, 0xBF, 0xBD, 0x00 },																									   { 0xFFFD, 0x0000 }},
			{SPECIAL_SEQ_BOUND_OTHER_10FFFF, { 0xF4, 0x8F, 0xBF, 0xBF, 0x00 },																									 { 0x10FFFF, 0x0000 }},
			{SPECIAL_SEQ_BOUND_OTHER_110000,
                { 0xF4, 0x90, 0x80, 0x80, 0x00 },
                { UNICODE_TOOLS_REPLACEMENT_CHAR, UNICODE_TOOLS_REPLACEMENT_CHAR, UNICODE_TOOLS_REPLACEMENT_CHAR, UNICODE_TOOLS_REPLACEMENT_CHAR, 0x0000 }                                                },

			{        SPECIAL_SEQ_CONT_FIRST,                   { 0x80, 0x00 },																			   { UNICODE_TOOLS_REPLACEMENT_CHAR, 0x0000 }},
			{         SPECIAL_SEQ_CONT_LAST,                   { 0xBF, 0x00 },																			   { UNICODE_TOOLS_REPLACEMENT_CHAR, 0x0000 }},

			{       SPECIAL_SEQ_CONT_2BYTES,             { 0x80, 0xBF, 0x00 },                                               { UNICODE_TOOLS_REPLACEMENT_CHAR, UNICODE_TOOLS_REPLACEMENT_CHAR, 0x0000 }},
			{       SPECIAL_SEQ_CONT_3BYTES,
                { 0x80, 0xBF, 0x80, 0x00 },
                { UNICODE_TOOLS_REPLACEMENT_CHAR, UNICODE_TOOLS_REPLACEMENT_CHAR, UNICODE_TOOLS_REPLACEMENT_CHAR, 0x0000 }                                                                                },
			{       SPECIAL_SEQ_CONT_4BYTES,
                { 0x80, 0xBF, 0x80, 0xBF, 0x00 },
                { UNICODE_TOOLS_REPLACEMENT_CHAR, UNICODE_TOOLS_REPLACEMENT_CHAR, UNICODE_TOOLS_REPLACEMENT_CHAR, UNICODE_TOOLS_REPLACEMENT_CHAR, 0x0000 }                                                },
			{       SPECIAL_SEQ_CONT_5BYTES,
                { 0x80, 0xBF, 0x80, 0xBF, 0x80, 0x00 },
                { UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                0x0000 }																																												  },
			{       SPECIAL_SEQ_CONT_6BYTES,
                { 0x80, 0xBF, 0x80, 0xBF, 0x80, 0xBF, 0x00 },
                { UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                0x0000 }																																												  },

			{     SPECIAL_SEQ_IMPOSSIBLE_FE,                   { 0xFE, 0x00 },																			   { UNICODE_TOOLS_REPLACEMENT_CHAR, 0x0000 }},
			{     SPECIAL_SEQ_IMPOSSIBLE_FF,                   { 0xFF, 0x00 },																			   { UNICODE_TOOLS_REPLACEMENT_CHAR, 0x0000 }},
			{   SPECIAL_SEQ_IMPOSSIBLE_FEFF,
                { 0xFE, 0xFE, 0xFF, 0xFF, 0x00 },
                { UNICODE_TOOLS_REPLACEMENT_CHAR, UNICODE_TOOLS_REPLACEMENT_CHAR, UNICODE_TOOLS_REPLACEMENT_CHAR, UNICODE_TOOLS_REPLACEMENT_CHAR, 0x0000 }                                                },

			{        SPECIAL_SEQ_OVERLONG_2,             { 0xC0, 0xAF, 0x00 },                                               { UNICODE_TOOLS_REPLACEMENT_CHAR, UNICODE_TOOLS_REPLACEMENT_CHAR, 0x0000 }},
			{        SPECIAL_SEQ_OVERLONG_3,
                { 0xE0, 0x80, 0xAF, 0x00 },
                { UNICODE_TOOLS_REPLACEMENT_CHAR, UNICODE_TOOLS_REPLACEMENT_CHAR, UNICODE_TOOLS_REPLACEMENT_CHAR, 0x0000 }                                                                                },
			{        SPECIAL_SEQ_OVERLONG_4,
                { 0xF0, 0x80, 0x80, 0xAF, 0x00 },
                { UNICODE_TOOLS_REPLACEMENT_CHAR, UNICODE_TOOLS_REPLACEMENT_CHAR, UNICODE_TOOLS_REPLACEMENT_CHAR, UNICODE_TOOLS_REPLACEMENT_CHAR, 0x0000 }                                                },
			{        SPECIAL_SEQ_OVERLONG_5,
                { 0xF8, 0x80, 0x80, 0x80, 0xAF, 0x00 },
                { UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                0x0000 }																																												  },
			{        SPECIAL_SEQ_OVERLONG_6,
                { 0xFC, 0x80, 0x80, 0x80, 0x80, 0xAF, 0x00 },
                { UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                UNICODE_TOOLS_REPLACEMENT_CHAR,
                0x0000 }																																												  },
		};

		static const SpecialSeqInfo* findSampleSpecialSeq(SpecialSeq _sampleCode)
		{
			return &sampleSpecialSeq[ux(_sampleCode)];
		}

		void testSpecial()
		{
			// cf http://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt for invalid sequence sample

			for (ux i = 0; i < SPECIAL_SEQ_COUNT; ++i)
			{
				const SpecialSeqInfo* specialSeq = findSampleSpecialSeq(SpecialSeq(i));

				ITF::Vector<UnicodeChar> result;

				const char* currentUTF8 = (const char*)specialSeq->utf8;

				while (*currentUTF8)
				{
					UnicodeChar uniChar = getNextUnicodeChar(currentUTF8);
					result.push_back(uniChar);
				};

				const UnicodeChar* currentUniChar = specialSeq->unicodeSeqExpected;
				while (*currentUniChar)
				{
					ITF_UNICODE_TEST(!result.empty());
					if (result.empty())
					{
						break;
					}
					ITF_UNICODE_TEST(*currentUniChar == result.front());
					result.erase(result.begin());
					currentUniChar++;
				};
				ITF_UNICODE_TEST(result.empty());
			}
		}

#undef ITF_UNICODE_TEST
	} // namespace InitTest::UnicodeTools

#endif

	// Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
	// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

#define UTF8_ACCEPT 0
#define UTF8_REJECT 12

	static const u8 utf8d[] = {
		// The first part of the table maps bytes to character classes that
		// to reduce the size of the transition table and create bitmasks.
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		9,
		9,
		9,
		9,
		9,
		9,
		9,
		9,
		9,
		9,
		9,
		9,
		9,
		9,
		9,
		9,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		7,
		8,
		8,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		10,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		4,
		3,
		3,
		11,
		6,
		6,
		6,
		5,
		8,
		8,
		8,
		8,
		8,
		8,
		8,
		8,
		8,
		8,
		8,

		// The second part is a transition table that maps a combination
		// of a state of the automaton and a character class to a state.
		0,
		12,
		24,
		36,
		60,
		96,
		84,
		12,
		12,
		12,
		48,
		72,
		12,
		12,
		12,
		12,
		12,
		12,
		12,
		12,
		12,
		12,
		12,
		12,
		12,
		0,
		12,
		12,
		12,
		12,
		12,
		0,
		12,
		0,
		12,
		12,
		12,
		24,
		12,
		12,
		12,
		12,
		12,
		24,
		12,
		24,
		12,
		12,
		12,
		12,
		12,
		12,
		12,
		12,
		12,
		24,
		12,
		12,
		12,
		12,
		12,
		24,
		12,
		12,
		12,
		12,
		12,
		12,
		12,
		24,
		12,
		12,
		12,
		12,
		12,
		12,
		12,
		12,
		12,
		36,
		12,
		36,
		12,
		12,
		12,
		36,
		12,
		12,
		12,
		12,
		12,
		36,
		12,
		36,
		12,
		12,
		12,
		36,
		12,
		12,
		12,
		12,
		12,
		12,
		12,
		12,
		12,
		12,
	};

	u32 utf8Decode(u32& _state, u32& _codep, char _srcByte)
	{
		u32 srcU32Byte = u32(u8(_srcByte));
		u32 charType = utf8d[srcU32Byte];

		_codep = (_state != UTF8_ACCEPT) ? ((_srcByte & 0x3Fu) | (_codep << 6u)) : ((0xFFu >> charType) & (srcU32Byte));

		_state = utf8d[256u + _state + charType];

		return _state;
	}

    UnicodeChar getNextUnicodeChar(const char*& _src)
    {
        if (*_src == 0)
            return 0u;

        u32 curState = UTF8_ACCEPT;
        u32 prevState = UTF8_ACCEPT;
        UnicodeChar uniChar = 0;

        do
        {
            utf8Decode(curState, uniChar, *_src);
            if (curState == UTF8_REJECT)
            {
                // the byte is invalid, get back to the first byte of the sequence
                curState = UTF8_ACCEPT;
                uniChar = UNICODE_TOOLS_REPLACEMENT_CHAR;  // replacement character
                if (prevState != UTF8_ACCEPT)              // if the previous character was an intermediate one, count it, and retry decoding of this one
                    --_src;
            }
            ++_src;
            prevState = curState;
        } while (curState != UTF8_ACCEPT);

        return uniChar;
    }

    ux getNbUnicodeChar(const char* _src)
    {
        ux count = 0;
        while (getNextUnicodeChar(_src) != 0u)
        {
            ++count;
        }

        return count;
    }

    ux getUTF8FromUnicodeChar(UnicodeChar _uniChar, char _utf8[4])  // return the number of bytes written
    {
        if (!isUnicodeValid(_uniChar))
            return getUTF8FromUnicodeChar(UNICODE_TOOLS_REPLACEMENT_CHAR, _utf8);

        if (_uniChar < 0x80u)
        {
            _utf8[0] = char(u8(_uniChar));
            return 1u;
        }
        else if (_uniChar < 0x800u)
        {
            _utf8[0] = char(u8((_uniChar >> 6u) | 0xC0u));
            _utf8[1] = char(u8((_uniChar & 0x3Fu) | 0x80u));
            return 2u;
        }
        else if (_uniChar < 0x10000u)
        {
            _utf8[0] = char(u8((_uniChar >> 12u) | 0xE0u));
            _utf8[1] = char(u8(((_uniChar >> 6u) & 0x3Fu) | 0x80u));
            _utf8[2] = char(u8((_uniChar & 0x3Fu) | 0x80u));
            return 3u;
        }
        else
        {
            _utf8[0] = char(u8((_uniChar >> 18u) | 0xF0u));
            _utf8[1] = char(u8(((_uniChar >> 12u) & 0x3Fu) | 0x80u));
            _utf8[2] = char(u8(((_uniChar >> 6u) & 0x3Fu) | 0x80u));
            _utf8[3] = char(u8((_uniChar & 0x3Fu) | 0x80u));
            return 4u;
        }
    }

    ux getNbUnicodeChar(const UTF16Char* _src)
    {
        ux count = 0;
        while (*_src != 0)
        {
            u32 codePoint = u32(*_src);
            if (codePoint >= UNICODE_TOOLS_LEAD_SURROGATE_MIN && codePoint <= UNICODE_TOOLS_LEAD_SURROGATE_MAX)
            {
                _src++;
                if (*_src != 0)
                    _src++;
            }
            else
            {
                _src++;
            }
            count++;
        }
        return count;
    }

    UnicodeChar getNextUnicodeChar(const UTF16Char*& _src)
    {
        if (*_src == 0)
            return 0;

        u32 codePoint = u32(*_src);
        _src++;

        if (codePoint >= UNICODE_TOOLS_LEAD_SURROGATE_MIN && codePoint <= UNICODE_TOOLS_LEAD_SURROGATE_MAX)
        {
            if (*_src == 0)  // error, return unknown
                return UNICODE_TOOLS_REPLACEMENT_CHAR;
            ux surrogate = (*_src);
            if (surrogate >= UNICODE_TOOLS_TRAIL_SURROGATE_MIN && surrogate <= UNICODE_TOOLS_TRAIL_SURROGATE_MAX)
            {
                _src++;
                codePoint = (codePoint << 10u) + surrogate + UNICODE_TOOLS_SURROGATE_OFFSET;
            }
            else
            {
                // return error and keep the current character
                return UNICODE_TOOLS_REPLACEMENT_CHAR;
            }
        }
        return UnicodeChar(codePoint);
    }

    bbool getUTF16FromUnicodeChar(UnicodeChar _uniChar,
        UTF16Char& _primary,
        UTF16Char& _secondary)  // return btrue if 2 UTF16Char are needed (is a surrogate)
    {
        if (_uniChar <= 0xFFFFu)
        {
            _primary = UTF16Char(_uniChar);
            _secondary = 0u;
            return false;
        }
        else
        {
            _primary = UTF16Char(UNICODE_TOOLS_LEAD_OFFSET + (_uniChar >> 10u));
            _secondary = UTF16Char(UNICODE_TOOLS_TRAIL_SURROGATE_MIN + (_uniChar & 0x3FFu));
            return true;
        }
    }

    ux getNbUTF16Word(const char* _src)  // including null character
    {
        ux count = 0;

        while (*_src != 0)
        {
            UnicodeChar uniChar = getNextUnicodeChar(_src);
            ITF_ASSERT(uniChar != 0);
            if (uniChar <= 0xFFFFu)
                ++count;
            else
                count += 2u;  // surrogate -> 2 wide chars
        }

        return count + 1u;  // add null character
    }

    void buildUTF16FromUTF8(const char* _src, UTF16Char* _u16Buffer, ux _nbMaxU16)  // write up to _nbMaxCharacters (including 0 terminal) in _u16Buffer
    {
        for (;;)
        {
            if (_nbMaxU16 == 0)
                return;
            if (_nbMaxU16 == 1u)
            {
                // mark 0 terminal and quit
                *_u16Buffer = 0;
                _u16Buffer++;
                return;
            }

            UnicodeChar uniChar = getNextUnicodeChar(_src);
            if (uniChar == 0)
            {
                *_u16Buffer = 0;
                _u16Buffer++;
                return;
            }

            UTF16Char primary = 0, surrogate = 0;
            if (getUTF16FromUnicodeChar(uniChar, primary, surrogate))
            {
                // more than 2 char must left, end of show, close it and left
                if (_nbMaxU16 == 2u)
                {
                    *_u16Buffer = 0;
                    return;
                }
                else  // surrogate case
                {
                    *_u16Buffer = primary;
                    _u16Buffer++;
                    *_u16Buffer = surrogate;
                    _u16Buffer++;
                    _nbMaxU16 -= 2u;
                }
            }
            else
            {
                *_u16Buffer = primary;
                _u16Buffer++;
                _nbMaxU16--;
            }
        }
    }

    ux getNbUTF8Byte(const UTF16Char* _src)
    {
        ux count = 0;

        while (*_src != 0)
        {
            UnicodeChar uniChar = getNextUnicodeChar(_src);
            ITF_ASSERT(uniChar != 0);
            char dummyUtf8[4];
            count += getUTF8FromUnicodeChar(uniChar, dummyUtf8);
        }

        return count + 1u;
    }

    ux getNbUTF8Byte(const UTF32Char* _src)
    {
        ux count = 0;

        while (*_src != 0)
        {
            UnicodeChar uniChar = *_src;
            _src++;
            char dummyUtf8[4];
            count += getUTF8FromUnicodeChar(uniChar, dummyUtf8);
        }

        return count + 1u; // add 1 for 0 terminal
    }

    void buildUTF8FromUTF16(const UTF16Char* _src, char* _dest, ux _nbMaxByte)  // write up to _nbMaxBytes (including 0 terminal) in _u16Buffer
    {
        for (;;)
        {
            if (_nbMaxByte == 0)
                return;
            if (_nbMaxByte == 1u)
            {
                // mark 0 terminal and quit
                *_dest = 0;
                _dest++;
                return;
            }

            UnicodeChar uniChar = getNextUnicodeChar(_src);
            if (uniChar == 0)
            {
                *_dest = 0;
                _dest++;
                return;
            }

            char utf8Code[4];
            ux nbByte = getUTF8FromUnicodeChar(uniChar, utf8Code);
            if (nbByte >= _nbMaxByte)
            {  // full, close it and left
                _dest = 0;
                return;
            }
            else
            {
                // std::copy(utf8Code, utf8Code+nbByte, _dest);
                for (ux i = 0; i < nbByte; ++i)
                {
                    _dest[i] = utf8Code[i];
                }
                _dest += nbByte;
                _nbMaxByte -= nbByte;
            }
        }
    }

    void buildUTF32FromUTF8(const char* _src, UTF32Char* _buffer, ux _nbMaxCodePoint)  // write up to _nbMaxCodePoint (including 0 terminal) in _buffer
    {
        for (;;)
        {
            if (_nbMaxCodePoint == 0)
                return;

            if (_nbMaxCodePoint == 1u)
            {
                // mark 0 terminal and quit
                *_buffer = 0;
                _buffer++;
                return;
            }

            UnicodeChar uniChar = getNextUnicodeChar(_src);
            *_buffer = static_cast<wchar_t>(uniChar);
            _buffer++;
            if (uniChar == 0)
            {
                return;
            }
            _nbMaxCodePoint--;
        }
    }

    void buildUTF32FromUTF16(const UTF16Char* _src, UTF32Char* _buffer, ux _nbMaxCodePoint)  // write up to _nbMaxWChar (including 0 terminal) in _buffer
    {
        for (;;)
        {
            if (_nbMaxCodePoint == 0)
                return;

            if (_nbMaxCodePoint == 1u)
            {
                // mark 0 terminal and quit
                *_buffer = 0;
                _buffer++;
                return;
            }

            UnicodeChar uniChar = getNextUnicodeChar(_src);
            *_buffer = static_cast<wchar_t>(uniChar);
            _buffer++;
            if (uniChar == 0)
            {
                return;
            }
            _nbMaxCodePoint--;
        }
    }

    void buildUTF8FromUTF32(const UTF32Char* _src, char* _buffer, ux _nbMaxChar)  // write up to _nbMaxChar (including 0 terminal) in _buffer
    {
        for (;;)
        {
            if (_nbMaxChar == 0)
                return;

            if (_nbMaxChar == 1u)
            {
                // mark 0 terminal and quit
                *_buffer = 0;
                return;
            }

            UnicodeChar uniChar = *_src;
            _src++;

            if (uniChar == 0)
            {
                *_buffer = 0;
                return;
            }

            if (!isUnicodeValid(uniChar))
            {
                uniChar = UNICODE_TOOLS_REPLACEMENT_CHAR;
            }

            char utf8[4];
            ux nbChar = getUTF8FromUnicodeChar(uniChar, utf8);

            if (nbChar >= _nbMaxChar) // crop the character if it is too big. Also assume we handle the 0 terminal placement
            {
                _buffer = 0;
                return;
            }

            std::copy(utf8, utf8 + nbChar, _buffer);
            _buffer += nbChar;
            _nbMaxChar -= nbChar;
        }
    }

    void buildUTF16FromUTF32(const UTF32Char* _src, UTF16Char* _u16Buffer, ux _nbMaxU16)  // write up to _nbMaxChar (including 0 terminal) in _buffer
    {
        for (;;)
        {
            if (_nbMaxU16 == 0)
                return;

            if (_nbMaxU16 == 1u)
            {
                // mark 0 terminal and quit
                *_u16Buffer = 0;
                _u16Buffer++;
                return;
            }

            UnicodeChar uniChar = *_src;
            _src++;

            if (uniChar == 0)
            {
                *_u16Buffer = 0;
                return;
            }

            if (!isUnicodeValid(uniChar))
            {
                uniChar = UNICODE_TOOLS_REPLACEMENT_CHAR;
            }

            UTF16Char primary = 0, surrogate = 0;
            if (getUTF16FromUnicodeChar(uniChar, primary, surrogate))
            {
                // more than 2 char must left, end of show, close it and left
                if (_nbMaxU16 == 2u)
                {
                    *_u16Buffer = 0;
                    return;
                }
                else  // surrogate case
                {
                    *_u16Buffer = primary;
                    _u16Buffer++;
                    *_u16Buffer = surrogate;
                    _u16Buffer++;
                    _nbMaxU16 -= 2u;
                }
            }
            else
            {
                *_u16Buffer = primary;
                _u16Buffer++;
                _nbMaxU16--;
            }
        }
    }

} // namespace ITF
