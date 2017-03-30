/*
 *  http.c
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
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <openssl/rc4.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>
#include "main.h"

/*
 * Open a socket to the Icecast/Shoutcast sever and return the fd.
 * - return -1 on error
 */

int	http_connect()
{
	int                     error, s;
	struct addrinfo         hints, *res, *res0;

	/*
	 * initialise hints
	 * AF_UNSPEC is for IPv4 and IPv6
	 */

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((error = getaddrinfo(g_host, g_port, &hints, &res0))) {
		fprintf(stderr, "[-] host not found: %s (%s)\n",
				g_host, gai_strerror(error));
		return -1;
	}

	if (g_mode & VERBOSE)
		printf("[+] trying to connect to %s:%s...\n", g_host, g_port);

	s = -1;
	for (res = res0; res; res = res->ai_next) {
		s = socket(res->ai_family, res->ai_socktype,
				res->ai_protocol);
		if (s < 0) {
			continue;
		}

		if (connect(s, res->ai_addr, res->ai_addrlen) < 0) {
			close(s);
			s = -1;
			continue;
		}

		break;
	}

	if (s < 0) {
		fprintf(stderr, "[-] cannot connect to server: %s\n", strerror(errno));
		return s;
	}

	if (! (g_mode & VERBOSE))
		return s;

	/* find IP for Verbose */
	switch (res->ai_family) {
		case AF_INET: {
	      		char ntop[INET_ADDRSTRLEN];

		      	if (inet_ntop(res->ai_family, &((struct sockaddr_in *)res->ai_addr)->sin_addr, \
						ntop, INET_ADDRSTRLEN) == NULL) {
				fprintf(stderr, "[-] inet_ntop() error: %s\n", strerror(errno));
			      	break;
		      	}

			printf("[+] connected to host [%s]:%s\n", ntop, g_port);
			break;
	      	}
		case AF_INET6: {
			char ntop[INET6_ADDRSTRLEN];

		       	if (inet_ntop(res->ai_family, &((struct sockaddr_in6 *)res->ai_addr)->sin6_addr, \
		       	              ntop, INET6_ADDRSTRLEN) == NULL) {
		   		fprintf(stderr, "[-] inet_ntop() error: %s\n", strerror(errno));
				break;
			}

			printf("[+] connected to host [%s]:%s\n", ntop, g_port);
			break;
		}
		default:
			fprintf(stderr, "[n] getaddrinfo() error: %s\n", strerror(EAFNOSUPPORT));
	}

	return s;
}

void	http_request(int socket)
{
	int	len;
	char	*request;
	ssize_t	sent;

	len = asprintf(&request, "GET %s HTTP/1.0 Accept: */*\nUser-Agent: check_ice2 %s\n\n", g_mount, VERSION);

	/* memory allocation error */
	if (len == -1) {
		fprintf(stderr, "[h] asprintf() error: %s\n", strerror(errno));
		close(socket);
		exit(ERROR);
	}

	sent = send(socket, request, len + 1, 0);
	free(request);

	if (sent < 0) {
		fprintf(stderr, "[h] send() error: %s\n", strerror(errno));
		close(socket);
		exit(ERROR);
	}
}
