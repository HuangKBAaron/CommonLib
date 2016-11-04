/** \file
\brief Auxiliary module for zlib library
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://asawicki.info/ \n

Part of CommonLib library. \n
Encoding UTF-8, end of line CR+LF \n
License: GNU LGPL. \n
Documentation: \ref Module_ZlibUtils\n
Module components: \ref code_zlibutils
*/
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif
#ifndef COMMON_ZLIB_UTILS_H_
#define COMMON_ZLIB_UTILS_H_

#include "Stream.hpp"
#include "Error.hpp"

namespace common
{

/** \addtogroup code_zlibutils ZlibUtils Module
Dokumentacja: \ref Module_ZlibUtils \n
Nag��wek: ZlibUtils.hpp */
//@{

/// Klasa wyj�tku dla b��du zg�oszonego przez bibliotek� zlib
class ZlibError : public Error
{
public:
	ZlibError(int Code, const tstring &Msg = _T(""), const tstring &File = _T(""), int Line = 0);
};

/** \name Poziomy kompresji Zlib. */
//@{
/// Brak kompresji (co nie znaczy �e dane zostan� dos�ownie przepisane)
const int ZLIB_STORE_LEVEL   = 0;
/// Najs�absza kompresja, ale najszybsza
const int ZLIB_FASTEST_LEVEL = 1;
/// Najlepsza jako�� kompresji
const int ZLIB_BEST_LEVEL    = 9;
/// Domy�lny poziom kompresji
extern const int ZLIB_DEFAULT_LEVEL;
//@}

/// \internal
class ZlibCompressionStream_pimpl;
/// \internal
class ZlibDecompressionStream_pimpl;

/// Strumie� do zapisu, kt�ry kompresuje dane w formacie zlib.
class ZlibCompressionStream : public OverlayStream
{
private:
	scoped_ptr<ZlibCompressionStream_pimpl> pimpl;

public:
	ZlibCompressionStream(Stream *a_Stream, int Level = ZLIB_DEFAULT_LEVEL, int WindowBits = 15);
	virtual ~ZlibCompressionStream();

	// ======== Implementacja Stream ========
	
	virtual void Write(const void *Data, size_t Size);
	/** Mo�e spowodowa�, �e skompresowane dane b�d� inaczej wyg�ada�y i obni�y� jako�� kompresji. */
	virtual void Flush();
	
	/// Oblicza maksymalny rozmiar bufora na skompresowane dane
	static size_t CompressLength(size_t DataLength);
	/// Po prostu kompresuje podane dane
	/**
	\return Zwraca d�ugo�� skompresowanych danych.
	\param[out] OutData Musi by� buforem o d�ugo�ci conajmniej takiej, jak obliczona przez CompressLength.
	\param BufLength D�ugo�� zaalokowanej pami�ci na OutData, podawana i sprawdzana na wszelki wypadek.
	*/
	static size_t Compress(void *OutData, size_t BufLength, const void *Data, size_t DataLength, int Level = ZLIB_DEFAULT_LEVEL);
};

/// Strumie� do zapisu, kt�ry kompresuje w formacie gzip do��czaj�c nag��wek gzip z podanymi parametrami.
/** Wszystko jak w klasie ZlibCompressionStream. */
class GzipCompressionStream : public OverlayStream
{
private:
	scoped_ptr<ZlibCompressionStream_pimpl> pimpl;

public:
	/**
	\param FileName Je�li ma nie by�, mo�na poda� NULL.
	\param Comment Je�li ma nie by�, mo�na poda� NULL.
	*/
	GzipCompressionStream(Stream *a_Stream, const string *FileName, const string *Comment, int Level = ZLIB_DEFAULT_LEVEL, int WindowBits = 15);
	virtual ~GzipCompressionStream();

	// ======== Implementacja Stream ========
	
	virtual void Write(const void *Data, size_t Size);
	virtual void Flush();
};

/// Strumie� do odczytu dekompresuj�cy dane w formacie Zlib.
class ZlibDecompressionStream : public OverlayStream
{
private:
	scoped_ptr<ZlibDecompressionStream_pimpl> pimpl;

public:
	ZlibDecompressionStream(Stream *a_Stream, int WindowBits = 15);
	virtual ~ZlibDecompressionStream();

	// ======== Implementacja Stream ========
	
	virtual size_t Read(void *Out, size_t MaxLength);
	virtual bool End();

	/// Po prostu dekompresuje podane dane
	/**
	Rozmiar danych po dekompresji musi by� gdzie� zapami�tany i odpowiednio du�y bufor musi by� ju� zaalkowany.
	\return Zwraca d�ugo�� rozkompresowanych danych.
	\param BufLength D�ugo�� zaalokowanej pami�ci na OutData, podawana i sprawdzana na wszelki wypadek.
	*/
	static size_t Decompress(void *OutData, size_t BufLength, const void *Data, size_t DataLength);
};

/// Strumie� do odczytu, kt�ry dekompresuje dane w formacie gzip odczytuj�c te� nag��wek.
/** Wszystko podobnie jak w klasie ZlibCompressionStream. */
class GzipDecompressionStream : public OverlayStream
{
private:
	scoped_ptr<ZlibDecompressionStream_pimpl> pimpl;

public:
	GzipDecompressionStream(Stream *a_Stream, int WindowBits = 15);
	virtual ~GzipDecompressionStream();

	// ======== Implementacja Stream ========
	
	virtual size_t Read(void *Out, size_t MaxLength);
	virtual bool End();

	/** \name Pobieranie informacji z nag��wka.
	Je�li plik posiada nag��wek, ten nag��wek przechowuje dan� informacj� i
	zosta� ju� w toku odczytywania odczytany, zwraca true i zwraca przez parametr
	z t� informacj�. */
	//@{
	bool GetHeaderFileName(string *OutFileName);
	bool GetHeaderComment(string *OutComment);
	//@}
};

/// Tryb otwarcia pliku GZip
enum GZIP_FILE_MODE
{
	GZFM_WRITE, ///< Do zapisu
	GZFM_READ,  ///< Do odczytu
};

/// \internal
class GzipFileStream_pimpl;

/// Obs�uga pliku w formacie gzip (zalecane rozszerzenie ".gz")
class GzipFileStream : public Stream
{
private:
	scoped_ptr<GzipFileStream_pimpl> pimpl;

public:
	/** Level ma znaczenie tylko przy zapisie. */
	GzipFileStream(const string &FileName, GZIP_FILE_MODE Mode, int Level = ZLIB_DEFAULT_LEVEL);
	virtual ~GzipFileStream();

	// ======== Implementacja Stream ========
	
	virtual void Write(const void *Data, size_t Size);
	virtual void Flush();
	
	virtual size_t Read(void *Out, size_t MaxLength);
	virtual bool End();
};

//@}
// code_zlibutils

} // namespace common

#endif
