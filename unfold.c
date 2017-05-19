/*
 * unfold.c
 * Copyright (C) 2016-2017 Alexei Frolov
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PROGRAM_NAME "unfold"

#define BUFFER_SIZE 4096

static int print = 0;

static void unfold(FILE *f, int delim)
{
	char buf[BUFFER_SIZE];
	char *s;

	while (fgets(buf, BUFFER_SIZE, f)) {
		if (print) {
			putchar(delim);
			print = 0;
		}

		if ((s = strchr(buf, '\n'))) {
			*s = '\0';
			print = 1;
		}
		printf("%s", buf);
	}
}

static void usage(FILE *f, const char *name)
{
	fprintf(f, "usage: %s [FILE...]\n", name);
}

int main(int argc, char **argv)
{
	int c, delim;

	static struct option long_opts[] = {
		{ "delimiter", required_argument, 0, 'd' },
		{ "help", no_argument, 0, 'h' },
		{ "version", no_argument, 0, 'v' },
		{ 0, 0, 0, 0 }
	};

	delim = ' ';

	while ((c = getopt_long(argc, argv, "d:cv", long_opts, NULL)) != EOF) {
		switch (c) {
		case 'd':
			delim = optarg[0];
			break;
		case '?':
			usage(stderr, argv[0]);
			exit(1);
		}
	}

	if (optind == argc)
		unfold(stdin, delim);

	putchar('\n');
	return 0;
}