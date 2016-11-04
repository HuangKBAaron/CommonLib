/** \file
\brief Parser of language syntax similar to C/C++
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://asawicki.info/ \n

Part of CommonLib library. \n
Encoding UTF-8, end of line CR+LF \n
License: GNU LGPL. \n
Documentation: \ref Module_Tokenizer \n
Module components: \ref code_tokenizer
*/
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif
#ifndef COMMON_TOKENIZER_H_
#define COMMON_TOKENIZER_H_

#include "Error.hpp"

namespace common
{

class Stream;

/** \addtogroup code_tokenizer Tokenizer Module
Dokumentacja: \ref Module_Tokenizer \n
Nag��wek: Tokenizer.hpp */
//@{

/// Klasa b��du dla tokenizera
class TokenizerError : public Error
{
private:
	size_t m_Char, m_Row, m_Col;
public:
	TokenizerError(size_t Char, size_t Row, size_t Col, const tstring &Msg = _T(""), const tstring &File = _T(""), int a_Line = 0);

	size_t GetCharNum() { return m_Char; }
	size_t GetRowNum() { return m_Row; }
	size_t GetColNum() { return m_Col; }
};

/// \internal
class Tokenizer_pimpl;

/// Tokenizer
class Tokenizer
{
private:
	scoped_ptr<Tokenizer_pimpl> pimpl;

public:
	/// Koniec wiersza '\n' ma by� uznawany za osobnego rodzaju token, nie za zwyczajny bia�y znak
	static const uint FLAG_TOKEN_EOL         = 0x01;
	/// �a�cuch "" mo�e si� rozci�ga� na wiele wierszy zawieraj�c w tre�ci koniec wiersza
	static const uint FLAG_MULTILINE_STRINGS = 0x02;

	/// Rodzaje token�w
	enum TOKEN
	{
		TOKEN_EOF,        ///< Koniec ca�ego dokumentu (dalej nie parsowa�)
		TOKEN_EOL,        ///< Token ko�ca linii (tylko kiedy FLAG_TOKEN_EOL)
		TOKEN_SYMBOL,     ///< Jednoznakowy symbol (np. kropka)
		TOKEN_INTEGER,    ///< Liczba ca�kowita, np. <tt>-123</tt>
		TOKEN_FLOAT,      ///< Liczba zmiennoprzecinkowa, np. <tt>10.5</tt>
		TOKEN_CHAR,       ///< Sta�a znakowa, np. <tt>'A'</tt>
		TOKEN_IDENTIFIER, ///< Identyfikator, np. <tt>abc_123</tt>
		TOKEN_KEYWORD,    ///< S�owo kluczowe, czyli identyfikator pasuj�cy do zarejestrowanych s��w kluczowych
		TOKEN_STRING,     ///< Sta�a �a�cuchowa, np. <tt>"ABC"</tt>
	};

	/// Tworzy z dokumentu podanego przez �a�cuch tchar*
	Tokenizer(const tchar *Input, size_t NumChars, uint Flags);
	/// Tworzy z dokumentu podanego przez tstring
	Tokenizer(const tstring *Input, uint Flags);
	/// Tworzy z dokumentu wczytywanego z dowolnego strumienia
	Tokenizer(Stream *Input, uint Flags);

	~Tokenizer();

	/** \name Konfiguracja */
	//@{
	void RegisterKeyword(uint Id, const tstring &Keyword);
	/// Wczytuje ca�� list� s��w kluczowych w postaci tablicy �a�cuch�w tchar*.
	/** S�owa kluczowe b�d� mia�y kolejne identyfikatory 0, 1, 2 itd. */
	void RegisterKeywords(const tchar **Keywords, size_t KeywordCount);
	//@}

	/** \name Funkcje podstawowe
	Zwracaj� informacje na temat ostatnio odczytanego tokena. */
	//@{

	/// Odczytuje nast�pny token
	void Next();

	/// Zwraca typ tokena
	TOKEN GetToken();

	size_t GetCharNum();
	size_t GetRowNum();
	size_t GetColNum();

	/// Dzia�a zawsze, ale zastosowanie g��wnie dla GetToken() == common::Tokenizer::TOKEN_IDENTIFIER lub common::Tokenizer::TOKEN_STRING
	const tstring & GetString();
	/// Tylko je�li GetToken() == common::Tokenizer::TOKEN_CHAR lub common::Tokenizer::TOKEN_SYMBOL
	tchar GetChar();
	/// Tylko je�li GetToken() == common::Tokenizer::TOKEN_KEYWORD
	uint GetId();
	/// Tylko je�li GetToken() == common::Tokenizer::TOKEN_INTEGER
	bool GetUint1(uint8 *Out);
	bool GetUint2(uint16 *Out);
	bool GetUint4(uint32 *Out);
	bool GetUint8(uint64 *Out);
	bool GetInt1(int8 *Out);
	bool GetInt2(int16 *Out);
	bool GetInt4(int32 *Out);
	bool GetInt8(int64 *Out);
	uint8 MustGetUint1();
	uint16 MustGetUint2();
	uint32 MustGetUint4();
	uint64 MustGetUint8();
	int8 MustGetInt1();
	int16 MustGetInt2();
	int32 MustGetInt4();
	int64 MustGetInt8();
	/** Tylko je�li GetToken() == common::Tokenizer::TOKEN_INTEGER lub common::Tokenizer::TOKEN_FLOAT */
	bool GetFloat(float *Out);
	/** Tylko je�li GetToken() == common::Tokenizer::TOKEN_INTEGER lub common::Tokenizer::TOKEN_FLOAT */
	bool GetDouble(double *Out);
	float MustGetFloat();
	double MustGetDouble();

	//@}

	/// Rzuca wyj�tek z b��dem parsowania. Domy�lny komunikat.
	void CreateError();
	/// Rzuca wyj�tek z b��dem parsowania. W�asny komunikat.
	void CreateError(const tstring &Msg);

	/** \name Zapytania
	Zwracaj� true, je�li ostatnio sparsowany token jest taki jak tu podany. */
	//@{
	bool QueryToken(TOKEN Token);
	bool QueryToken(TOKEN Token1, TOKEN Token2);
	bool QueryEOF();
	bool QueryEOL();
	bool QuerySymbol(tchar Symbol);
	bool QueryIdentifier(const tstring &Identifier);
	bool QueryKeyword(uint KeywordId);
	bool QueryKeyword(const tstring &Keyword);
	//@}

	/** \name Asercje
	Rzucaj� wyj�tek, je�li ostatnio wczytany token nie spe�nia danego warunku. */
	//@{
	/// Rzuca wyj�tek z b��dem parsowania, je�li ostatnio sparsowany token nie jest podanego rodzaju.
	void AssertToken(TOKEN Token);
	/// Rzuca wyj�tek z b��dem parsowania, je�li ostatnio sparsowany token nie jest jednego z dw�ch podanych rodzaj�w.
	void AssertToken(TOKEN Token1, TOKEN Token2);
	void AssertEOF();
	void AssertEOL();
	void AssertSymbol(tchar Symbol);
	void AssertIdentifier(const tstring &Identifier);
	void AssertKeyword(uint KeywordId);
	void AssertKeyword(const tstring &Keyword);
	//@}

	/// Zamienia typ tokena na jego czytelny dla u�ytkownika opis tekstowy, np. "Sta�a znakowa"
	static const tchar * GetTokenName(TOKEN Token);
};

class TokenWriter
{
public:
	static const uint ESCAPE_EOL   = 0x01; ///< Zamienia� znaki ko�ca wiersza na <tt>\\r \\n</tt> na sekwencje ucieczki
	static const uint ESCAPE_OTHER = 0x02; ///< Zamienia� znaki niedost�ne z klawiatury (w tym polskie litery) na <tt>\\xHH</tt>
	/// Escapuje �a�cuch zgodnie ze sk�adni� tokenizera
	/** \param EscapeFlags U�ywa� sta�ych ESCAPE_EOL, ESCAPE_OTHER. */
	static void Escape(tstring *Out, const tstring &In, uint EscapeFlags);

	TokenWriter(tstring *outStr);
	TokenWriter(common::Stream *outStream);

	//====== Configuration
	const tstring & GetEOL() { return m_EOL; }
	const tstring & GetIndent() { return m_Indent; }
	void SetEOL(const tstring &s) { m_EOL = s; }
	void SetIndent(const tstring &s) { m_Indent = s; }
	uint GetEscapeFlags() { return m_EscapeFlags; }
	void SetEscapeFlags(uint v) { m_EscapeFlags = v; }
	void RegisterSymbol(tchar ch, bool spaceBefore, bool spaceAfter, int levelDelta);
	void RegisterSymbol(const tstring &s, bool spaceBefore, bool spaceAfter, int levelDelta);
	void RegisterKeyword(const tstring &s, int levelDelta);
	
	uint GetLevel() { return m_Level; }

	void WriteSpace(tchar ch = _T(' '));
	void WriteSpace(const tstring &s);
	void WriteEOL();

	void WriteKeyword(const tstring &s);
	void WriteIdentifier(const tstring &s);
	void WriteSymbol(tchar ch);
	void WriteSymbol(const tstring &s);
	void WriteString(const tstring &s);
	void WriteUint1(uint8 v, bool hex = false);
	void WriteUint2(uint16 v, bool hex = false);
	void WriteUint4(uint32 v, bool hex = false);
	void WriteUint8(uint64 v, bool hex = false);
	void WriteInt1(int8 v);
	void WriteInt2(int16 v);
	void WriteInt4(int32 v);
	void WriteInt8(int64 v);
	void WriteFloat(float v, char Mode = 'g', int Precision = 6);
	void WriteDouble(double v, char Mode = 'g', int Precision = 6);
	void WriteNumberString(const tstring &v);
	void WriteComment(const tstring &s, bool alwaysMultiline = false);

private:
	tstring *m_OutString;
	common::Stream *m_OutStream;

	tstring m_EOL, m_Indent;
	uint m_EscapeFlags;
	tstring m_Symbols_Chars_SpaceBefore, m_Symbols_Chars_SpaceAfter;
	tstring m_Symbols_Chars_LevelInc, m_Symbols_Chars_LevelDec;
	STRING_VECTOR m_Symbols_Strings_SpaceBefore, m_Symbols_Strings_SpaceAfter;
	STRING_VECTOR m_Symbols_Strings_LevelInc, m_Symbols_Strings_LevelDec;
	STRING_VECTOR m_Keywords_LevelInc, m_Keywords_LevelDec;

	uint m_Level;
	bool m_WasEOL;
	bool m_SpaceRequired;

	void Ctor();
	void WriteIndent(uint level);
	void LevelInc() { m_Level++; }
	void LevelDec() { assert(m_Level > 0); m_Level--; }
	void WriteRaw(tchar ch);
	void WriteRaw(const tchar *s);
	void WriteRaw(const tstring &s);
	int GetKeywordLevelDelta(const tstring &s);
	int GetSymbolLevelDelta(tchar ch);
	int GetSymbolLevelDelta(const tstring &s);
	bool IsSymbolSpaceBefore(tchar ch);
	bool IsSymbolSpaceBefore(const tstring &s);
	bool IsSymbolSpaceAfter(tchar ch);
	bool IsSymbolSpaceAfter(const tstring &s);
	void WriteMultiLineComment(const tstring &s);
	void WriteSingleLineComment(const tstring &s);
};

//@}
// code_tokenizer

} // namespace common

#endif
