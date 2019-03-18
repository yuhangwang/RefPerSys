/*
 * File: refpersys/src/refpersys.cc
 *
 * Description:
 *      This file is part of the Reflective Persistent System. It implements the
 *      refpersys executable
 *
 * Author(s):
 *      Basile Starynkevitch <basile@starynkevitch.net>
 *      Niklas Rosencrantz <niklasro@gmail.com>
 *      Abhishek Chakravarti <abhishek@taranjali.org>
 *
 * Copyright:
 *      (c) 2019 The Reflective Persistent System Team
 *      <https://refpersys.gitlab.io>
 *
 * License:
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


        /* include required header files */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <inttypes.h>
#include <argp.h>
#include "../inc/.version.gen.h"


/** COMMAND LINE ARGUMENT PARSING *********************************************/


        /* define long names for argument options */
#define NAME_PRAND "print-random-uint64"


        /* define keys for argument options */
#define KEY_PRAND 'r'


        /* define documentation strings for arguments options */
#define DOC_PRAND "Print random 64-bit unsigned integer"


        /* define length of buffer to hold version metadata */
#define VERSION_BFRLEN 1024


        /* define error message to display if no argument is provided */
#define MSG_NOARGS "refpersys: no options specified\n" \
                   "\trun refpersys --help to display options\n"


        /* define buffer to hold version metadata generated at compile time;
         * TODO: confirm from Dr. Basile if this needs to be thread_local */
static char version_bfr[VERSION_BFRLEN];


        /* set the address to e-mail bug reports; this global is provided by the
         * ARGP library */
const char *argp_program_bug_address = "basile@starynkevitch.net";


        /* set the version information; this global is provided by the ARGP
         * library */
const char *argp_program_version = version_bfr;


        /* define ARGP-specific vector to hold argument options;
         * TODO: confirm from Dr. Basile if this needs to be thread_local */
static struct argp_option argopt_vec[] = {
        {
                .name = NAME_PRAND,
                .key = KEY_PRAND,
                .arg = NULL,
                .flags = 0,
                .doc = DOC_PRAND
        },
        { NULL }
};


        /* parse version metadata from the .version.gen.h header file generated
         * at compile time */
static void version_parse(void)
{
        const char *format = "refpersys: version information\n"
                             "\t last git commit: %s\n";

        snprintf(version_bfr, VERSION_BFRLEN, format, RPS_VERSION_LASTCOMMIT);
}


        /* define helper function to execute the NAME_PRAND argument option */
static void exec_prand(void)
{
        //printf("rps_random_uint64(): %" PRIu64 "\n", rps_random_uint64());
        printf("rps_random_uint64(): %" PRIu64 "\n", 5UL);
}


        /* define the argument option parsing callback */
static error_t argopt_parse(int key, char *arg, struct argp_state *state)
{
                /* switch through argument option keys, handling each case in
                 * turn as required */
        switch (key) {
        case KEY_PRAND:
                exec_prand();
                break;

        default:
                return ARGP_ERR_UNKNOWN;
        }

                /* signal that no error has occurred */
        return 0;
}


/** MAIN ENTRY POINT **********************************************************/


        /* process refpersys command invoked with with arguments */
int main(int argc, char **argv)
{
        struct argp argopt = {
                .options = argopt_vec,
                .parser = argopt_parse,
                .args_doc = NULL,
                .doc = NULL
        };

                /* parse refpersys version metadata generated at compile time */
        version_parse();

                /* ensure we have at least one command line argument in addition
                 * to the refpersys command */
        if (argc < 2) {
                printf(MSG_NOARGS);
                exit(EXIT_SUCCESS);
        }

                /* parse argument options */
        return argp_parse(&argopt, argc, argv, 0, NULL, NULL);
}

