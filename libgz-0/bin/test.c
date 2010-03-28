#include <gzio.h>
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
	printf("%s: %lli", filename, (long long int) gz_filelength(fp));
	gz_fclose(fp);
	return 0;
    } else {
	puts("unknown argument");
	puts(argv[1]);
	puts(help);
	return 1;
    }
}