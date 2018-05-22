/*
 * Copyright (c) 2018 Guo Xiang
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __STRING_HELP_HPP__
#define __STRING_HELP_HPP__

#include "StringHeader.hpp"

namespace String {

const static uint8_t HexMap[] = {
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

template <class T, ENABLE_IFEQ(sizeof(T), 1)>
inline CStringPtr HEX(const T &t, uint32_t align = 0, char padding = ' ')
{
	CStringParam p(CStringParam::CT_BYTE, CStringParam::CM_HEX,
				   (uint8_t)t, align, padding);

	CString q(CStringParam::CT_BYTE, CStringParam::CM_HEX,
				   (uint8_t)t, align, padding);

	return CStringPtr(CStringParam::CT_BYTE, CStringParam::CM_HEX,
					  (uint8_t)t, align, padding);
}

template <class T, ENABLE_IFEQ(sizeof(T), 2)>
inline CStringPtr HEX(const T &t, uint32_t align = 0, char padding = ' ')
{
	return CStringPtr(CStringParam::CT_SHORT, CStringParam::CM_HEX,
					  (uint16_t)t, align, padding);
}

template <class T, ENABLE_IFEQ(sizeof(T), 4)>
inline CStringPtr HEX(const T &t, uint32_t align = 0, char padding = ' ')
{
	return CStringPtr(CStringParam::CT_WORD, CStringParam::CM_HEX,
					  (uint32_t)t, align, padding);
}

template <class T, ENABLE_IFEQ(sizeof(T), 8)>
inline CStringPtr HEX(const T &t, uint32_t align = 0, char padding = ' ')
{
	return CStringPtr(CStringParam::CT_DWORD, CStringParam::CM_HEX,
					  (uint64_t)t, align, padding);
}

template <class T, ENABLE_IFEQ(sizeof(T), 1)>
inline CStringPtr DEC(T t, uint32_t align = 0, char padding = ' ')
{
	return CStringPtr(CStringParam::CT_BYTE, CStringParam::CM_DEC,
					  t, align, padding);
}

template <class T, ENABLE_IFEQ(sizeof(T), 2)>
inline CStringPtr DEC(T t, uint32_t align = 0, char padding = ' ')
{
	return CStringPtr(CStringParam::CT_SHORT, CStringParam::CM_DEC,
					  t, align, padding);
}

template <class T, ENABLE_IFEQ(sizeof(T), 4)>
inline CStringPtr DEC(T t, uint32_t align = 0, char padding = ' ')
{
	return CStringPtr(CStringParam::CT_WORD, CStringParam::CM_DEC,
					  t, align, padding);
}

template <class T, ENABLE_IFEQ(sizeof(T), 8)>
inline CStringPtr DEC(T t, uint32_t align = 0, char padding = ' ')
{
	return CStringPtr(CStringParam::CT_DWORD, CStringParam::CM_DEC,
					  t, align, padding);
}

template <class T, ENABLE_IFEQ(sizeof(T), 1)>
inline CStringPtr CHAR(T t, uint32_t align = 0, char padding = ' ')
{
	return CStringPtr(CStringParam::CT_BYTE, CStringParam::CM_CHAR,
					  t, align, padding);
}

inline CStringPtr STR(uint32_t size, uint32_t offset = 0)
{
	CStringParam::CStringCapacity cap = {size};
	return CStringPtr(cap, offset);
}

} /* namespace String */

namespace std {

template<>
struct less<CConstStringPtr> {
	bool operator() (const CConstStringPtr &lhs, const CConstStringPtr &rhs) const {
		const char *lbuf = lhs;
		const char *rbuf = rhs;
		int lsize = lhs->GetSize();
		int rsize = rhs->GetSize();

		bool less = lsize < rsize;
		int size = less ? lsize : rsize;

		for (int i = 0; i < size; ++i) {
			if (lbuf[i] != rbuf[i])
				return lbuf[i] < rbuf[i];
		}

		return less;
	}
};

};

using namespace String;

#endif /* __STRING_HELP_HPP__ */
