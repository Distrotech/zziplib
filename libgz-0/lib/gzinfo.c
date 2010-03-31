#include <_config.h>
#include <gzinfo.h>

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
    return fgetc(file) ^ stream->mask;
}
#else
#define freadchar(__stream, __file) (fgetc((__file)) ^ (__stream)->mask));
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
        if (ch == EOF) { oops="eof magic 1"; goto uncompressed; }
        if (ch != 31)  { oops="bad magic 1"; goto uncompressed; }
        ch = freadchar(info, file);
        if (ch == EOF) { oops="eof magic 2"; goto uncompressed; }
        if (ch != 139) { oops="bad magic 2"; goto uncompressed; }
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
        info->compressed = 0; /* allow reading as raw data */
        info->rewindpos = 0;
        info->error = oops;
        return info->compressed;
    }
}
