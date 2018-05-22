#include <EasyCpp.hpp>

static const uint8_t base64_eidx[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"	/* 0 - 25 */
	"abcdefghijklmnopqrstuvwxyz"	/* 26 - 51 */
	"0123456789+/";					/* 52 - 63 */

static const uint8_t base64_didx[] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,		/* 0 - 9 */
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,		/* 10 - 19 */
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,		/* 20 - 29 */
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,		/* 30 - 39 */
	0xFF, 0xFF, 0xFF,   62, 0xFF, 0xFF, 0xFF,   63,   52,   53,		/* 40, 41, 42, '+', 44, 45, 46, '/' '0' '1' */
	  54,   55,   56,   57,   58,   59,   60,   61, 0xFF, 0xFF,		/* '2' - '9', 58, 59 */
	0xFF,    0, 0xFF, 0xFF, 0xFF,    0,    1,    2,    3,    4,		/* 60, '=', 62, 63, 64, 'A' - 'E' */
	   5,    6,    7,    8,    9,   10,   11,   12,   13,   14,		/* 'F' - 'O' */
	  15,   16,   17,   18,   19,   20,   21,   22,   23,   24,		/* 'P' - 'Y' */
	  25, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,   26,   27,   28,		/* 'X', 91 - 96, 'a', 'b', 'c' */
	  29,   30,   31,   32,   33,   34,   35,   36,   37,   38,		/* 'd' - 'm' */
	  39,   40,   41,   42,   43,   44,   45,   46,   47,   48,		/* 'n' - 'w' */
	  49,   50,   51};												/* 'x', 'y', 'z' */

CStringPtr CString::Base64Encode(void) const
{
	CStringPtr out(STR(GetSize() << 1));

	const char *src = GetPtr();
	char *dst = out;
	uint32_t size = 0;
	uint32_t i;

	for (i = 0; i < GetSize() - 2;) {
		uint32_t tmp = (src[i++] << 16);
		tmp |= (src[i++] << 8);
		tmp |= src[i++];

		dst[size++] = base64_eidx[(tmp >> 18) & 0x3F];
		dst[size++] = base64_eidx[(tmp >> 12) & 0x3F];
		dst[size++] = base64_eidx[(tmp >> 6) & 0x3F];
		dst[size++] = base64_eidx[tmp & 0x3F];
	}

	if (i == (GetSize() - 2)) {
		uint32_t tmp = (src[i++] << 16);
		tmp |= (src[i++] << 8);

		dst[size++] = base64_eidx[(tmp >> 18) & 0x3F];
		dst[size++] = base64_eidx[(tmp >> 12) & 0x3F];
		dst[size++] = base64_eidx[(tmp >> 6) & 0x3F];
		dst[size++] = '=';
	} else if (i == (GetSize() - 1)) {
		uint32_t tmp = (src[i++] << 16);

		dst[size++] = base64_eidx[(tmp >> 18) & 0x3F];
		dst[size++] = base64_eidx[(tmp >> 12) & 0x3F];
		dst[size++] = '=';
		dst[size++] = '=';
	}

	dst[size++] = '\0';
	out->SetSize(size);
	return out;
}

CStringPtr CString::Base64Decode(void) const
{
	CStringPtr out(STR(GetSize()));

	const char *src = GetPtr();
	char *dst = out;
	uint32_t size = 0;
	uint32_t i;

	for (i = 0; i< GetSize() - 3;) {
		uint8_t a = src[i++];
		uint8_t b = src[i++];
		uint8_t c = src[i++];
		uint8_t d = src[i++];
		uint8_t _a = base64_didx[a];
		uint8_t _b = base64_didx[b];
		uint8_t _c = base64_didx[c];
		uint8_t _d = base64_didx[d];

		if (0xFF == _a || 0xFF == _b || 0xFF == _c || 0xFF == _d) {
			return nullptr;
		}

		unsigned tmp = (_a << 18) | (_b << 12) | (_c << 6) | _d;

		dst[size++] = (tmp >> 16) & 0xFF;
		if ('=' != c) {
			dst[size++] = (tmp >> 8) & 0xFF;

			if ('=' != d) {
				dst[size++] = tmp & 0xFF;
			}
		}
	}

	dst[size++] = '\0';
	out->SetSize(size);
	return out;
}

