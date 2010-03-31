#ifndef _LIBGZ_GZIO_H
#define _LIBGZ_GZIO_H

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


/* _config.h must never be installed */
#include <gzconfig.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>

/* ------------ checking autodetected configuration ------------- */

/* win32 special and overrides */
#if defined WIN32
# undef GZ_SIZEOF_OFF64_T
#define GZ_SIZEOF_OFF64_T 8
#define gz_off64_t __int64
#elif defined _gz_off64_t
#define gz_off64_t _gz_off64_t
#endif
#if defined _gz_off_t
#define gz_off_t _gz_off_t
#endif

/* due to ax_prefix_config_h.m4 */
#ifndef gz_off_t
#define gz_off_t off_t
#endif
#ifndef gz_off64_t
#define gz_off64_t off64_t
#endif

#if GZ_SIZEOF_LONG != GZ_SIZEOF_OFF64_T
#define GZ_LARGEFILE64 /* largefile sensitive platform */
#if _FILE_OFFSET_BITS+0 == 64 || defined _LARGE_FILES || defined WIN32
#define GZ_RENAMED64 /* with dualmode largefile functions */
#endif
#endif

#ifdef GZ_RENAMED64
#define gz_filelength gz_filelength64
#define gz_fseeko gz_fseeko64
#define gz_ftello gz_ftello64
#define gz_fgetpos gz_fgetpos64
#define gz_fsetpos gz_fsetpos64
#endif

/* ------------ structure and function declarations ------------- */

/* opaque handle structure */
typedef struct _GZ_FILE GZ_FILE;

GZ_FILE*   gz_fopen(const char* path, const char* mode);
GZ_FILE*   gz_fdopen(int fd, const char* mode);
GZ_FILE*   gz_freopen(const char* path, const char* mode, GZ_FILE* stream);
void       gz_fclose(GZ_FILE* stream);

size_t     gz_fread(void* ptr, size_t size, size_t nmemb, GZ_FILE* stream);
size_t     gz_fwrite(const void* ptr, size_t size, size_t nmemb, GZ_FILE* stream);
int        gz_feof(GZ_FILE* stream);
int        gz_ferror(GZ_FILE* stream);
int        gz_fileno(GZ_FILE* stream);
void       gz_clearerr(GZ_FILE* stream);

gz_off_t   gz_filelength(GZ_FILE* stream);
int        gz_fseek(GZ_FILE* stream, long offset, int whence);
long       gz_ftell(GZ_FILE* stream);
int        gz_fseeko(GZ_FILE* stream, gz_off_t offset, int whence);
gz_off_t   gz_ftello(GZ_FILE* stream);
int        gz_fgetpos(GZ_FILE* stream, fpos_t* pos);
int        gz_fsetpos(GZ_FILE* stream, fpos_t* pos);
void       gz_rewind(GZ_FILE* stream);

#if defined GZ_LARGEFILE64 && defined _LARGEFILE64_SOURCE && !defined GZ_RENAMED64
gz_off64_t gz_filelength64(GZ_FILE* stream);
int        gz_fseeko64(GZ_FILE* stream, gz_off64_t offset, int whence);
gz_off64_t gz_ftello64(GZ_FILE* stream);
int        gz_fgetpos64(GZ_FILE* stream, fpos64_t* pos);
int        gz_fsetpos64(GZ_FILE* stream, fpos64_t* pos);
#endif

#endif
