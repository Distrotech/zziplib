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
#include <string.h>
#include <errno.h>

#include <gzio.h>
#include <gzinfo.h>
#include <zlib.h>

#include <assert.h>

/* NOTE: gz_off_t / gz_off64_t are used for those platforms
   which do not export off_t / off64_t - especially WIN32.
   Otherwise they are identical to their UNIX98 counterparts.
 */

#if defined GZ_LARGEFILE64 && defined WIN32
#define fseeko _fseeki64
#define ftello _ftelli64
#endif

struct _GZ_FILE
{
    FILE* file;
    const char* mode;
    GZ_INFO info;
    gz_off64_t returnpos;
    gz_off64_t filelength;
    z_stream z_buffer;
    int started;
    int buf_error;
    size_t buf_avail;
    size_t buf_pos;
    unsigned char buf32k[32 * 1024];
};

static void buffer_init(GZ_FILE* stream)
{
    gz_info_reset(&stream->info);
    stream->buf_avail = 0;
    stream->returnpos = 0;
    stream->filelength = -1;
    stream->started = 0;
}

static int detected_compression(GZ_FILE* stream)
{
    if (stream->info.compressed >= 0) {
        return stream->info.compressed;
    } else {
        return gz_info_detect(&stream->info, stream->file);
    }
}

/* ----------------------------------------------------------------- */

GZ_FILE* gz_fopen(const char* path, const char* mode) 
{
    FILE* file = fopen(path, mode);
    if (file != NULL) {
        GZ_FILE* stream = malloc(sizeof(*stream));
        stream->mode = mode;
        stream->file = file;
        buffer_init(stream);
        if (strchr(mode, 'r') && gz_info_read_uncompressed(path))
            stream->info.compressed = 0; /* uncompressed */
        else if (strchr(mode, 'w') && gz_info_write_uncompressed(path))
            stream->info.compressed = 0; /* uncompressed */
        else if (strchr(mode, 'w') && gz_info_write_compressed(path))
            stream->info.compressed = 8; /* zlib compressed */
        return stream;
    } else {
        return NULL;
    }
}

GZ_FILE* gz_fdopen(int fd, const char* mode) 
{
    FILE* file = fdopen(fd, mode);
    if (file != NULL)
    {
        GZ_FILE* stream = malloc(sizeof(*stream));
        stream->mode = mode;
        stream->file = file;
        buffer_init(stream);
        return stream;
    } else {
        return NULL;
    }
}

GZ_FILE* gz_freopen(const char* path, const char* mode, GZ_FILE* stream) 
{
    FILE* file = freopen(path, mode, stream->file);
    if (file == NULL) {
        stream->mode = mode;
        buffer_init(stream);
        if (strchr(mode, 'r') && gz_info_read_uncompressed(path))
            stream->info.compressed = 0; /* uncompressed */
        else if (strchr(mode, 'w') && gz_info_write_uncompressed(path))
            stream->info.compressed = 0; /* uncompressed */
        else if (strchr(mode, 'w') && gz_info_write_compressed(path))
            stream->info.compressed = 8; /* zlib compressed */
        return stream;
    } else {
        free(stream);
        return NULL;
    }
}

static void _finish(GZ_FILE* stream); /* forward declaration */

void gz_fclose(GZ_FILE* stream)
{
    _finish(stream);
    fclose(stream->file);
    free(stream);
}

/* ----------------------------------------------------------------- */

size_t gz_fread(void* ptr, size_t size, size_t nmemb, GZ_FILE* stream) 
{
    if (detected_compression(stream))
    {
        size_t total_out = size * nmemb;
        size_t out_before;
        int err;
        if (! stream->started) {
            stream->started = 1;
#           define MAX_WINDOWSIZE_BITS 15 /* i.e. 32K window */
            inflateInit2(& stream->z_buffer, -MAX_WINDOWSIZE_BITS);
            /* negative WINDOWSIZE = RAW BUFFER (no extra header) */
            if (ftello(stream->file) != stream->info.rewindpos)
                fseeko(stream->file, stream->info.rewindpos, SEEK_SET);
        }
        /* read data */
        stream->z_buffer.next_out = ptr;
        stream->z_buffer.avail_out = total_out;
        do {
            if (stream->buf_error) break;
            if (stream->z_buffer.avail_in == 0) {
                size_t avail_in = fread(stream->buf32k, sizeof(char),
                        sizeof(stream->buf32k), stream->file);
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

static const char gzipheader[] =
        "\x1F\x8B\x08\00" /* magic compression gzipflags */
        "\00\00\00\00" /* mtime */
        "\00\00"; /* extraflags ostype */

size_t gz_fwrite(const void* ptr, size_t size, size_t nmemb, GZ_FILE* stream)
{
    if (detected_compression(stream)) {
        size_t total_out = size * nmemb;
        int err;
        if (! stream->started) {
            size_t out;
            stream->started = 1;
#           define MAX_WINDOWSIZE_BITS 15 /* i.e. 32K window */
            deflateInit(& stream->z_buffer, Z_DEFAULT_COMPRESSION);
            /* negative WINDOWSIZE = RAW BUFFER (no extra header) */
            out = fwrite(gzipheader, 10, 1, stream->file);
            if (out < 10) { return 0; } /* errno is set */
        }
        /* write data */
        stream->z_buffer.next_in = (void*) ptr;
        stream->z_buffer.avail_in = total_out;
        do {
            if (stream->buf_error) break;
            stream->z_buffer.next_out = stream->buf32k;
            stream->z_buffer.avail_out = sizeof(stream->buf32k);
            err = deflate(& stream->z_buffer, Z_NO_FLUSH);
            if (err == Z_OK)
            {
                fwrite(stream->buf32k, sizeof(char),
                        stream->z_buffer.next_out - stream->buf32k, stream->file);
            } else {
                stream->buf_error = err;
                break;
            }
        } while (stream->z_buffer.avail_in);
        return total_out - stream->z_buffer.avail_out;
    } else {
        return fwrite(ptr, size, nmemb, stream->file);
    }
}

static void _finish(GZ_FILE* stream) {
    if (strchr(stream->mode, 'w') && stream->started) {
        /* some bits had not been pushed to full bytes to be written before */
        stream->z_buffer.next_in = stream->buf32k + 128;
        stream->z_buffer.avail_in = 0;
        stream->z_buffer.next_out = stream->buf32k;
        stream->z_buffer.avail_out = 128;
        {
            int err = deflate(& stream->z_buffer, Z_FINISH);
            if (err == Z_OK) {
                fwrite(stream->buf32k, sizeof(char),
                        stream->z_buffer.next_out - stream->buf32k, stream->file);
            }
        }
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
        gz_rewind(stream);
        /* read data until end */
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
        gz_off64_t pos = offset;
        if (strchr(stream->mode, 'w')) {
            errno = EBADF; return -1;
        }
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
        fseeko(stream->file, stream->info.rewindpos, SEEK_SET);
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
