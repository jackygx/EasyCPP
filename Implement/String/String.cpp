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

#include <regex>
#include <string.h>

#include <EasyCpp.hpp>

bool CString::IsNum(uint32_t &offset) const
{
	int size = (int)GetSize() - (int)offset;
	if (size <= 0)
		return false;

	const char *buf = GetPtr() + offset;

	if (1 == size) {
		char ch = buf[0];
		if (ch <= '9' && ch >= '0') {
			++offset;
			return true;
		} else {
			return false;
		}
	}

	if ('0' == buf[0] && 'x' == buf[1]) {
		return (2 == size) ? false : IsHexSafe(offset);
	} else {
		return IsDecSafe(offset);
	}
}

bool CString::IsDec(uint32_t &offset) const
{
	int size = (int)GetSize() - (int)offset;
	if (size <= 0)
		return false;

	const char *buf = GetPtr();

	if (1 == size) {
		char ch = buf[offset];
		if (ch <= '9' && ch >= '0') {
			++offset;
			return true;
		} else {
			return false;
		}
	}

	return IsDecSafe(offset);
}

bool CString::IsDecSafe(uint32_t &offset) const
{
	const char *buf = GetPtr();

	char ch = buf[offset];
	if ((ch > '9' || ch < '0') && (ch != '-'))
		return false;

	for (++offset; offset < GetSize(); ++offset) {
		char ch = buf[offset];
		if (ch > '9' || ch < '0')
			return false;
	}

	return true;
}

bool CString::IsHexSafe(uint32_t &offset) const
{
	const char *buf = GetPtr();

	for (offset += 2; offset < GetSize(); ++offset) {
		char ch = buf[offset];
		if ((ch > '9' || ch < '0') &&
			(ch < 'A' || ch > 'F') &&
			(ch < 'a' || ch > 'f'))
			return false;
	}

	return true;
}

