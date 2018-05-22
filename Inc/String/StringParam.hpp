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

#ifndef __STRING_PARAM_HPP__
#define __STRING_PARAM_HPP__

#include <string.h>
#include <stdlib.h>

#include <Common/Common.hpp>
#include <Function/Function.hpp>
#include <Interface/Interface.hpp>
#include <Meta/Meta.hpp>
#include <Pool/MemPool.hpp>
#include <Debug/Assert.hpp>

//#define STR_DEBUG(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
//#define STR_INFO(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

#if !defined(STR_DEBUG)
#define STR_DEBUG(...)
#endif

#if !defined(STR_INFO)
#define STR_INFO(...)
#endif

#define DEFAULT_STR_SIZE 512

class CString;

class CStringParam
{
public:
	/* Default constructor.
	 * The capacity is DEFAULT_STR_SIZE and the size is 0 */
	inline CStringParam(void);

	/* Constructor from the CMemPtr */
	inline CStringParam(const CMemPtr &mem,
						uint32_t size = 0,
						uint32_t offset = 0);

	/* Constructor from the CStringParam */
	inline CStringParam(const CStringParam &param,
						uint32_t size = 0,
						uint32_t offset = 0);

	/* Construcor with specified capacity and offset */
	struct CStringCapacity
	{
		uint32_t cap;
	};

	inline CStringParam(const CStringCapacity &capacity,
						uint32_t offset = 0);

	/* Constructor with const char * */
	inline explicit CStringParam(const char *buf, uint32_t size);
	inline explicit CStringParam(const char *buf);

	/* Constructor from the number or char */
	enum Type {
		CT_BYTE,	/* Save to memory as byte */
		CT_SHORT,	/* Save to memory as short */
		CT_WORD,	/* Save to memory as word */
		CT_DWORD,	/* Save to memory as dword */
	};

	enum Mode {
		CM_DEC,		/* Convert to string with %d */
		CM_HEX,		/* Convert to string with 0x%X */
		CM_CHAR,	/* Convert to char */
	};

	template <class T>
	inline explicit CStringParam(Type type, Mode mode, T i,
								 uint32_t align = 0, char padding = ' ');

	/* Construct an empty param. */
	struct CStringEmpty {};
	inline CStringParam(const CStringEmpty &);

	inline char *GetPtr(void);
	inline const char *GetPtr(void) const;

	inline uint32_t GetFree(void) const;
	inline uint32_t GetSize(void) const;
	inline uint32_t GetCapacity(void) const;
	inline uint32_t GetOffset(void) const;

	inline void SetSize(uint32_t size);
	inline void SetOffset(uint32_t offset);

	inline int32_t Compare(const CStringParam &str) const;

	inline bool operator == (const CStringParam &str) const;

	inline void Append(const CStringParam &param);

	#define DEFAULT_STR_SIZE 512
	typedef CMemPool<DEFAULT_STR_SIZE> StringBufPool;

	inline void CheckAndAlloc(bool copy);

private:
	inline char *_GetPtr(void);
	inline const char *_GetPtr(void) const;

private:
	uint32_t mCapacity;
	uint32_t mSize;
	uint32_t mOffset;
	CMemPtr mBuf;
	mutable atomic_t mNeedAlloc;
};

#include "StringHelp.hpp"

inline CStringParam::CStringParam(void) :
	mCapacity(DEFAULT_STR_SIZE),
	mSize(0),
	mOffset(0),
	mBuf(StringBufPool::Alloc()),
	mNeedAlloc(0)
{
	STR_DEBUG("Construct default");
}

inline CStringParam::CStringParam(const CMemPtr &mem,
								  uint32_t size, uint32_t offset) :
	mCapacity(size),
	mSize(size),
	mOffset(offset),
	mBuf(mem),
	mNeedAlloc(0)
{
	STR_DEBUG("Construct from CMemPtr, size: %u", size);
}

inline CStringParam::CStringParam(const CStringParam &param,
								  uint32_t size, uint32_t offset) :
	mCapacity(param.GetCapacity()),
	mSize(size),
	mOffset(offset),
	mBuf(param.mBuf),
	mNeedAlloc(1)
{
	STR_DEBUG("Construct from CStringParam, size: %u, offset: %u", size, offset);
	param.mNeedAlloc = 1;
}

inline CStringParam::CStringParam(const CStringCapacity &capacity, uint32_t offset) :
	mCapacity(capacity.cap),
	mSize(0),
	mOffset(offset),
	mBuf(nullptr),
	mNeedAlloc(0)
{
	STR_DEBUG("Construct from capacity, capacity: %u, offset: %u", mCapacity, offset);

	if (mCapacity < DEFAULT_STR_SIZE) {
		mBuf = StringBufPool::Alloc();
		mCapacity = DEFAULT_STR_SIZE;
	} else {
		mBuf = CMemPtr(new char[mCapacity], CMemPtr::ArrayDeleter);
	}
}

inline CStringParam::CStringParam(const char *buf, uint32_t size) :
	mCapacity(size),
	mSize(size),
	mOffset(0),
	mBuf((char *)buf, mBuf.NullDeleter),
	mNeedAlloc(1)
{
	CHECK_PARAM(NULL != buf, "buf is null");
	STR_DEBUG("Construct from buf and size, buf: %p, size: %u", buf, size);
}

inline CStringParam::CStringParam(const char *buf) :
	mCapacity(strlen(buf)),
	mSize(strlen(buf)),
	mOffset(0),
	mBuf((char *)buf, mBuf.NullDeleter),
	mNeedAlloc(1)
{
	CHECK_PARAM(NULL != buf, "buf is null");
	STR_DEBUG("Construct from buf, buf: %p, size: %u", buf, mSize);
}

template <class T>
inline CStringParam::CStringParam(Type type, Mode mode, T i, uint32_t align, char padding) :
	mCapacity(DEFAULT_STR_SIZE),
	mSize(0),
	mOffset(0),
	mBuf(StringBufPool::Alloc()),
	mNeedAlloc(0)
{
	char fmt[32];
	int size;

	STR_DEBUG("Construct from int or char, type: %u, mode: %u", type, mode);

	if (' ' == padding) {
		size = snprintf(fmt, 32, "%%%d", align);
	} else {
		size = snprintf(fmt, 32, "%%%c%d", padding, align);
	}

	if (CT_DWORD == type) {
		fmt[size++] = 'l';
		fmt[size++] = 'l';
	}

	switch (mode) {
		case CM_DEC:
			fmt[size++] = 'd';
			break;

		case CM_HEX:
			fmt[size++] = 'X';
			break;

		case CM_CHAR:
			fmt[size++] = 'c';
			break;
	}

	fmt[size] = '\0';
	mSize = snprintf(_GetPtr(), GetCapacity(), fmt, i);
}

inline CStringParam::CStringParam(const CStringEmpty &) :
	mCapacity(0),
	mSize(0),
	mOffset(0),
	mBuf(nullptr),
	mNeedAlloc(0)
{
	STR_DEBUG("Construct empty");
}

inline char *CStringParam::GetPtr(void)
{
	CheckAndAlloc(true);

	return _GetPtr();
}

inline const char *CStringParam::GetPtr(void) const
{
	return _GetPtr();
}

inline char *CStringParam::_GetPtr(void)
{
	return mBuf.Get() + mOffset;
}

inline const char *CStringParam::_GetPtr(void) const
{
	return mBuf.Get() + mOffset;
}

inline uint32_t CStringParam::GetFree(void) const
{
	return mCapacity - mSize - 1;
}

inline uint32_t CStringParam::GetSize(void) const
{
	return mSize;
}

inline uint32_t CStringParam::GetCapacity(void) const
{
	return mCapacity - mOffset;
}

inline uint32_t CStringParam::GetOffset(void) const
{
	return mOffset;
}

inline void CStringParam::SetSize(uint32_t size)
{
	CHECK_PARAM(size < GetCapacity());

	mSize = size;
}

inline void CStringParam::SetOffset(uint32_t offset)
{
	CHECK_PARAM(offset < mCapacity);

	mSize = mSize + mOffset - offset;
	mOffset = offset;
}

inline int32_t CStringParam::Compare(const CStringParam &param) const
{
	const char *l = mBuf.Get();
	const char *r = param.mBuf.Get();
	uint32_t size = GetSize();
	uint32_t psize = param.GetSize();
	uint32_t offset = GetOffset();
	uint32_t poffset = param.GetOffset();
	int32_t ret = 0;

	if (size < psize) {
		ret = -1;
	} else if (size > psize) {
		size = psize;
		ret = 1;
	}

	for (uint32_t i = 0; i < size; ++i) {
		char _l = l[offset++];
		char _r = r[poffset++];

		if (_l > _r) {
			return 1;
		} else if (_l < _r) {
			return -1;
		}
	}

	return ret;
}

inline bool CStringParam::operator == (const CStringParam &param) const
{
	return (mSize != param.mSize) ? false :
		0 == memcmp(_GetPtr(), param._GetPtr(), mSize);
}

inline void CStringParam::Append(const CStringParam &param)
{
	CheckAndAlloc(true);

	uint32_t size = GetFree();
	uint32_t nsize = param.GetSize();
	size = size < nsize ? size : nsize;

	::memcpy(&_GetPtr()[mSize], param._GetPtr(), size);
	mSize += size;

	_GetPtr()[mSize] = '\0';
}

inline void CStringParam::CheckAndAlloc(bool copy)
{
	if (!ATOMIC_COMPARE_AND_SWAP(&mNeedAlloc, 1, 0))
		return;

	/* Reserve a byte for \0 */
	uint32_t size = GetSize() + 1;
	CMemPtr buf(nullptr);

	if (size <= DEFAULT_STR_SIZE) {
		buf = StringBufPool::Alloc();
		mCapacity = DEFAULT_STR_SIZE;
	} else {
		buf = CMemPtr(new char[size], CMemPtr::ArrayDeleter);
		mCapacity = size;
	}

	if (copy) {
		char *_buf = buf.Get();
		::memcpy(_buf, _GetPtr(), GetSize());
		_buf[GetSize()] = '\0';
	}

	mOffset = 0;
	mBuf = buf;
}

#endif /* __STRING_PARAM_HPP__ */

