/*
 *  Author: Seo Gangwon
 *  Date: 2019-01-10
 *  Contact: seogs12@naver.com
 *
 *  This is revised by Seogangwon to use Android asset to iostream. 2019-01-10
 *  The original code is from California Institute of Technology
 *
 *  ---------------------------------------------------------------------------------
 *  Copyright (c) 2002-2004 California Institute of Technology
 *  Copyright (c) 2004-2007 University of Southern California
 *  Rob Peters <rjpeters at usc dot edu>
 *
 *  This was a parts of Library that is GroovX On GNU General Public License
 *  REF
 *  BaseURL:    http://ilab.usc.edu/rjpeters/groovx/
 *  Header :    http://ilab.usc.edu/rjpeters/groovx/gzstreambuf_8h-source.html
 *  Source :    http://ilab.usc.edu/rjpeters/groovx/gzstreambuf_8cc-source.html
 *
 *  ---------------------------------------------------------------------------------
 *  Usage
 *
 *  #include <AssetFILEp.h>
 *  #include <AssetBuf.h>
 *  FILE* fp = _s_ndk_util::android_fopen([AssetFileName] , "r", [AssetManagerFromAndroidContext]);
 *  _s_ndk_util::AssetBuf assetBuf(fp, "r", false);
 *  std::iostream fs(&assetBuf);
 *
 *  ps+ [AssetFileName] location is res/assets (src/main/assets)
*/

#ifndef _STREAM_BUF_FOR_ANDROID_NDK_ASSET_H_
#define _STREAM_BUF_FOR_ANDROID_NDK_ASSET_H_

#include <cstdio> 
#include <streambuf> 
#include <iostream>
#include <ios> 
#include <fcntl.h>

// linux doesn't care about O_TEXT and O_BINARY
#ifndef O_TEXT
#define O_TEXT 0
#endif
#ifndef O_BINARY
#define O_BINARY 0
#endif

namespace _s_ndk_util {
	class AssetBuf : public std::streambuf {
	private:
		int m_mode;
		int m_filedes;
		FILE* m_fp;

		AssetBuf(const AssetBuf &);

		static const int s_buf_size = 4092;
		static const int s_pback_size = 4;
		char buffer[s_buf_size];

		void init(int fd, int openMode, bool throw_exception);
		int flushoutput();

	public:
		AssetBuf(FILE * fp, const char* openMode, bool throw_exception = false);
		AssetBuf(FILE * fp, int openMode, bool throw_exception = false);
		AssetBuf(int fd, const char* openMode, bool throw_exception = false);
		AssetBuf(int fd, int openMode, bool throw_exception = false);
		~AssetBuf() {
			close();
		}

		bool is_open() const {
			return m_filedes >= 0;
		}
		void close();

		virtual int underflow();
		virtual int overflow(int c);
		virtual int sync();

	private:
		int encodeOpenMode(const char* openMode);
		const char* decodeOpenMode(int openMode);
	};

	// -------------------------------------------------------------------------------
	/*
		This class is not checked for validation. 2019-01-10
	*/
	class Assetstream : public std::iostream {
	private:
		_s_ndk_util::AssetBuf m_buf;

	public:
		Assetstream(FILE * fp, int openMode, bool throw_exception = false)
			: std::iostream(0), m_buf(fp, openMode, throw_exception) {
			rdbuf(&m_buf);
		}

		Assetstream(int fd, int openMode, bool throw_exception = false)
			: std::iostream(0), m_buf(fd, openMode, throw_exception) {
			rdbuf(&m_buf);
		}

		bool is_open() const {
			return m_buf.is_open();
		}

		void close() {
			m_buf.close();
		}
	};
}
#endif // !_STREAM_BUF_FOR_ANDROID_NDK_ASSET_H_