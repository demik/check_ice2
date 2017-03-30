/*
 * check_ice: Nagios plugin to check if a ICE or Shoucast stream is up.
 * Usage:check_ice -H host -p port [-m mount_point] Example: check_ice -H
 * stream.icecast.org -p 8000 -m /foobar
 * 
 * Much of the socket code is stolen from:
 * http://pont.net/socket/prog/tcpClient.c Put up by Frederic Pont at
 * fred@pont.net
 * 
 * The rest of it is copyright 2008 Tim Pozar
 * 
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "main.h"

#define LF  10
#define BUF_SIZE 4096

/* prototypes */
static inline int       check_code(char *line);
static inline int       check_icy_name(char *str);

/*
 * main function
 * return/exit values are based on the POSIX spec of returning a positive value :
 *
 * 0: OK
 * 1: WARNING
 * 2: ERROR
 * 3: UNKNOWN
 */
int	main(int argc, char *argv[])
{
	int	r;	/* return code */
	int	s;	/* socket file descriptor */

	char	buff[BUFF_SIZE];
	ssize_t	len;

	char	*line;
	char	*content_type = NULL;
	char 	*soft;
	char 	*icy_name;


	/* check args */
	if (checkopt(&argc, &argv))
		fprintf(stderr, "[-] warning: trailing arguments\n");

	s = http_connect();
	if (s == -1)
		exit(UNKNOWN);
	http_request(s);

	len = read(s, buff, BUFF_SIZE - 1);
	close(s);

	if (len < 0) {
		perror("[-] read error from server");
		exit(UNKNOWN);
	}

	buff[len - 1] = '\0';

	/* HTTP/1.0 302 OK <- minimum iresponse string */
	if (len < 14) {
		fprintf(stderr, "[-] incomplete response received\n");
		exit(ERROR);
	}

	for (line = strtok(buff, "\r\n"); line; line = strtok(NULL, "\r\n")) {
		if (strncmp(line, "Content-Type: ", 14) == 0) {
			content_type = line + 14;
			continue ;
		}

		if (strncmp(line, "icy-name:", 9) == 0) {
			icy_name = line + 9;
			if (g_mode & VERBOSE)
				printf("[+] ICY Name: %s\n", icy_name);
			continue ;
		}

		if (strncmp(line, "Content-Type: ", 14) == 0) {
			content_type = line + 14;
			if (g_mode & VERBOSE)
				printf("[+] Content_type: %s\n", content_type);
			continue ;
		}

		if (strncmp(line, "Server: ", 8) == 0) {
			soft = line + 8;
			if (g_mode & VERBOSE)
				printf("[+] Remote server: %s\n", soft);
			continue ;
		}

		if (strncmp(line, "HTTP/", 5) == 0) {
			r = check_code(line);	
		}
	}

	if (r > 0)
		exit(r);

	/* check only ICY metadata of no previous errors */
	if (g_name != NULL)
		r = check_icy_name(icy_name);
	
	if (r > 0)
		printf("Stream is up on %s, but icy-name doesn't match: %s.\n", g_mount, icy_name);
	else
		printf("Stream is up on %s (name: %s, type: %s).\n", g_mount, icy_name, content_type);
	exit(r);
}

static inline int	check_code(char *line)
{
	int	code = -1;

	/* avoid crappy chained strstr and strcmpi mixed in if else... */
	/* HTTP/1.0 200 OK */
	if (strlen(line) < 13) {
		fprintf(stderr, "[-] incomplete response received\n");
		exit(ERROR);
	}

	line[12] = '\0';
	code = atoi(line + 9);

	switch(code) {
		case 200:
		case 206:
			return NOERROR;
		case 400:
			printf("Stream at: \"http://%s:%s%s\" is full.\n", g_host, g_port, g_mount);
			return ERROR;
		case 401:
		case 403:
		case 404:
		case 416:
			printf("Stream at: \"http://%s:%s%s\" was not found.\n", g_host, g_port, g_mount);
			return ERROR;
		case 500:
			printf("Stream at: \"http://%s:%s%s\" encountered a server error.\n", g_host, g_port, g_mount);
			return ERROR;
		case 503:
			printf("Stream at: \"http://%s:%s%s\" was not avaible.\n", g_host, g_port, g_mount);
			return ERROR;
		default:	
			printf("Unknown state for stream at: \"http://%s:%s%s\"\n", g_host, g_port, g_mount);
			return UNKNOWN;
	}
}

static inline int	check_icy_name(char *str)
{
	if (strcmp(str, g_name) == 0) {
		return NOERROR;
	}
	else {
		if (g_mode & ICYWARN)
			return WARNING;
		else
			return ERROR;
	}
}
