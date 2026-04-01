#include <utils/vsprintf.h>

#include <utils/string.h>
#include <stdarg.h>
#include <stdbool.h>

/* Flags */
#define FLAG_LEFT   (1 << 0)   /* '-': left-align */
#define FLAG_PLUS   (1 << 1)   /* '+': force sign */
#define FLAG_SPACE  (1 << 2)   /* ' ': space before positive number */
#define FLAG_ZERO   (1 << 3)   /* '0': zero-pad */

/* Length modifiers */
#define LEN_INT  0
#define LEN_LONG 1

/*
 * Convert unsigned integer to string in the given base.
 * Returns number of characters written (no NUL terminator).
 */
static int num_to_str(unsigned long x, int base, int upper, char* out) {
	const char* digits_lower = "0123456789abcdef";
	const char* digits_upper = "0123456789ABCDEF";
	const char* digits = upper ? digits_upper : digits_lower;
	char tmp[65];
	char* p = tmp + 64;
	*p = '\0';
	if (x == 0) {
		*--p = '0';
	} else {
		while (x) {
			*--p = digits[x % (unsigned)base];
			x /= (unsigned)base;
		}
	}
	int len = (int)((tmp + 64) - p);
	memcpy(out, p, (unsigned int)len);
	return len;
}

/*
 * Write a formatted number field to out.
 * prefix/pfxlen: sign or "0x" prefix (placed inside zero-padding).
 * prec_zeros:    leading zeros required by precision.
 * digits/diglen: the numeric digits.
 * width:         minimum field width.
 * flags:         FLAG_* bits.
 * fill:          padding character (' ' or '0').
 */
static int write_num(char* out,
                     const char* prefix, int pfxlen,
                     int prec_zeros,
                     const char* digits, int diglen,
                     int width, int flags, char fill) {
	int content = pfxlen + prec_zeros + diglen;
	int pad = (width > content) ? width - content : 0;
	char* b = out;

	if (flags & FLAG_LEFT) {
		memcpy(b, prefix, (unsigned int)pfxlen);
		b += pfxlen;
		for (int i = 0; i < prec_zeros; i++) {
			*b++ = '0';
		}
		memcpy(b, digits, (unsigned int)diglen);
		b += diglen;
		for (int i = 0; i < pad; i++) {
			*b++ = ' ';
		}
	} else if (fill == '0') {
		/* sign/prefix first, then zero-pad, then digits */
		memcpy(b, prefix, (unsigned int)pfxlen);
		b += pfxlen;
		for (int i = 0; i < pad; i++) {
			*b++ = '0';
		}
		for (int i = 0; i < prec_zeros; i++) {
			*b++ = '0';
		}
		memcpy(b, digits, (unsigned int)diglen);
		b += diglen;
	} else {
		/* space-pad first, then prefix, then digits */
		for (int i = 0; i < pad; i++) {
			*b++ = ' ';
		}
		memcpy(b, prefix, (unsigned int)pfxlen);
		b += pfxlen;
		for (int i = 0; i < prec_zeros; i++) {
			*b++ = '0';
		}
		memcpy(b, digits, (unsigned int)diglen);
		b += diglen;
	}
	return (int)(b - out);
}

int vsprintf(char* buf, const char* fmt, va_list args) {
	char* out = buf;

	while (*fmt) {
		if (*fmt != '%') {
			*out++ = *fmt++;
			continue;
		}
		fmt++;

		if (*fmt == '\0') {
			break;
		}

		/* --- flags --- */
		int flags = 0;
		while (true) {
			if (*fmt == '-') {
				flags |= FLAG_LEFT;
				fmt++;
			} else if (*fmt == '+') {
				flags |= FLAG_PLUS;
				fmt++;
			} else if (*fmt == ' ') {
				flags |= FLAG_SPACE;
				fmt++;
			} else if (*fmt == '0') {
				flags |= FLAG_ZERO;
				fmt++;
			} else {
				break;
			}
		}
		/* '-' overrides '0' */
		if (flags & FLAG_LEFT) {
			flags &= ~FLAG_ZERO;
		}

		/* --- width --- */
		int width = 0;
		if (*fmt == '*') {
			width = va_arg(args, int);
			if (width < 0) {
				flags |= FLAG_LEFT;
				flags &= ~FLAG_ZERO;
				width = -width;
			}
			fmt++;
		} else {
			while (*fmt >= '0' && *fmt <= '9') {
				width = width * 10 + (*fmt++ - '0');
			}
		}

		/* --- precision --- */
		int prec = -1;
		if (*fmt == '.') {
			fmt++;
			prec = 0;
			if (*fmt == '*') {
				prec = va_arg(args, int);
				if (prec < 0) {
					prec = -1;
				}
				fmt++;
			} else {
				while (*fmt >= '0' && *fmt <= '9') {
					prec = prec * 10 + (*fmt++ - '0');
				}
			}
		}

		/* --- length modifier --- */
		int len_mod = LEN_INT;
		if (*fmt == 'h') {
			fmt++;
			if (*fmt == 'h') {
				fmt++; /* hh -> treat as int */
			}
		} else if (*fmt == 'l') {
			len_mod = LEN_LONG;
			fmt++;
			if (*fmt == 'l') {
				fmt++; /* ll -> same as l on 32-bit */
			}
		} else if (*fmt == 'z') {
			len_mod = LEN_LONG;
			fmt++;
		}

		char spec = *fmt;
		if (spec == '\0') {
			break;
		}
		fmt++;

		/* --- format specifiers --- */
		switch (spec) {

		case '%':
			*out++ = '%';
			break;

		case 'c': {
			char c = (char)va_arg(args, int);
			int pad = (width > 1) ? width - 1 : 0;
			if (!(flags & FLAG_LEFT)) {
				for (int i = 0; i < pad; i++) {
					*out++ = ' ';
				}
			}
			*out++ = c;
			if (flags & FLAG_LEFT) {
				for (int i = 0; i < pad; i++) {
					*out++ = ' ';
				}
			}
			break;
		}

		case 's': {
			const char* s = va_arg(args, const char*);
			if (!s) {
				s = "(null)";
			}
			int slen = strlen(s);
			if (prec >= 0 && prec < slen) {
				slen = prec;
			}
			int pad = (width > slen) ? width - slen : 0;
			if (!(flags & FLAG_LEFT)) {
				for (int i = 0; i < pad; i++) {
					*out++ = ' ';
				}
			}
			memcpy(out, s, (unsigned int)slen);
			out += slen;
			if (flags & FLAG_LEFT) {
				for (int i = 0; i < pad; i++) {
					*out++ = ' ';
				}
			}
			break;
		}

		case 'd':
		case 'i': {
			long val = (len_mod == LEN_LONG) ? va_arg(args, long) : (long)va_arg(args, int);
			char prefix[2];
			int pfxlen = 0;
			unsigned long uval;
			if (val < 0) {
				prefix[0] = '-';
				pfxlen = 1;
				uval = (unsigned long)(-(val + 1)) + 1UL;
			} else {
				if (flags & FLAG_PLUS) {
					prefix[0] = '+';
					pfxlen = 1;
				} else if (flags & FLAG_SPACE) {
					prefix[0] = ' ';
					pfxlen = 1;
				}
				uval = (unsigned long)val;
			}
			char nbuf[12];
			int nlen = num_to_str(uval, 10, 0, nbuf);
			int prec_zeros = (prec > nlen) ? prec - nlen : 0;
			char fill = ((flags & FLAG_ZERO) && prec < 0) ? '0' : ' ';
			out += write_num(out, prefix, pfxlen, prec_zeros, nbuf, nlen, width, flags, fill);
			break;
		}

		case 'u': {
			unsigned long val = (len_mod == LEN_LONG) ? va_arg(args, unsigned long) : (unsigned long)va_arg(args, unsigned int);
			char nbuf[12];
			int nlen = num_to_str(val, 10, 0, nbuf);
			int prec_zeros = (prec > nlen) ? prec - nlen : 0;
			char fill = ((flags & FLAG_ZERO) && prec < 0) ? '0' : ' ';
			out += write_num(out, "", 0, prec_zeros, nbuf, nlen, width, flags, fill);
			break;
		}

		case 'o': {
			unsigned long val = (len_mod == LEN_LONG) ? va_arg(args, unsigned long) : (unsigned long)va_arg(args, unsigned int);
			char nbuf[12];
			int nlen = num_to_str(val, 8, 0, nbuf);
			int prec_zeros = (prec > nlen) ? prec - nlen : 0;
			char fill = ((flags & FLAG_ZERO) && prec < 0) ? '0' : ' ';
			out += write_num(out, "", 0, prec_zeros, nbuf, nlen, width, flags, fill);
			break;
		}

		case 'x':
		case 'X': {
			unsigned long val = (len_mod == LEN_LONG) ? va_arg(args, unsigned long) : (unsigned long)va_arg(args, unsigned int);
			char nbuf[10];
			int nlen = num_to_str(val, 16, spec == 'X', nbuf);
			int prec_zeros = (prec > nlen) ? prec - nlen : 0;
			char fill = ((flags & FLAG_ZERO) && prec < 0) ? '0' : ' ';
			out += write_num(out, "", 0, prec_zeros, nbuf, nlen, width, flags, fill);
			break;
		}

		case 'p': {
			unsigned long val = va_arg(args, unsigned long);
			char nbuf[10];
			int nlen = num_to_str(val, 16, 0, nbuf);
			int prec_zeros = (prec > nlen) ? prec - nlen : 0;
			char fill = ((flags & FLAG_ZERO) && prec < 0) ? '0' : ' ';
			out += write_num(out, "0x", 2, prec_zeros, nbuf, nlen, width, flags, fill);
			break;
		}

		default:
			*out++ = '%';
			*out++ = spec;
			break;
		}
	}

	*out = '\0';
	return (int)(out - buf);
}