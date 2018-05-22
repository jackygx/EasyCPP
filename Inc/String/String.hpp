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

#ifndef __STRING_HPP__
#define __STRING_HPP__

#include "StringHelp.hpp"
#include "CommonString.hpp"
#include "Json.hpp"

#include <Regex/Regex.hpp>

template <class... Tn,
		 DECLARE_ENABLE_IF(has_constructor<CStringParam, Tn...>)>
inline CString::CString(const Tn &... tn) :
	mData(tn...)
{
	STR_DEBUG("Construct CString from CStringParam");
}

/* Create CString from other CString(s) */
template <class... Tn,
		 DECLARE_ENABLE_IF(!has_constructor<CStringParam, Tn...>),
		 DECLARE_ENABLE_IFS(has_constructor<CConstStringPtr, Tn>)>
inline CString::CString(const Tn &... tn) :
	mData(CStringParam::CStringEmpty())
{
	STR_DEBUG("Construct from multi-CStrings");

	/* Adds one byte for EOS */
	uint32_t size = 1;

	/* Convert Tn... to CConstStringPtr array */
	CConstStringPtr arr[sizeof...(tn)] = {CConstStringPtr(tn)...};

	/* Calculate total size first */
	for (uint32_t i = 0; i < sizeof...(tn); ++i) {
		size += arr[i]->GetSize();
	}

	/* Allocate the memory according to the total size */
	if (size <= DEFAULT_STR_SIZE) {
		mData = CStringParam();
	} else {
		CStringParam::CStringCapacity cap = {size};
		mData = CStringParam(cap);
	}

	/* Doing the copy */
	for (uint32_t i = 0; i < sizeof...(tn); ++i) {
		mData.Append(arr[i]->mData);
	}
}

inline uint32_t CString::GetSize(void) const
{
	return mData.GetSize();
}

inline uint32_t CString::GetCapacity(void) const
{
	return mData.GetCapacity();
}

inline uint32_t CString::GetOffset(void) const
{
	return mData.GetOffset();
}

inline void CString::SetSize(uint32_t size)
{
	mData.SetSize(size);
}

inline void CString::SetOffset(uint32_t offset)
{
	mData.SetOffset(offset);
}

inline char *CString::GetPtr(void)
{
	return mData.GetPtr();
}

inline const char *CString::GetPtr(void) const
{
	return mData.GetPtr();
}

inline uint32_t CString::GetFree(void) const
{
	return mData.GetFree();
}

/* Overload operation */
inline void CString::operator += (const CString &str)
{
	mData.Append(str.mData);
}

inline bool CString::operator == (const CString &str) const
{
	return mData == str.mData;
}

inline char &CString::operator [] (uint32_t i)
{
	CHECK_PARAM(i < GetSize(),
				"i: ", DEC(i),
				", mSize: ", DEC(GetSize()));

	return GetPtr()[i];
}

inline const char &CString::operator [] (uint32_t i) const
{
	CHECK_PARAM(i < GetSize(),
				"i: ", DEC(i),
				", mSize: ", DEC(GetSize()));

	return GetPtr()[i];
}

inline bool CString::operator > (const CString &str) const
{
	return mData.Compare(str.mData) > 0;
}

inline bool CString::operator < (const CString &str) const
{
	return mData.Compare(str.mData) < 0;
}

template <class T>
inline T CString::Dump(uint32_t offset) const
{
	CHECK_PARAM(offset < GetSize(),
				"Illegal offset: ", DEC(offset),
				", mSize: ", DEC(GetSize()));

	return *((T *)(&GetPtr()[offset]));
}

template <typename T,
		 DECLARE_ENABLE_IF(std::is_pointer<T>),
		 DECLARE_ENABLE_IF(!IS_CONST(T))>
inline T CString::Convert(uint32_t offset)
{
	CHECK_PARAM(offset <= GetCapacity(),
				"Type: ", TYPE_NAME(T), " is too big",
				" offset: ", DEC(offset),
				" size: ", DEC(GetCapacity()));

	return (T)(GetPtr() + offset);
}

template <typename T,
		 DECLARE_ENABLE_IF(std::is_pointer<T>),
		 DECLARE_ENABLE_IF(IS_CONST(T))>
inline T CString::Convert(uint32_t offset) const
{
	CHECK_PARAM(offset <= GetCapacity(),
				"Type: ", TYPE_NAME(T), " is too big",
				" offset: ", DEC(offset),
				" size: ", DEC(GetCapacity()));

	return (T)(GetPtr() + offset);
}

inline CStringPtr CString::ToCStr(void) const
{
	CStringPtr str(Slice(0, -1));

	if ('\0' != GetPtr()[GetSize()]) {
		str->mData.CheckAndAlloc(true);
	}

	return str;
}

inline int CString::Sprintf(const char *fmt, ...)
{
	CHECK_PARAM(NULL != fmt, "NULL == fmt");

	va_list ap;

	va_start(ap, fmt);
	SetSize(vsnprintf(GetPtr(), mData.GetCapacity(), fmt, ap));
	va_end(ap);

	return GetSize();
}

inline void CString::Memset(uint32_t offset, uint32_t size, uint8_t val)
{
	CHECK_PARAM(GetCapacity() >= offset + size);

	::memset(GetPtr() + offset, val, size);
	SetSize(size);
}

inline void CString::Memcpy(const CString &str)
{
	Memcpy(str, str.GetOffset(), str.GetSize());
}

inline void CString::Memcpy(const CString &str, uint32_t offset, uint32_t size)
{
	CHECK_PARAM(offset + size < str.GetCapacity(),
				"Offset + size is too large. "
				"Size: ", DEC(size),
				". Offset: ", DEC(offset),
				". String size: ", DEC(str.GetCapacity()));

	::memcpy(GetPtr(), str.GetPtr() + offset, size);
	SetSize(size);
}

inline CStringPtr CString::Reverse(void) const
{
	uint32_t size = GetSize();
	CStringPtr str(STR(size, 0));
	const char *sbuf = GetPtr();
	char *tbuf = str->GetPtr();

	for (uint32_t i = 0; i < size; ++i) {
		tbuf[size - i - 1] = sbuf[i];
	}

	str->SetSize(size);

	return str;
}

inline CStringPtr CString::Strip(void) const
{
	const char *buf = GetPtr();

	for (uint32_t i = 0; i < GetSize(); ++i) {
		if (buf[i] != ' ' &&
			buf[i] != '\t' &&
			buf[i] != '\r' &&
			buf[i] != '\n') {
			for (uint32_t j = GetSize() - 1; j < 0x80000000; --j) {
				if (buf[j] != ' ' &&
					buf[j] != '\t' &&
					buf[j] != '\r' &&
					buf[j] != '\n') {
					return Slice(i, j + 1);
				}
			}
		}
	}

	return Slice(0, 0);
}

inline CStringPtr CString::LStrip(void) const
{
	const char *buf = GetPtr();

	for (uint32_t i = 0; i < GetSize(); ++i) {
		if (buf[i] != ' ' &&
			buf[i] != '\t' &&
			buf[i] != '\r' &&
			buf[i] != '\n') {
			return Slice(i, -1);
		}
	}

	return Slice(0, 0);
}

inline CStringPtr CString::RStrip(void) const
{
	const char *buf = GetPtr();

	/* for (int i = x; i >= 0; --i) */
	for (uint32_t i = GetSize() - 1; i < 0x80000000; --i) {
		if (buf[i] != ' ' &&
			buf[i] != '\t' &&
			buf[i] != '\r' &&
			buf[i] != '\n') {
			return Slice(0, i + 1);
		}
	}

	return Slice(0, 0);
}

inline CStringPtr CString::Slice(int start, int end) const
{
	int _start = (start >= 0) ? start : (GetSize() + start + 1);
	int _end = (end >= 0) ? end : (GetSize() + end + 1);

	CHECK_PARAM((_start >= 0) && (_start <= _end) && (_end <= (int)GetSize()),
				"start: ", DEC(start),
				", end: ", DEC(end),
				", mSize: ", DEC(GetSize()));

	CStringPtr str(mData, (uint32_t)(_end - _start), GetOffset() + _start);

	return str;
}

inline int CString::Find(char ch, Order order) const
{
	uint32_t size = GetSize();
	const char *buf = GetPtr();

	if (size < 1) {
		return -1;
	}

	if (REVERSE == order) {
		for (int i = (int)size - 1; i >= 0; --i) {
			if (buf[i] == ch) {
				return i;
			}
		}

	} else {
		for (uint32_t i = 0; i < size; ++i) {
			if (buf[i] == ch) {
				return (int)i;
			}
		}
	}

	return -1;
}

inline int CString::Find(const CConstStringPtr &str, Order order) const
{
	uint32_t size = str->GetSize();
	uint32_t _size = GetSize();
	const char *buf = GetPtr();
	const char *kbuf = str->GetPtr();

	if (size > _size) {
		return -1;
	} else if (1 == size) {
		return Find(kbuf[0], order);
	} else {
		if (REVERSE == order) {
			for (int i = (int)_size - (int)size; i >= 0; --i) {
				if (0 == ::memcmp(&buf[i], kbuf, size))
					return i;
			}

		} else {
			for (uint32_t i = 0; i < _size - size; ++i) {
				if (0 == ::memcmp(&buf[i], kbuf, size))
					return (int)i;
			}
		}
	}

	return -1;
}

inline int CString::FindToken(void) const
{
	const char *buf = Convert<const char *>();
	uint32_t i;

	for (i = 0; i < GetSize(); ++i) {
		if (buf[i] == ' ' ||
			buf[i] == '\t' ||
			buf[i] == '\r' ||
			buf[i] == '\n') {
			return i;
		}
	}

	return i;
}

inline CString::CStringSwitchPtr CString::Switch(void) const
{
	return CStringSwitchPtr([&](const CConstStringPtr &str) {
		return mData == str->mData;
	});
}

template <class Fn>
inline void CString::Match(const CStringPtr &regex, Fn) const
{
	CRegex engine;
	engine.Compile(regex);
//	engine.Debug();
//	engine.Match(Duplicate(), fn);
}

inline bool CString::ToNum(int &val) const
{
	const char *buf = GetPtr();
	val = 0;

	/* Hex data */
	if ((GetSize() > 2) && (buf[0] == '0') && (buf[1] == 'x')) {
		for (uint32_t i = 2; i < GetSize(); ++i) {
			if (buf[i] >= '0' && buf[i] <= '9') {
				val = val * 16 + (buf[i] - '0');
			} else if ((buf[i] >= 'A' || buf[i] <= 'F')) {
				val = val * 16 + (buf[i] - 'A' + 10);
			} else if ((buf[i] >= 'a' || buf[i] <= 'f')) {
				val = val * 16 + (buf[i] - 'a' + 10);
			} else {
				return false;
			}
		}
	/* Dec data */
	} else {
		int negative = 1;
		uint32_t start = 0;

		if (buf[0] == '-') {
			negative = -1;
			start = 1;
		}

		for (uint32_t i = start; i < GetSize(); ++i) {
			if (buf[i] >= '0' || buf[i] <= '9') {
				val = val * 10 + (buf[i] - '0');
			} else {
				return false;
			}
		}

		val *= negative;
	}

	return true;
}

inline int CString::ToNum(NumberFormat format) const
{
	static const int iformat[] = {
		16,		/* HEX */
		10		/* DEC */
	};

	return ::strtol(GetPtr(), NULL, iformat[format]);
}

inline bool CString::IsNum(void) const
{
	const char *buf = GetPtr();

	/* Hex data */
	if ((GetSize() > 2) && (buf[0] == '0') && (buf[1] == 'x')) {
		for (uint32_t i = 2; i < GetSize(); ++i) {
			if ((buf[i] < '0' || buf[i] > '9') &&
				(buf[i] < 'A' || buf[i] > 'F') &&
				(buf[i] < 'a' || buf[i] > 'f')) {
				return false;
			}
		}
	/* Dec data */
	} else {
		if ((buf[0] < '0' || buf[0] > '9') &&
			(buf[0] != '-')) {
			return false;
		}

		for (uint32_t i = 1; i < GetSize(); ++i) {
			if (buf[i] < '0' || buf[i] > '9') {
				return false;
			}
		}
	}

	return true;
}

inline CString::Iterator::Iterator(const CConstStringPtr &src) :
	mSrc(src),
	mStart(0),
	mEnd(0),
	mIsEnd(true)
{
	TRACE_ASSERT(src);
}

inline bool CString::Iterator::_End(void) const
{
	return mIsEnd;
}

template <class Fn, class... Tn>
inline decltype(auto) CString::Iterator::_Get(const Fn &fn, const Tn & ... tn)
{
	return fn(mSrc->Slice(mStart, mEnd), tn...);
}

inline void CString::Iterator::SetEnd(void)
{
	mStart = 0;
	mEnd = 0;
	mIsEnd = true;
}

#endif /* __STRING_HPP__ */

