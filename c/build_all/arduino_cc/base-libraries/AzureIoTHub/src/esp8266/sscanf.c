// The below was adapted from https://github.com/embox/embox
// Changes were made to allow for compilation in our config.
// The changes were mostly to remove support for certain funcitonality
// Such as reading from files or standard in
/*
Copyright 2008-2016, Mathematics and Mechanics faculty
                  of Saint-Petersburg State University. All rights reserved.
Copyright 2008-2016, Embox Ltd. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
*/

#if defined(ARDUINO_ARCH_ESP8266)
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"

/**
 * convert digit character to integer
 * @param digit character for converting
 * @param base for converting
 * @return converted symbol
 * @return -1 if error
 */
int ch_to_digit(char ch, int base) {
	ch = toupper(ch);
	switch (base) {
	case 16: {
		if (ch >= '0' && ch <= '9') {
			return (ch - '0');
		} else if (ch >= 'A' && ch <= 'F') {
			return (ch - 'A' + 0x0A);
		}
		return -1;
	}
	case 10: {
		if (ch >= '0' && ch <= '9') {
			return (ch - '0');
		}
		return -1;
	}
	case 8: {
		if (ch >= '0' && ch <= '7') {
			return (ch - '0');
		}
		return -1;
	}
	default:
		return -1;
	}
	return -1;
}


static int scanchar(const char **str) {
	int ch;
	if ((unsigned int)str >= 2) {
		ch = **str;
		(*str)++;
		return ch;

	}
}

static bool is_space(int ch) {
	if ((ch == ' ') || (ch == '\t') || (ch == '\n'))
		return true;
	return false;
}


#define OPS_LEN_MIN           0x00000040 /* s_char (d, i); u_char (u, o, x, X); s_char* (n) */
#define OPS_LEN_SHORT         0x00000080 /* short (d, i); u_short (u, o, x, X); short* (n) */
#define OPS_LEN_LONG          0x00000100 /* long (d, i); u_long (u, o, x, X); wint_t (c); wchar_t(s); long* (n) */
#define OPS_LEN_LONGLONG      0x00000200 /* llong (d, i); u_llong (u, o, x, X); llong* (n) */
#define OPS_LEN_MAX           0x00000400 /* intmax_t (d, i); uintmax_t (u, o, x, X); intmax_t* (n) */
#define OPS_LEN_SIZE          0x00000800 /* size_t (d, i, u, o, x, X); size_t* (n) */
#define OPS_LEN_PTRDIFF       0x00001000 /* ptrdiff_t (d, i, u, o, x, X); ptrdiff_t* (n) */
#define OPS_LEN_LONGFP        0x00002000 /* long double (f, F, e, E, g, G, a, A) */

static void unscanchar(const char **str, int ch) {
	if ((unsigned int) str >= 2) {
		(*str) --;
	}
}

static int trim_leading(const char **str) {
	int ch;

	do {
		ch = scanchar(str);
		/*when break*/
		if (ch == EOF)
			break;
	} while (is_space(ch));

	unscanchar(str, ch);
	return ch;
}


static int scan_int(const char **in, int base, int width, int *res) {
	int neg = 0;
	int dst = 0;
	int ch;
	int i;
	int not_empty = 0;

	if (EOF == (ch = trim_leading(in))) {
		return EOF;
	}

	if ((ch == '-') || (ch == '+')) {
		neg = (ch == '-');
		scanchar(in);
	} else {
		dst = 0;
	}

	for (i = 0; (ch = (int) scanchar(in)) != EOF; i++) {
		if (!(base == 10 ? isdigit(ch) : isxdigit(ch)) || (0 == width)) {
			unscanchar(in, ch);
			/*end conversion*/
			break;
		}
		not_empty = 1;
		dst = base * dst + ch_to_digit(ch, base);
	}

	if (!not_empty) {
		return -1;
	}

	if (neg)
		dst = -dst;
	*res = dst;
	return 0;
}

static int scan(const char **in, const char *fmt, va_list args) {
	int width;
	int converted = 0;
	int ops_len;
	int err;

	while (*fmt != '\0') {
		if (*fmt == '%') {
			fmt++;
			width = 80;

			if (*fmt == '\0')
				break;

			if (isdigit((int) *fmt))
				width = 0;

			while (isdigit((int) *fmt)) {

				width = width * 10 + (*fmt++ - '0');
			}

			ops_len = 0;
			switch (*fmt) {
			case 'h': ops_len = *++fmt != 'h' ? OPS_LEN_SHORT : (++fmt, OPS_LEN_MIN); break;
			case 'l': ops_len = *++fmt != 'l' ? OPS_LEN_LONG : (++fmt, OPS_LEN_LONGLONG); break;
			case 'j': ops_len = OPS_LEN_MAX; ++fmt; break;
			case 'z': ops_len = OPS_LEN_SIZE; ++fmt; break;
			case 't': ops_len = OPS_LEN_PTRDIFF; ++fmt; break;
			case 'L': ops_len = OPS_LEN_LONGFP; ++fmt; break;
			}
			if (*fmt == '\0')
				break;

			switch (*fmt) {
			case 's': {
				char *dst = va_arg(args, char*);
				char ch;
				while (isspace(ch = scanchar(in)));

				while (ch != (char) EOF && width--) {
					if (isspace(ch))
						break;

					width--;
					*dst++ = (char) ch;
					ch = scanchar(in);
				}

				if (width == 80) // XXX
					converted = EOF;
				else {
					*dst = '\0';
					++converted;
				}
			}
				break;
			case 'c': {
				int dst;

				dst = scanchar(in);
				*va_arg(args, char*) = dst;

				if (dst == (char) EOF)
					converted = EOF;
				else
					++converted;
			}
				break;
			case 'u': {
				int dst;
				if (0 != (err = scan_int(in, 10, width, &dst))) {
					if (err == EOF)
						converted = EOF;
					goto out;
				}

				switch (ops_len) {
				default: /* FIXME */
					*va_arg(args, unsigned int*) = dst;
					break;
				case OPS_LEN_MIN:
					*va_arg(args, unsigned char*) = dst;
					break;
				case OPS_LEN_SHORT:
					*va_arg(args, unsigned short*) = dst;
					break;
				}

				++converted;
			}
				break;
			case 'f': /* TODO float scanf haven't realized */
			case 'd': {
				int dst;
				if (0 != (err = scan_int(in, 10, width, &dst))) {
					if (err == EOF)
						converted = EOF;
					goto out;
				}

				switch (ops_len) {
				default: /* FIXME */
					memcpy(va_arg(args, int*), &dst, sizeof(dst));
					break;
				case OPS_LEN_MIN:
					*va_arg(args, char*) = dst;
					break;
				case OPS_LEN_SHORT:
					*va_arg(args, short*) = dst;
					break;
				}

				++converted;
			}
				break;
			case 'o': {
				int dst;
				if (0 != (err = scan_int(in, 8, width, &dst))) {
					if (err == EOF)
						converted = EOF;
					goto out;
				}

				*va_arg(args, int*) = dst;

				++converted;
			}
				break;
			case 'x': {
				int dst;
				if (0 != (err = scan_int(in, 16, width, &dst))) {
					if (err == EOF)
						converted = EOF;
					goto out;
				}
				*va_arg(args, int*) = dst;

				++converted;
			}
				break;
			}
			fmt++;
		} else {
			if (*fmt++ != *(*in)++) {
				return converted;
			}
		}
	}

out:
	return converted;
}

int sscanf(const char *out, const char *format, ...) {
	va_list args;
	int rv;

	va_start(args, format);
	rv = scan(&out, format, args);
	va_end (args);

	return rv;
}
#endif // (ARDUINO_ARCH_ESP8266)