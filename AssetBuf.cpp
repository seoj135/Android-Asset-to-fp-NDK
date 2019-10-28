/*
 *  Author: Seo Gangwon
 *  Date: 2019-01-10
 *  Contact: seogs12@naver.com
 */
#include "AssetBuf.h"

#ifndef _STREAM_BUF_FOR_ANDROID_NDK_ASSET_CPP_
#define _STREAM_BUF_FOR_ANDROID_NDK_ASSET_CPP_

void _s_ndk_util::AssetBuf::init(int fd, int openMode, bool throw_exception) {
	m_filedes = fd;

	if (m_filedes >= 0) {
		m_mode = openMode;
	}

	if (throw_exception && !is_open()) {
		//   throw error("couldn't open file _s_ndk_util::AssetBuf", SRC_POS);
		throw - 1;
	}

	setg(buffer + s_pback_size,
		buffer + s_pback_size,
		buffer + s_pback_size);

	setp(buffer,
		buffer + s_buf_size - 1);
}

_s_ndk_util::AssetBuf::AssetBuf(FILE * fp, const char* openMode, bool throw_exception)
	: m_mode(0), m_filedes(-1), m_fp(fp) {
	init(fileno(fp), encodeOpenMode(openMode), throw_exception);
}

_s_ndk_util::AssetBuf::AssetBuf(FILE * fp, int openMode, bool throw_exception)
	: m_mode(0), m_filedes(-1), m_fp(fp) {
	init(fileno(fp), openMode, throw_exception);
}

_s_ndk_util::AssetBuf::AssetBuf(int fd, const char* openMode, bool throw_exception)
	: m_mode(0), m_filedes(-1), m_fp(NULL) {
	m_fp = fdopen(fd, openMode);
	init(fd, encodeOpenMode(openMode), throw_exception);
}

_s_ndk_util::AssetBuf::AssetBuf(int fd, int openMode, bool throw_exception)
	: m_mode(0), m_filedes(-1), m_fp(NULL) {
	m_fp = fdopen(fd, decodeOpenMode(openMode));
	init(fd, openMode, throw_exception);
}

void _s_ndk_util::AssetBuf::close() {
	if (is_open()) {
		this->sync();
		// NOTE: We don't do ::close(m_filedes) here since we leave that
		// up to whoever instantiated this _s_ndk_util::AssetBuf.
		m_filedes = -1;
		m_mode = 0;
		if(m_fp != NULL) fclose(m_fp);
	}
}

int _s_ndk_util::AssetBuf::underflow() // with help fropenMode Josuttis, p. 678
{
	//printf("gptr() %p, egptr() %p\n", gptr(), egptr());
	//printf("gptr() %d, egptr() %d\n", *gptr(), *egptr());
	// is read position before end of buffer?
	if (gptr() < egptr())
		return *gptr();

	
	int numPutback = 0;
	if (s_pback_size > 0) {
		// process size of putback area
		// -use number of characters read
		// -but at most four
		numPutback = gptr() - eback();
		if (numPutback > 4)
			numPutback = 4;

		// copy up to four characters previously read into the putback
		// buffer (area of first four characters)
		std::memcpy(buffer + (4 - numPutback), gptr() - numPutback, numPutback);
	}
	

	// read new characters
	const int num = fread(buffer + s_pback_size, sizeof(unsigned char), s_buf_size - s_pback_size, m_fp);

	if (num <= 0)
		return EOF;

	// reset buffer pointers
	setg(buffer + s_pback_size - numPutback, buffer + s_pback_size, buffer + s_pback_size + num);

	// return next character Hrmph. We have to cast to unsigned char to
	// avoid problems with eof. Problem is, -1 is a valid char value to
	// return. However, without a cast, char(-1) (0xff) gets converted
	// to int(-1), which is 0xffffffff, which is EOF! What we want is
	// int(0x000000ff), which we have to get by int(unsigned char(-1)).
	return static_cast <unsigned char> (*gptr());
}

int _s_ndk_util::AssetBuf::overflow(int c) {
	if (!(m_mode & std::ios::out) || !is_open()) return EOF;

	if (c != EOF) {
		// insert the character into the buffer
		*pptr() = c;
		pbump(1);
	}

	if (flushoutput() == EOF) {
		return -1; // ERROR
	}

	return c;
}

int _s_ndk_util::AssetBuf::sync() {
	if (flushoutput() == EOF) {
		return -1; // ERROR
	}
	return 0;
}

int _s_ndk_util::AssetBuf::flushoutput() {
	if (!(m_mode & std::ios::out) || !is_open()) return EOF;

	const int num = pptr() - pbase();

	if (fwrite(pbase(), 1, num, m_fp) == -1) {
		return EOF;
	}

	pbump(-num);
	return num;
}


int _s_ndk_util::AssetBuf::encodeOpenMode(const char* mode) {

	// Char
	if (strcmp(mode, "r") == 0) {
		return O_RDONLY;
	}
	else if (strcmp(mode, "r+") == 0) {
		return O_RDWR;
	}
	else if (strcmp(mode, "w") == 0) {
		return O_WRONLY;
	}
	else if (strcmp(mode, "w+") == 0) {
		return O_RDWR;
	}
	else if (strcmp(mode, "a") == 0) {
		return O_WRONLY | O_APPEND;
	}
	else if (strcmp(mode, "a+") == 0) {
		return O_RDWR | O_APPEND;
	}

	// Binary
	else if (strcmp(mode, "rb") == 0) {
		return O_RDONLY | O_BINARY;
	}
	else if (strcmp(mode, "rb+") == 0 || strcmp(mode, "r+b") == 0) {
		return O_RDWR | O_BINARY;
	}
	else if (strcmp(mode, "wb") == 0) {
		return O_WRONLY | O_BINARY;
	}
	else if (strcmp(mode, "rb+") == 0 || strcmp(mode, "r+b") == 0) {
		return O_RDWR | O_BINARY;
	}
	else if (strcmp(mode, "ab") == 0) {
		return O_WRONLY | O_APPEND | O_BINARY;
	}
	else if (strcmp(mode, "ab+") == 0 || strcmp(mode, "r+b") == 0) {
		return O_RDWR | O_APPEND | O_BINARY;
	}

	// by default
	return O_RDONLY;
}

const char* _s_ndk_util::AssetBuf::decodeOpenMode(int openMode) {
	// Char
	if (openMode == O_RDONLY) return "r";
	else if (openMode == O_RDWR) return "r+";	// "w+"
	else if (openMode == O_WRONLY) return "w";
	else if (openMode == O_RDWR) return "w+";	// not caught this condition
	else if (openMode == (O_WRONLY | O_APPEND)) return "a";
	else if (openMode == (O_RDWR | O_APPEND)) return "a+";

	// Binary
	else if (openMode == (O_RDONLY | O_BINARY)) return "rb";
	else if (openMode == (O_RDWR | O_BINARY)) return "rb+";
	else if (openMode == (O_WRONLY | O_BINARY)) return "wb";
	else if (openMode == (O_RDWR | O_BINARY)) return "wb+";		// not caught this condition
	else if (openMode == (O_WRONLY | O_APPEND | O_BINARY)) return "ab";
	else if (openMode == (O_RDWR | O_APPEND | O_BINARY)) return "ab+";

	// by default
	return "r";
}

#endif // !_STREAM_BUF_FOR_ANDROID_NDK_ASSET_CPP_