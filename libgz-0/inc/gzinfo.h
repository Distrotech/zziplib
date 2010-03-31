#ifndef _LIBGZ_GZINFO_H
#define _LIBGZ_GZINFO_H

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

/* atleast for gz_off64_t */
#include <gzio.h>

typedef struct _GZ_INFO
{
    int compressed;
    int gzipflags;
    int mask;
    const char* error;
    gz_off64_t rewindpos;
} GZ_INFO;

void gz_info_reset(GZ_INFO *info);
int gz_info_detect(GZ_INFO* info, FILE* file);

/* all patterns are fnmatch() glob patterns */
void gz_pattern_write_uncompressed(const char* pattern);
void gz_pattern_write_compressed(const char* pattern);
void gz_pattern_read_uncompressed(const char* pattern);

int gz_info_write_compressed(const char* filename);
int gz_info_write_uncompressed(const char* filename);
int gz_info_read_uncompressed(const char* filename);

#endif
