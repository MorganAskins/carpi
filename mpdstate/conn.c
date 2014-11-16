/* 
 * state-utils
 * (c) 2005-2009 by Avuton Olrich <avuton@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "libmpdclient.h"
#include "conn.h"

#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT "6600"
#define MPD_TIMEOUT 60

/* stolen from mpc */
static void parse_password(const char *host, int *password_len, int *parsed_len)
{
	/* parse password and host */
	char *ret = strstr(host, "@");
	int len = ret - host;

	if (ret && len == 0) {
		parsed_len++;
	} else if (ret) {
		*password_len = len;
		*parsed_len += len + 1;
	}
}

/* stolen from mpc */
void printErrorAndExit(mpd_Connection * conn)
{
	if (conn->error) {
		fprintf(stderr, "error: %s\n", conn->errorStr);
		exit(EXIT_FAILURE);
	}
}

mpd_Connection *setup_connection(char * host, char * port)
{
	int iport;
	char *test;
	int password_len = 0;
	int parsed_len = 0;
	mpd_Connection *conn;

	if(!host || strlen(host)<1) {
		if ((test = getenv("MPD_HOST")) && test) {
			host = test;
		} else {
			host = DEFAULT_HOST;
		}
	}

	if(!port || strlen(port)<1) {
		if ((test = getenv("MPD_PORT")) && test) {
			port = test;
		} else {
			port = DEFAULT_PORT;
		}
	}

	iport = strtol(port, &test, 10);

	if (iport <= 0 || *test != '\0') {
		fprintf(stderr,
			"MPD_PORT \"%s\" is not a positive integer\n", port);
		exit(EXIT_FAILURE);
	}

	parse_password(host, &password_len, &parsed_len);

	conn = mpd_newConnection(host + parsed_len, iport, (float)MPD_TIMEOUT);

	printErrorAndExit(conn);

	if (password_len) {
		mpd_sendPasswordCommand(conn, &host[password_len]);
		printErrorAndExit(conn);
		mpd_finishCommand(conn);
		printErrorAndExit(conn);
	}

	return conn;
}
