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

#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define PROGRAM_NAME    "unfold"
#define PROGRAM_VERSION "0.2.0"

#define BUFFER_SIZE 4096

static int print_delim = 0;
static int unfolded = 0;

static void unfold(int fd, int delim)
{
	char buf[BUFFER_SIZE];
	char *s;
	ssize_t bytes;

	while ((bytes = read(fd, buf, BUFFER_SIZE - 1)) > 0) {
		if (print_delim) {
			write(STDOUT_FILENO, &delim, 1);
			print_delim = 0;
		}

		buf[bytes] = '\0';
		/*
		 * Clear the last newline read in case it is the newline at the
		 * end of the input stream, which shouldn't be replaced with a
		 * delimiter.
		 */
		if (buf[bytes - 1] == '\n') {
			buf[--bytes] = '\0';
			print_delim = 1;
		}

		s = buf;
		while ((s = strchr(s, '\n')))
			*s = delim;

		write(STDOUT_FILENO, buf, bytes);
	}
	if (bytes == -1)
		perror("read");
	else
		unfolded = 1;
}

static int open_file(const char *path)
{
	struct stat sb;
	int fd;

	if (stat(path, &sb) != 0) {
		perror(path);
		return -1;
	}
	if (!S_ISREG(sb.st_mode)) {
		fprintf(stderr, "%s: not a regular file\n", path);
		return -1;
	}
	if ((fd = open(path, O_RDONLY)) == -1) {
		perror(path);
		return -1;
	}

	return fd;
}

static void help(FILE *f, const char *name)
{
	fprintf(f, "usage: %s [OPTION]... [FILE]...\n", name);
	fprintf(f, "Join input lines and print to standard output\n\n");
	fprintf(f, "    -d, --delimiter=DELIM\n\n");
	fprintf(f, "        use DELIM instead of space to replace newlines\n");
	fprintf(f, "    -h, --help\n");
	fprintf(f, "        display this help text and exit\n");
	fprintf(f, "    -n, --no-newline\n");
	fprintf(f, "        do not output a trailing newline\n");
	fprintf(f, "    -S, --separate-files\n");
	fprintf(f, "        don't join lines from different files\n");
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
	int c, delim, filebreaks, newline, status, fd;

	static struct option long_opts[] = {
		{ "delimiter", required_argument, 0, 'd' },
		{ "help", no_argument, 0, 'h' },
		{ "no-newline", no_argument, 0, 'n' },
		{ "separate-files", no_argument, 0, 'S' },
		{ "version", no_argument, 0, 'v' },
		{ 0, 0, 0, 0 }
	};

	delim = ' ';
	filebreaks = 0;
	newline = 1;
	status = EXIT_SUCCESS;

	while ((c = getopt_long(argc, argv, "d:hnSv", long_opts, NULL))
	       != EOF) {
		switch (c) {
		case 'd':
			delim = optarg[0];
			break;
		case 'h':
			help(stdout, PROGRAM_NAME);
			exit(0);
		case 'n':
			newline = 0;
			break;
		case 'S':
			filebreaks = 1;
			break;
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
		unfold(STDIN_FILENO, delim);
	} else {
		for (; optind < argc; ++optind) {
			if ((fd = open_file(argv[optind])) == -1)
				continue;
			unfold(fd, delim);
			if (filebreaks && optind < argc - 1) {
				putchar('\n');
				print_delim = 0;
			}
		}
	}

	if (unfolded && newline)
		putchar('\n');

	return status;
}
