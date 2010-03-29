#ifndef _LIBGZ_GZSTDIO_H
#define _LIBGZ_GZSTDIO_H

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

#include <gzio.h>

# undef FILE
#define FILE GZ_FILE

# undef fopen
#define fopen gz_fopen
# undef fdopen
#define fdopen gz_fdopen
# undef freopen
#define freopen gz_freopen
# undef fclose
#define fclose gz_fclose

# undef fread
#define fread gz_fread
# undef fwrite
#define fwrite gz_fwrite
# undef feof
#define feof gz_feof
# undef ferror
#define ferror gz_ferror
# undef fileno
#define fileno gz_fileno
# undef clearerr
#define clearerr gz_clearerr

# undef filelength
#define filelength gz_filelength
# undef fseek
#define fseek gz_fseek
# undef ftell
#define ftell gz_ftell
# undef fseeko
#define fseeko gz_fseeko
# undef ftello
#define ftello gz_ftello
# undef fgetpos
#define fgetpos gz_fgetpos
# undef fsetpos
#define fsetpos gz_fsetpos
# undef rewind
#define rewind gz_rewind

#endif
