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

#include "_config.h" /* autoconf puts _FILE_OFFSET_BITS=64 in this file */

#include <gzio.h>
#include <gzinfo.h>
#include <string.h>

const char* help =
"gzlib <command> <GZFILE> \n"
"commands: \n"
"- filelength GZFILE - printlength \n"
;

int main(int argc, char** argv)
{
    if (argc <= 1) {
	puts(help);
	return 0;
    }
    if (! strcmp(argv[1], "filelength") || ! strcmp(argv[1], "-l")) {
	const char* filename = argv[2];
	GZ_FILE* fp = gz_fopen(filename, "r");
	if (! fp) { perror(filename); return 1; }
	printf("%s: %lli\n", filename, (long long int) gz_filelength(fp));
	gz_fclose(fp);
	return 0;
    } else if (! strcmp(argv[1], "cat") || ! strcmp(argv[1], "-p")) {
	const char* filename = argv[2];
	GZ_FILE* fp = gz_fopen(filename, "r");
        if (! fp) { perror(filename); return 1; }
        {
            char buffer[1024];
            while (! gz_feof(fp)) {
                int len = gz_fread(buffer, sizeof(char), sizeof(buffer), fp);
                printf("%.*s", len, buffer);
                if (! len) break;
            }
            gz_fclose(fp);
	}
	return 0;	
    } else if (! strcmp(argv[1], "gzip") || ! strcmp(argv[1], "-c")) {
        GZ_FILE* gz;
        const char* filename = argv[2];
        GZ_FILE* fp = gz_fopen(filename, "r");
        if (! fp) { perror(filename); return 1; }
        {
            int len = strlen(filename);
            char* outputname = malloc(len+4);
            memcpy(outputname, filename, len);
            memcpy(outputname+len, ".gz", 4);
            gz_pattern_write_compressed("*");
            gz = gz_fopen(outputname, "w");
            if (! gz) { perror(outputname); return 1; }
            {
                char buffer[1024];
                while (! gz_feof(fp)) {
                    int len = gz_fread(buffer, sizeof(char), sizeof(buffer), fp);
                    if (! len) break;
                    gz_fwrite(buffer, sizeof(char), len, gz);
                }
                gz_fclose(gz);
            }
            gz_fclose(fp);
        }
        return 0;
    } else if (! strcmp(argv[1], "gunzip") || ! strcmp(argv[1], "-d")) {
        GZ_FILE* gz;
        const char* filename = argv[2];
        GZ_FILE* fp = gz_fopen(filename, "r");
        if (! fp) { perror(filename); return 1; }
        {
            int len = strlen(filename);
            char* outputname = malloc(len+5);
            if (len > 3 && !memcmp(filename+len-3, ".gz", 3)) {
                memcpy(outputname, filename, len-3);
                outputname[len-3] = '\0';
            } else {
                memcpy(outputname, filename, len);
                memcpy(outputname+len, ".raw", 5);
            }
            gz_pattern_write_uncompressed("*");
            gz = gz_fopen(outputname, "w");
            if (! gz) { perror(outputname); return 1; }
            {
                char buffer[1024];
                while (! gz_feof(fp)) {
                    int len = gz_fread(buffer, sizeof(char), sizeof(buffer), fp);
                    if (! len) break;
                    gz_fwrite(buffer, sizeof(char), len, gz);
                }
                gz_fclose(gz);
            }
            gz_fclose(fp);
        }
        return 0;
    } else {
	puts("unknown argument");
	puts(argv[1]);
	puts(help);
	return 1;
    }
}
