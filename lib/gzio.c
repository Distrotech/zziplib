/* (MIT/X11 License)
   
   Copyright (c) 2010 Guido U. Draheim

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#include <_config.h>

#define _GZ_SOURCE 1
#define _LARGEFILE_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <gzio.h>
#include <zlib.h>

/* NOTE: gz_off_t / gz_off64_t are used for those platforms
   which do not export off_t / off64_t - especially WIN32.
   Otherwise they are identical to their UNIX98 counterparts.
*/

#if defined GZ_LARGEFILE64 && defined WIN32
#define fseeko _fseeki64
#define ftello _ftelli64
#endif

#define UNKNOWN -1

struct _GZ_FILE
{
	FILE* file;
	int compressed;
	int mask;
	gz_off64_t returnpos;
	gz_off64_t rewindpos;
	int gzipflags;
	z_stream z_buffer;
	int buf_error;
	size_t buf_avail;
	size_t buf_pos;
	unsigned char buf32k[32 * 1024];
	gz_off64_t filelength;
};

GZ_FILE* gz_fopen(const char* path, const char* mode) 
{
	GZ_FILE* stream = malloc(sizeof(*stream));
	stream->file = fopen(path, mode);
	stream->compressed = UNKNOWN;
	stream->mask = 0;
	stream->filelength = -1;
	return stream;
}

GZ_FILE* gz_fdopen(int fd, const char* mode) 
{
	GZ_FILE* stream = malloc(sizeof(*stream));
	stream->file = fdopen(fd, mode);
	stream->compressed = UNKNOWN;
	stream->mask = 0;
	stream->filelength = -1;
	return stream;
}

GZ_FILE* gz_freopen(const char* path, const char* mode, GZ_FILE* stream) 
{
	stream->file = freopen(path, mode, stream->file);
	stream->compressed = UNKNOWN;
	stream->mask = 0;
	stream->filelength = -1;
	return stream;
}

void gz_fclose(GZ_FILE* stream) {
	fclose(stream->file);
	free(stream);
}

#if __STDC_VERSION__+0 > 199900L || __GNUC__ > 2
static inline int freadchar(GZ_FILE* stream)
{
	return fgetc(stream->file) ^ stream->mask;
}
#else
#define freadchar(__stream) (fgetc((__stream)->file) ^ (__stream)->mask);
#endif

static void buffer_init(GZ_FILE* stream)
{
	stream->buf_avail = 0;
	stream->returnpos = 0;
}

typedef enum detection_error
{
    DETECT_OK,
    DETECT_BAD_MAGIC_1,
    DETECT_BAD_MAGIC_2,
    DETECT_UNSUPPORTED_COMPRESSION,
    DETECT_UNSUPPORTED_FLAGS,
    DETECT_BAD_EXTRALENGTH_1,
    DETECT_BAD_EXTRALENGTH_2,
    DETECT_EOF_IN_EXTRAFIELD,
    DETECT_EOF_IN_FILENAME,
    DETECT_EOF_IN_COMMENT,
    DETECT_EOF_IN_CRC,
} detection_error_t;

static int detected_compression(GZ_FILE* stream)
{
	if (stream->compressed != UNKNOWN) {
		return stream->compressed;
	} else {
		detection_error_t oops = DETECT_OK;
		register int ch = -1;
		fseeko(stream->file, 0, SEEK_SET);
		ch = fgetc(stream->file);
		if (ch != 31) { oops=DETECT_BAD_MAGIC_1; goto uncompressed; }
		ch = freadchar(stream);
		if (ch != 139) { oops=DETECT_BAD_MAGIC_2; goto uncompressed; }
		ch = freadchar(stream);
		if (ch != 8) { oops=DETECT_UNSUPPORTED_COMPRESSION; goto uncompressed; }
		stream->compressed = ch;
		ch = freadchar(stream);
		if (ch == EOF) { oops=DETECT_UNSUPPORTED_FLAGS; goto uncompressed; }
		stream->gzipflags = ch;
		ch = freadchar(stream);
		ch = freadchar(stream);
		ch = freadchar(stream);
		ch = freadchar(stream);
		/* buffer[4..7] - modification time */
		ch = freadchar(stream);
		/* buffer[8]    - extra flags */
		ch = freadchar(stream);
		/* buffer[9]    - operating system */
		if (stream->gzipflags & 4){  /* extra field */
			unsigned long len = 0;
			ch = freadchar(stream);
			if (ch == EOF) { oops=DETECT_BAD_EXTRALENGTH_1; goto uncompressed; }
			len = ch;
			ch = freadchar(stream);
			if (ch == EOF) { oops=DETECT_BAD_EXTRALENGTH_2; goto uncompressed; }
			len += (unsigned long)ch << 8;
			ch = fseeko(stream->file, len, SEEK_CUR);
			if (ch) { oops=DETECT_EOF_IN_EXTRAFIELD; goto uncompressed; }
		}
		if (stream->gzipflags & 8) { /* filename */
			for(;;) {
				ch = freadchar(stream);
				if (ch == EOF) { oops=DETECT_EOF_IN_FILENAME; goto uncompressed; }
				if (ch == 0) break;
			}
		}
		if (stream->gzipflags & 16) { /* comment */
			for(;;) {
				ch = freadchar(stream);
				if (ch == EOF) { oops=DETECT_EOF_IN_COMMENT; goto uncompressed; }
				if (ch == 0) break;
			}
		}
		if (stream->gzipflags & 2) { /* header crc */
			ch = freadchar(stream);
			if (ch == EOF) { oops=DETECT_EOF_IN_CRC; goto uncompressed; }
			ch = freadchar(stream);
			if (ch == EOF) { oops=DETECT_EOF_IN_CRC; goto uncompressed; }
		}
		stream->rewindpos = ftello(stream->file);
#               define MAX_WINDOWSIZE_BITS 15 /* i.e. 32K window */
		inflateInit2(& stream->z_buffer, -MAX_WINDOWSIZE_BITS); /* negative is RAW (no header) */
		buffer_init(stream);
		return stream->compressed;
	uncompressed:
		stream->compressed = 0;
		stream->rewindpos = 0;
		return stream->compressed;
	}
}

/* ----------------------------------------------------------------- */

size_t gz_fread(void* ptr, size_t size, size_t nmemb, GZ_FILE* stream) 
{
	if (detected_compression(stream)) 
	{
		size_t total_out = size * nmemb;
		size_t out_before;
		int err;
		stream->z_buffer.next_out = ptr;
		stream->z_buffer.avail_out = total_out;
		do {
			if (stream->buf_error) break;
			if (stream->z_buffer.avail_in == 0) {
				size_t avail_in = fread(stream->buf32k, sizeof(char), sizeof(stream->buf32k), stream->file);
				if (avail_in == 0) {
					stream->buf_error = EOF;
					break;
				}
				stream->z_buffer.avail_in = avail_in;
				stream->z_buffer.next_in = stream->buf32k;
			}
			out_before = stream->z_buffer.total_out;
			err = inflate(& stream->z_buffer, Z_NO_FLUSH);
			if (err == Z_OK) {
				stream->returnpos += stream->z_buffer.total_out - out_before;
			} else if (err == Z_STREAM_END) {
				stream->returnpos += stream->z_buffer.total_out - out_before;
				stream->buf_error = EOF;
				break;
			} else {
				stream->buf_error = err;
				break;
			}
		} while (stream->z_buffer.avail_out);
		return total_out - stream->z_buffer.avail_out;
	} else {
	    return fread(ptr, size, nmemb, stream->file);
	}
}

size_t gz_fwrite(const void* ptr, size_t size, size_t nmemb, GZ_FILE* stream)
{
	if (detected_compression(stream)) {
	    errno = ENOSYS; /* NOT IMPLEMENTED */
	    return 0;
	} else {
	    return fwrite(ptr, size, nmemb, stream->file);
	}
}

int gz_feof(GZ_FILE* stream)
{
	if (detected_compression(stream)) {
		return stream->buf_error == EOF;
	} else {
		return feof(stream->file);
	}
}

int gz_ferror(GZ_FILE* stream)
{
	if (detected_compression(stream)) {
		return stream->buf_error;
	} else {
		return ferror(stream->file);
	}
}

int gz_fileno(GZ_FILE* stream)
{
	return fileno(stream->file);
}

void gz_clearerr(GZ_FILE* stream)
{
	stream->buf_error = 0;
	return clearerr(stream->file);
}

/* ----------------------------------------------------------------- */
gz_off64_t gz_filelength(GZ_FILE* stream)
{
    if (stream->filelength >= 0) {
	return stream->filelength;
    }
    if (detected_compression(stream)) {
	gz_off64_t old = gz_ftello(stream);
	/* read data until end */
	gz_rewind(stream);
	/* read data until pos */
	while (1)
	{
		char buffer[32 * 1024];
		size_t done = gz_fread(buffer, sizeof(char), sizeof(buffer), stream);
		if (! done || gz_feof(stream)) break;
	}
	stream->filelength = gz_ftello(stream);
	gz_fseeko(stream, old, SEEK_SET);
    } else {
	off64_t old = ftello(stream->file);
	int err = fseeko(stream->file, 0, SEEK_END);
	if (err) return -1L;
	stream->filelength = ftello(stream->file);
	fseeko(stream->file, old, SEEK_SET);
    }
    return stream->filelength;
}

#ifdef GZ_RENAMED64
#undef gz_filelength
long gz_filelength(GZ_FILE* stream);

long gz_filelength(GZ_FILE* stream) 
{
	if (sizeof(long) == sizeof(gz_off64_t)) {
		return gz_filelength64(stream);
	} else {
		gz_off64_t len = gz_filelength64(stream);
		if (len >= 0) {
			long len32 = len;
			if (len32 == len) return len32;
			errno = EFBIG; /* EOVERFLOW */
		}
		return -1L;
	}
}
#endif

int gz_fseek(GZ_FILE* stream, long offset, int whence) 
{
	return gz_fseeko(stream, offset, whence);
}

long gz_ftell(GZ_FILE* stream)
{
	off64_t off = gz_ftello(stream);
	if (off >= 0) {
		long off32 = off;
		if (off32 == off) return off32;
		errno = EFBIG; /* EOVERFLOW */
	}
	return -1;
}

int gz_fseeko(GZ_FILE* stream, gz_off_t offset, int whence)
{
	if (detected_compression(stream)) {
		off64_t pos = offset;
		if (whence == SEEK_CUR) pos += stream->returnpos;
		if (whence == SEEK_END) pos = gz_filelength(stream) - offset;
		gz_rewind(stream);
		/* read data until pos */
		while (pos)
		{
			char buffer[32 * 1024];
			size_t done = gz_fread(buffer, sizeof(char), sizeof(buffer), stream);
			pos -= done;
			if (! done) return -1;
		}
		return 0;
	} else {
		return fseeko(stream->file, offset, whence);
	}
}

gz_off_t gz_ftello(GZ_FILE* stream)
{
	if (detected_compression(stream)) {
		return stream->returnpos;
	} else {
		return ftello(stream->file);
	}
}

#ifdef GZ_RENAMED64
#undef gz_fseeko
#undef gz_ftello
int  gz_fseeko(GZ_FILE* stream, long offset, int whence);
long gz_ftello(GZ_FILE* stream);

int gz_fseeko(GZ_FILE* stream, long offset, int whence) 
{
	return gz_fseeko64(stream, offset, whence);
}

long gz_ftello(GZ_FILE* stream) 
{
	if (sizeof(long) == sizeof(gz_off64_t)) {
		return gz_ftello64(stream);
	} else {
		gz_off64_t off = gz_ftello64(stream);
		if (off >= 0) {
			long off32 = off;
			if (off32 == off) return off32;
			errno = EFBIG; /* EOVERFLOW */
		}
		return -1L;
	}
}
#endif


void gz_rewind(GZ_FILE* stream)
{
	if (detected_compression(stream)) {
		rewind(stream->file);
		fseeko(stream->file, stream->rewindpos, SEEK_SET);
		inflateReset(& stream->z_buffer);
		stream->buf_error = 0;
	} else {
		return rewind(stream->file);
	}
}

int gz_fgetpos(GZ_FILE* stream, fpos_t* pos)
{
	if (detected_compression(stream)) {
		off64_t* offp = (off64_t*) pos;
		*offp = stream->returnpos;
		return 0;
	} else {
		return fgetpos(stream->file, pos);
	}
}

int gz_fsetpos(GZ_FILE* stream, fpos_t* pos)
{
	if (detected_compression(stream)) {
		gz_off64_t* offp = (gz_off64_t*) pos;
		return gz_fseeko(stream, *offp, SEEK_SET);
	} else {
		return fsetpos(stream->file, pos);
	}
}

#ifdef GZ_RENAMED64
#undef gz_fgetpos
#undef gz_fsetpos
int gz_fgetpos(GZ_FILE* stream, long* pos);
int gz_fsetpos(GZ_FILE* stream, long* pos);

int gz_fgetpos(GZ_FILE* stream, long* pos) 
{
	if (sizeof(long) == sizeof(gz_off64_t)) {
		return gz_fgetpos64(stream, (fpos_t*) pos);
	} else {
		fpos_t offs;
		int err = gz_fgetpos64(stream, &offs);	
		if (err == 0) {
			gz_off64_t off = *(gz_off64_t*)(&offs);
			long off32 = off;
			if (off32 == off) { *pos = off32; return 0; }
			errno = EFBIG; /* EOVERFLOW */
		}
		return -1L;
	}
}

int gz_fsetpos(GZ_FILE* stream, long* pos) 
{
	if (sizeof(long) == sizeof(gz_off64_t)) {
		return gz_fsetpos64(stream, (fpos_t*) pos);
	} else {
		fpos_t offs;
		*(gz_off64_t*)(&offs) = *pos;
		return gz_fsetpos64(stream, & offs);
	}
}
#endif
