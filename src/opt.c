/*
 *  opt.c
 *  check_ice2 
 *
 *  Copyright (c) 2017 Michel Depeige.
 *  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of mosquitto nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "main.h"

/* global variables needed for getopt */
extern char	*optarg;
extern int	optind, opterr, optopt;
unsigned int	g_mode;
char		*g_host, *g_port, *g_mount, *g_name;

/* prototypes */
void	usage(void);
void	version(void);

#define	OPTION_STRING	"hH:m:n:p:vVw"

/* check arg, set variables as necessary, and return */
int	checkopt(int *argc, char ***argv)
{
	int	c;
	int	option_index = 0;

	g_mode = 0;
	g_name = NULL;
	g_host = "localhost";
	g_port = "8000";
	g_mount = "/";

	while (1)
	{
		static struct option long_options[] = {
		{"help", 0, 0, 'h'},
		{"host", 1, 0, 'H'},
		{"mount", 1, 0, 'p'},
		{"name", 1, 0, 'n'},
		{"port", 1, 0, 'p'},
		{"verbose", 0, 0, 'v'},
		{"version", 0, 0, 'V'},
		{"warning", 0, 0, 'w'},
		{0, 0, 0, 0}
		};

		c = getopt_long(*argc, *argv, OPTION_STRING,
				long_options, &option_index);

		if (c == -1)
			break;

		switch (c) {
			case 'h':
				usage();
				break;
			case 'H':
				g_host = optarg;
				break;
			case 'm':
				g_mount = optarg;
				break;
			case 'n':
				g_name = optarg;
				break;
			case 'p':
				g_port = optarg;
				break;
			case 'v':
				g_mode |= VERBOSE;
				break;
			case 'V':
				version();
				break;
			case 'w':
				g_mode |= ICYWARN;
				break;
			default:
				break;
		}
	}

     	*argc -= optind;
        *argv += optind;

	return 0;
}

/* display an error with perror and exit the program */
void    fatal(char *str)
{
	perror(str);
	exit(-1);
}

/* print usage and exit */
void	usage(void)
{
	printf("check_ice2 client version %s\n", VERSION);
	printf("check_ice2 [-H hostname] [-v] [-m mount]\n");
	printf("\t -h --help\t this screen.\n");
	printf("\t -H --host\t remote host.\n");
	printf("\t -m --mount\t remote mount.\n");
	printf("\t -p --port\t remote port.\n");
	printf("\t -v --verbose\t verbose mode.\n");
	printf("\t -V --version\t show version and exit.\n");
	printf("\t -w --warning\t treat ICY issues as warnings.\n");
	exit(NOERROR);
}

/* print version and exit */
void	version(void)
{
	printf("check_ice2 version %s\n", VERSION);
	exit(NOERROR);
}
