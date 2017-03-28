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
#include <unistd.h>		/* close */

#define LF  10
#define BUF_SIZE 4096

char		verrev    [BUF_SIZE] = "$Id: check_ice.c,v 1.5 2008/10/07 04:11:13 pozar Exp $";

char		hostname  [BUF_SIZE] = "scfire-dtc-aa01.stream.aol.com";
char		mount     [BUF_SIZE] = "/";
char		send_string[BUF_SIZE] = "";
char		recv_string[BUF_SIZE] = "";
int		portnum = 80;

int
main(int argc, char *argv[])
{
	int		sd        , rc, i;
	struct sockaddr_in localAddr, servAddr;
	struct hostent *h;

	if (argc < 2) {
		banner();
		exit(3);
	}
	/* scan command line arguments, and look for files to work on. */
	for (i = 1; i < argc; i++) {
		switch (argv[i][1]) {	/* be case indepenent... */
		case 'H':	/* Hostname... */
			i++;
			strncpy(hostname, argv[i], BUF_SIZE);
			break;
		case 'p':
			i++;
			if (argv[i]) {
				portnum = atoi(argv[i]);
			}
			break;
		case 'm':
			i++;
			/* We need a slash in front of the mount... */
			if (argv[i]) {
				if ('/' != argv[i][0]) {
					sprintf(mount, "/%s", argv[i]);
				} else {
					strncpy(mount, argv[i], BUF_SIZE);
				}
			}
			break;
		default:
			printf("I don't know the meaning of the command line argument: \"%s\".\n", argv[i]);
			banner();
			exit(3);
		}
	}

	h = gethostbyname(hostname);
	if (h == NULL) {
		printf("%s: unknown host '%s'\n", argv[0], hostname);
		exit(3);
	}
	servAddr.sin_family = h->h_addrtype;
	memcpy((char *)&servAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
	servAddr.sin_port = htons(portnum);

	/* create socket */
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0) {
		perror("cannot open socket ");
		exit(3);
	}
	/* bind any port number */
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localAddr.sin_port = htons(0);

	rc = bind(sd, (struct sockaddr *)&localAddr, sizeof(localAddr));
	if (rc < 0) {
		printf("%s: cannot bind port TCP %u\n", argv[0], portnum);
		perror("error ");
		exit(3);
	}
	/* connect to server */
	rc = connect(sd, (struct sockaddr *)&servAddr, sizeof(servAddr));
	if (rc < 0) {
		perror("cannot connect ");
		exit(2);
	}
	sprintf(send_string, "GET %s HTTP/1.1 Accept: */*\n\n", mount);

	rc = send(sd, send_string, strlen(send_string) + 1, 0);

	if (rc < 0) {
		perror("cannot send data ");
		close(sd);
		exit(2);
	}
	/* rc = recv(sd, recv_string, BUF_SIZE, 0); */
	readline(sd, recv_string);

	if (rc < 0) {
		perror("cannot recv data ");
		close(sd);
		exit(2);
	}
	if (rc == 0) {
		printf("Got zero bytes back.\n");
		exit(2);
	} else {
		if (0 < strstr(recv_string, " 200 ")) {
			printf("Stream at: \"http://%s:%i%s\" is up.\n", hostname, portnum, mount);
			exit(0);
		}
		if (0 < strstr(recv_string, " 206 ")) {
			printf("Stream at: \"http://%s:%i%s\" is up but only partially.\n", hostname, portnum, mount);
			exit(1);
		}
		if (0 < strstr(recv_string, " 400 ")) {
			printf("Stream at: \"http://%s:%i%s\" is full.\n", hostname, portnum, mount);
			exit(2);
		}
		if (0 < strstr(recv_string, " 401 ")) {
			printf("Stream at: \"http://%s:%i%s\" was not found.\n", hostname, portnum, mount);
			exit(2);
		}
		if (0 < strstr(recv_string, " 403 ")) {
			printf("Stream at: \"http://%s:%i%s\" was not found.\n", hostname, portnum, mount);
			exit(2);
		}
		if (0 < strstr(recv_string, " 404 ")) {
			printf("Stream at: \"http://%s:%i%s\" was not found.\n", hostname, portnum, mount);
			exit(2);
		}
		if (0 < strstr(recv_string, " 416 ")) {
			printf("Stream at: \"http://%s:%i%s\" was not found.\n", hostname, portnum, mount);
			exit(2);
		}
		if (0 < strstr(recv_string, " 500 ")) {
			printf("Stream at: \"http://%s:%i%s\" encountered a server error.\n", hostname, portnum, mount);
			exit(2);
		}
		if (0 < strstr(recv_string, " 503 ")) {
			printf("Stream at: \"http://%s:%i%s\" was not found.\n", hostname, portnum, mount);
			exit(2);
		}
	}
	/* We really shouldn't get this far... */
	printf("Unknown state for stream at: \"http://%s:%i%s\"\n", hostname, portnum, mount);
	exit(3);
}

readline(fip, buffer)
	int		fip;
	char           *buffer;
{
	int		i = 0;
	char		c;
	int		ret;

	bzero(buffer, BUF_SIZE);
	while (i < BUF_SIZE) {
		ret = read(fip, &c, sizeof(char));
		if (ret < 1)
			break;
		if (c == LF)
			break;
		buffer[i] = c;
		i++;
	}
	return (i);
}


banner()
{
	printf("check_ice: Nagios plugin to check if a ICE or Shoucast stream is up.\n");
	printf("Usage:check_ice -H host -p port [-m mount_point]\n");
	printf("  Example: check_ice -H stream.icecast.org -p 8000 -m /foobar\n");
	return;
}
