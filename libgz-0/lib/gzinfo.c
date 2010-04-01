#define _XOPEN_SOURCE 700 /* glibc idiots will only allow strdup with this */

#include <_config.h>
#include <gzinfo.h>
#include <string.h>
#include <fnmatch.h>
#include <stdlib.h>

#if defined GZ_LARGEFILE64 && defined WIN32
#define fseeko _fseeki64
#define ftello _ftelli64
#endif

#define UNKNOWN -1

void gz_info_reset(GZ_INFO* info)
{
    info->compressed = UNKNOWN;
    info->error = NULL;
    info->rewindpos = 0;
    info->gzipflags = 0;
}

#if __STDC_VERSION__+0 > 199900L || __GNUC__ > 2
static inline int freadchar(GZ_INFO* stream, FILE* file)
{
    return fgetc(file);
}
#else
#define freadchar(__stream, __file) (fgetc((__file)));
#endif

int gz_info_detect(GZ_INFO* info, FILE* file)
{
    if (info->compressed != UNKNOWN) {
        return info->compressed;
    } else {
        const char* oops = NULL;
        register int ch = -1;
        fseeko(file, 0, SEEK_SET);
        ch = fgetc(file);
        if (ch == EOF)  { oops="eof magic 1"; goto uncompressed; }
        if (ch != 0x1F) { oops="bad magic 1"; goto uncompressed; }
        ch = freadchar(info, file);
        if (ch == EOF)  { oops="eof magic 2"; goto uncompressed; }
        if (ch != 0x8B) { oops="bad magic 2"; goto uncompressed; }
        ch = freadchar(info, file);
        if (ch == EOF) { oops="eof compression field"; goto uncompressed; }
        if (ch != 8) { oops="unsupported compression"; goto uncompressed; }
        info->compressed = ch;
        ch = freadchar(info, file);
        if (ch == EOF) { oops="eof gzip flags"; goto uncompressed; }
        info->gzipflags = ch;
        ch = freadchar(info, file);
        if (ch == EOF) { oops="eof mtime field 1"; goto uncompressed; }
        ch = freadchar(info, file);
        if (ch == EOF) { oops="eof mtime field 2"; goto uncompressed; }
        ch = freadchar(info, file);
        if (ch == EOF) { oops="eof mtime field 3"; goto uncompressed; }
        ch = freadchar(info, file);
        if (ch == EOF) { oops="eof mtime field 4"; goto uncompressed; }
        ch = freadchar(info, file);
        if (ch == EOF) { oops="eof extra flags"; goto uncompressed; }
        ch = freadchar(info, file);
        if (ch == EOF) { oops="eof source os flag"; goto uncompressed; }
        if (info->gzipflags & 4){  /* extras field */
            unsigned long len = 0;
            ch = freadchar(info, file);
            if (ch == EOF) { oops="eof extras length 1"; goto uncompressed; }
            len = ch;
            ch = freadchar(info, file);
            if (ch == EOF) { oops="eof extras length 2"; goto uncompressed; }
            len += (unsigned long)ch << 8;
            ch = fseeko(file, len, SEEK_CUR);
            if (ch) { oops="eof in extras field"; goto uncompressed; }
        }
        if (info->gzipflags & 8) { /* filename */
            for(;;) {
                ch = freadchar(info, file);
                if (ch == EOF) { oops="eof in filename field"; goto uncompressed; }
                if (ch == 0) break;
            }
        }
        if (info->gzipflags & 16) { /* comment */
            for(;;) {
                ch = freadchar(info, file);
                if (ch == EOF) { oops="eof in comment field"; goto uncompressed; }
                if (ch == 0) break;
            }
        }
        if (info->gzipflags & 2) { /* header crc */
            ch = freadchar(info, file);
            if (ch == EOF) { oops="eof crc 1"; goto uncompressed; }
            ch = freadchar(info, file);
            if (ch == EOF) { oops="eof crc 2"; goto uncompressed; }
        }
        info->rewindpos = ftello(file);
        info->error = NULL;
        return info->compressed;
    uncompressed:
        rewind(file);
        info->compressed = 0; /* allow reading as raw data */
        info->rewindpos = 0;
        info->error = oops;
        return info->compressed;
    }
}

/* ----------------------------------------------------------------- */


typedef struct _Node { const char* pattern; struct _Node* next; } Node;

static Node* write_compressed = NULL;
static Node* write_uncompressed = NULL;
static Node* read_uncompressed = NULL;

static const char* default_write_compressed = "*.gz";
static const char* default_write_uncompressed = "/dev/*";
static const char* default_read_uncompressed = "/dev/*";


void gz_pattern_write_compressed(const char* pattern)
{
    Node* node = malloc(sizeof(*node));
    node->pattern = strdup(pattern);
    node->next = write_compressed; write_compressed = node;
}

void gz_pattern_write_uncompressed(const char* pattern)
{
    Node* node = malloc(sizeof(*node));
    node->pattern = strdup(pattern);
    node->next = write_uncompressed; write_uncompressed = node;
}

void gz_pattern_read_uncompressed(const char* pattern)
{
    Node* node = malloc(sizeof(*node));
    node->pattern = strdup(pattern);
    node->next = read_uncompressed; read_uncompressed = node;
}

int gz_info_read_uncompressed(const char* filename)
{
    if (read_uncompressed == NULL) {
        if (! fnmatch(default_read_uncompressed, filename, 0)) {
            return 1;
        }
    } else {
        Node* node = read_uncompressed;
        for(; node; node = node->next) {
            if (! fnmatch(node->pattern, filename, 0)) {
                return 1;
            }
        }
    }
    return 0;
}

int gz_info_write_uncompressed(const char* filename)
{
    if (write_uncompressed == NULL) {
        if (! fnmatch(default_write_uncompressed, filename, 0)) {
            return 1;
        }
    } else {
        Node* node = write_uncompressed;
        for(; node; node = node->next) {
            if (! fnmatch(node->pattern, filename, 0)) {
                return 1;
            }
        }
    }
    return 0;
}

int gz_info_write_compressed(const char* filename)
{
    if (write_compressed == NULL) {
        if (! fnmatch(default_write_compressed, filename, 0)) {
            return 1;
        }
    } else {
        Node* node = write_compressed;
        for(; node; node = node->next) {
            if (! fnmatch(node->pattern, filename, 0)) {
                return 1;
            }
        }
    }
    return 0;
}
