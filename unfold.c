/*
 * unfold.c
 * Copyright (C) 2017 Alexei Frolov
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
#include <sys/stat.h>
#include <unistd.h>

#define PROGRAM_NAME    "unfold"
#define PROGRAM_VERSION "0.2.0"

#define BUFFER_SIZE 4096

static int print = 0;
static int unfolded = 0;

static void unfold(FILE *f, int delim)
{
	char buf[BUFFER_SIZE];
	char *s;

	unfolded = 1;
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

static FILE *open_file(const char *path)
{
	struct stat sb;
	FILE *f;

	if (stat(path, &sb) != 0) {
		perror(path);
		return NULL;
	}
	if (!S_ISREG(sb.st_mode)) {
		fprintf(stderr, "%s: not a regular file\n", path);
		return NULL;
	}
	if (!(f = fopen(path, "r"))) {
		perror(path);
		return NULL;
	}

	return f;
}

static void help(FILE *f, const char *name)
{
	fprintf(f, "usage: %s [OPTION]... [FILE]...\n", name);
	fprintf(f, "Join input lines and print to standard output\n\n");
	fprintf(f, "    -d, --delimiter=DELIM\n\n");
	fprintf(f, "        use DELIM instead of space to replace newlines\n");
	fprintf(f, "    -h, --help\n");
	fprintf(f, "        display this help text and exit\n");
	fprintf(f, "    -v, --version\n");
	fprintf(f, "        display version information and exit\n");
}

static void version(FILE *f, const char *name)
{
	fprintf(f, "%s %s\n", name, PROGRAM_VERSION);
	fprintf(f, "Copyright (C) 2017 Alexei Frolov\n");
	fprintf(f, "This program is distributed as free software under the\n");
	fprintf(f, "terms of the GNU General Public License, version 3.\n");
}

int main(int argc, char **argv)
{
	int c, delim, status;
	FILE *f;

	static struct option long_opts[] = {
		{ "delimiter", required_argument, 0, 'd' },
		{ "help", no_argument, 0, 'h' },
		{ "version", no_argument, 0, 'v' },
		{ 0, 0, 0, 0 }
	};

	delim = ' ';
	status = EXIT_SUCCESS;

	while ((c = getopt_long(argc, argv, "d:hv", long_opts, NULL)) != EOF) {
		switch (c) {
		case 'd':
			delim = optarg[0];
			break;
		case 'h':
			help(stdout, PROGRAM_NAME);
			exit(0);
		case 'v':
			version(stdout, PROGRAM_NAME);
			exit(0);
		case '?':
		default:
			help(stderr, argv[0]);
			exit(1);
		}
	}

	if (optind == argc) {
		unfold(stdin, delim);
	} else {
		for (; optind < argc; ++optind) {
			if (!(f = open_file(argv[optind])))
				continue;
			unfold(f, delim);
		}
	}

	if (unfolded)
		putchar('\n');

	return status;
}
