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

#ifndef POOL_BASE_HPP__
#define POOL_BASE_HPP__

#include <stdio.h>
#include <DataStruct/SList.hpp>

#define POOL_ALLOC_GRAN 16

//#define DEBUG_POOL

template <int size>
class CPoolBase
{
public:
#ifdef DEBUG_POOL
	inline static char *Alloc(void)
	{
		return mInst._Alloc();
	}

	inline char *_Alloc(void)
	{
		char *ptr = (char *)SList::Pop(&sPoolHead);
		if (NULL == ptr) {
			ptr = RealAlloc();
		}

		++mAlloc;

		return ptr;
	}

	inline static void Release(char *buf)
	{
		mInst._Release(buf);
	}

	inline void _Release(char *buf)
	{
		++mFree;
		SList::Push(&sPoolHead, (SList *)buf);
	}
#else
	inline static char *Alloc(void)
	{
		char *ptr = (char *)SList::Pop(&sPoolHead);
		return NULL == ptr ? RealAlloc() : ptr;
	}

	inline static void Release(char *buf)
	{
		SList::Push(&sPoolHead, (SList *)buf);
	}
#endif

private:
#ifdef DEBUG_POOL
	inline CPoolBase(void) :
		sPoolHead(0),
		mRealAlloc(0),
		mAlloc(0),
		mFree(0)
	{
		/* Does nothing */
	}

	inline ~CPoolBase(void)
	{
		printf("MemPool size: %d is desconstructed\n", size);
		printf("RealAlloc: %d, Alloc: %d, Release: %d\n", mRealAlloc, mAlloc, mFree);
	}

	inline char *RealAlloc(void)
	{
		char *ptr = new char[size * POOL_ALLOC_GRAN];

		++mRealAlloc;

		for (int i = 0; i < POOL_ALLOC_GRAN - 1; ++i) {
			SList::Push(&sPoolHead, (SList *)ptr);
			ptr += size;
		}

		return ptr;
	}

	uint64_t sPoolHead;
	int mRealAlloc;
	int mAlloc;
	int mFree;

	static CPoolBase<size> mInst;
#else
	inline static char *RealAlloc(void)
	{
		char *ptr = new char[size * POOL_ALLOC_GRAN];

		for (int i = 0; i < POOL_ALLOC_GRAN - 1; ++i) {
			SList::Push(&sPoolHead, (SList *)ptr);
			ptr += size;
		}

		return ptr;
	}

	static uint64_t sPoolHead;
#endif
};

#ifdef DEBUG_POOL
template <int size>
CPoolBase<size> CPoolBase<size>::mInst;
#else
template <int size>
uint64_t CPoolBase<size>::sPoolHead = 0;
#endif

#define DEFINE_POOL_BASE(name, s) \
	static const int _s = s > sizeof(SList) ? s : sizeof(SList); \
	typedef class CPoolBase<_s> name;

#endif /* POOL_BASE_HPP__ */

