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

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "state.h"

char *get_home_dir(void)
{
	struct passwd *pw = getpwuid(getuid());

	if (pw) {
		return pw->pw_dir;
	} else {
		char *s = getenv("HOME");
		if (!s) {
			printf
			    ("Failed to get home directory from the password\n"
			     "database or from environment variable! exiting...");
			exit(30);
		}
		return s;
	}
}
