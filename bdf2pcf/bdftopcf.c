/*

Copyright 1991, 1993, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from The Open Group.

*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

//#include <X11/X.h>
//#include <X11/Xproto.h>
#include "fontmisc.h"
#include <X11/fonts/fontstruct.h>
#include "fntfilio.h"
#include "fntfil.h"
#include "bdfint.h"
#include "pcf.h"
#include <stdio.h>
#include <X11/Xos.h>

//#include <emscripten.h>
int char_dict_load(const char* dict_file);

int
main(int argc, char *argv[])
{
    FontRec font = { 0 };
    FontFilePtr input, output;
    char* input_name = NULL, * output_name = NULL, * dict_name = NULL;
    char* program_name;
    int bit, byte, glyph, scan, lz4 = 0;

    FontDefaultFormat(&bit, &byte, &glyph, &scan);
    program_name = argv[0];
    argc--, argv++;
    while (argc-- > 0) {
        if (argv[0][0] == '-') {
            switch (argv[0][1]) {
            case 'p':
                switch (argv[0][2]) {
                case '1':
                case '2':
                case '4':
                case '8':
                    if (argv[0][3] != '\0')
                        goto usage;
                    glyph = argv[0][2] - '0';
                    break;
                default:
                    goto usage;
                }
                break;

            case 'u':
                switch (argv[0][2]) {
                case '1':
                case '2':
                case '4':
                    if (argv[0][3] != '\0')
                        goto usage;
                    scan = argv[0][2] - '0';
                    break;
                default:
                    goto usage;
                }
                break;

            case 'm':
                if (argv[0][2] != '\0')
                    goto usage;
                bit = MSBFirst;
                break;

            case 'z':
                if (argv[0][2] != '\0')
                    goto usage;
                lz4 = 1;
                break;

            case 'l':
                if (argv[0][2] != '\0')
                    goto usage;
                bit = LSBFirst;
                break;

            case 'M':
                if (argv[0][2] != '\0')
                    goto usage;
                byte = MSBFirst;
                break;

            case 'L':
                if (argv[0][2] != '\0')
                    goto usage;
                byte = LSBFirst;
                break;

            case 't':          /* attempt to make terminal fonts if possible */
                if (argv[0][2] != '\0')
                    goto usage;
                break;

            case 'i':          /* inhibit ink metric computation */
                if (argv[0][2] != '\0')
                    goto usage;
                break;

            case 'o':
                if (argv[0][2])
                    output_name = argv[0] + 2;
                else {
                    if (!argv[1])
                        goto usage;
                    argv++;
                    argc--;
                    output_name = argv[0];
                }
                break;

            case 'd':
                if (argv[0][2])
                    dict_name = argv[0] + 2;
                else {
                    if (!argv[1])
                        goto usage;
                    argv++;
                    argc--;
                    dict_name = argv[0];
                }
                break;

            case 'v':
                printf("%s\n", "X bdftopcf");
                exit(0);

            default:
                goto usage;
            }
        }
        else {
            if (input_name) {
            usage:
                fprintf(stderr, "%s: invalid option '%s'\n",
                    program_name, argv[0]);
                fprintf(stderr,
                    "usage: %s [-p#] [-u#] [-z] [-m] [-l] [-M] [-L] [-t] [-i] [-d dict file] [-o pcf file] [bdf file]\n"
                    "       where # for -p is 1, 2, 4, or 8\n"
                    "       and   # for -u is 1, 2, or 4\n",
                    program_name);
                exit(1);
            }
            input_name = argv[0];
        }
        argv++;
    }

    // the input/source bdf file
    if (input_name) {
        input = FontFileOpen(input_name);
        if (!input) {
            fprintf(stderr, "%s: can't open bdf source file %s\n",
                program_name, input_name);
            exit(1);
        }
    }
    else {
        fprintf(stdout, "please input bdf contents:\n");
        input = FontFileOpenFd(STDIN_FILENO);
    }

    // the dict file for output glyphics
    if (dict_name) {
        int result = char_dict_load(dict_name);
        if (!result) {
            fprintf(stderr, "%s: can't open dict file %s\n",
                program_name, dict_name);
            exit(1);
        }
    }

    // output pcf file
    if (bdfReadFont(&font, input, bit, byte, glyph, scan) != Successful) {
        fprintf(stderr, "%s: bdf input, %s, corrupt\n",
            program_name, input_name ? input_name : "<stdin>");
        exit(1);
    }
    if (output_name) {
        output = FontFileOpenWrite(output_name);
        if (!output) {
            fprintf(stderr, "%s: can't open pcf sink file %s\n",
                program_name, output_name);
            exit(1);
        }
    }
    else {
        output = FontFileOpenWriteFd(STDOUT_FILENO);
    }

    if (pcfWriteFont(&font, output, lz4) != Successful) {
        fprintf(stderr, "%s: can't write pcf file %s\n",
            program_name, output_name ? output_name : "<stdout>");
        if (output_name)
            remove(output_name);
        exit(1);
    }
    else
    {
        FontFileClose(output);
    }
    return (0);
}

int font_convert() {
    const char *input_name = "/input.bdf", *output_name="/output.pcf";
    FontRec font = { 0 };
    FontFilePtr input = NULL, output = NULL;
    int bit, byte, glyph, scan;

    FontDefaultFormat(&bit, &byte, &glyph, &scan);

    input = FontFileOpen(input_name);
    output = FontFileOpenWrite(output_name);
    int result = Successful;
    if (!input) {
        fprintf(stderr, "can't open bdf source file %s\n",
                input_name); fflush(stderr);
        result = 0;
    }
    if (!output) {
        fprintf(stderr, "can't open pcf sink file %s\n",
                output_name); fflush(stderr);
        result = 0;
    }
    if (result == Successful) {
        fprintf(stderr, "reading input font\n");
        result = bdfReadFont(&font, input, bit, byte, glyph, scan);
        if (result != Successful) {
            fprintf(stderr, "bdf input, corrupt\n");
        }
    }
    if (result == Successful) {
      fprintf(stderr, "preparing output font\n"); fflush(stderr);

        result = pcfWriteFont(&font, output, 0);
        if (result != Successful) {
            fprintf(stderr, "can't write pcf file %s\n", output_name); fflush(stderr);
            remove(output_name);
        }
    }
    if (input)
        FontFileClose(input);
    if (output)
        FontFileClose(output);
    return (result == Successful);
}
