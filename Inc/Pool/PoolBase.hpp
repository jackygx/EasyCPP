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
#include <Debug/TypeDebug.hpp>
#include <Common/Color.hpp>

#define DEBUG_POOL
//#define DEBUG_POOL_ALLOC

#ifndef DEBUG_POOL
#undef DEBUG_POOL_ALLOC
#endif

#define POOL_ALLOC_GRAN 16

#ifdef DEBUG_POOL
struct CPoolList
{
	void (*Reset)(void);
	void (*ShowUsage)(void);
	void (*ShowTrace)(void);
	int size;
	CPoolList *mNext;
};

template <class T = CPoolList>
class CPoolStastics
{
public:
	static uint32_t RegisterPool(T *list)
	{
		list->mNext = sPoolList;
		sPoolList = list;

		return 0;
	}

	static void ResetPool(void)
	{
		T *head = sPoolList;

		while (nullptr != head) {
			head->Reset();
			head = head->mNext;
		}
		printf("========== Pool reset done ==========\n");
	}

	static void ShowPoolUsage(void)
	{
		T *head = sPoolList;

		printf("========== Start dumping pool usage ==========\n");
		while (nullptr != head) {
			head->ShowUsage();
			head = head->mNext;
		}
		printf("========== Stop dumping pool usage ==========\n");
	}

	static void ShowPoolTrace(int size)
	{
		T *head = sPoolList;

		printf("========== Start dumping pool trace <%d> ==========\n", size);
		while (nullptr != head) {
			if (size == head->size) {
				head->ShowTrace();
				break;
			}
			head = head->mNext;
		}
		printf("========== Stop dumping pool trace <%d> ==========\n", size);
	}

	struct PoolOwnerInfo {
		const char *func;
		int line;
		int size;
	};

private:
	static T *sPoolList;
};

template <class T>
T *CPoolStastics<T>::sPoolList = nullptr;

#else

template <class T = int>
class CPoolStastics
{
public:
	static uint32_t RegisterPool(T *)
	{
		return 0;
	}

	static void ResetPool(void)
	{
		printf("Please enable DEBUG_POOL to debug Memory pool\n");
	}

	static void ShowPoolUsage(void)
	{
		printf("Please enable DEBUG_POOL to debug Memory pool\n");
	}

	static void ShowPoolTrace(int)
	{
		printf("Please enable DEBUG_POOL to debug Memory pool\n");
	}
};

#endif

#ifdef DEBUG_POOL_ALLOC
#define POOL_BASE_TEMPLATE template <int size, class T>
#define POOL_BASE_TEMPLATE_IMPL size, T
#else
#define POOL_BASE_TEMPLATE template <int size>
#define POOL_BASE_TEMPLATE_IMPL size
#endif

POOL_BASE_TEMPLATE
class CPoolBase
{
public:
	inline static char *Alloc(void)
	{
#ifdef DEBUG_POOL
		uint32_t idx = ATOMIC_FETCH_AND_ADD(&sPoolTraceIdx, 1);
		PoolTrace *trace = &sPoolTrace[idx & (TRACE_CNT - 1)];
		trace->ops = ALLOC_IN;
		trace->ptr = 0;
		trace->head = sPoolHead;

		ATOMIC_ADD_AND_FETCH(&sAlloc, 1);
#endif

		char *ptr = (char *)SList::Pop(&sPoolHead);
		char *ret = (NULL == ptr) ? RealAlloc() : ptr;

#ifdef DEBUG_POOL
		idx = ATOMIC_FETCH_AND_ADD(&sPoolTraceIdx, 1);
		trace = &sPoolTrace[idx & (TRACE_CNT - 1)];
		trace->ops = ALLOC_OUT;
		trace->ptr = ret;
		trace->head = sPoolHead;
#endif
		return ret;
	}

	inline static void Release(char *buf)
	{
#ifdef DEBUG_POOL
		uint32_t idx = ATOMIC_FETCH_AND_ADD(&sPoolTraceIdx, 1);
		PoolTrace *trace = &sPoolTrace[idx & (TRACE_CNT - 1)];
		trace->ops = FREE_IN;
		trace->ptr = buf;
		trace->head = sPoolHead;

		ATOMIC_ADD_AND_FETCH(&sFree, 1);
#endif

		SList::Push(&sPoolHead, (SList *)buf);

#ifdef DEBUG_POOL
		idx = ATOMIC_FETCH_AND_ADD(&sPoolTraceIdx, 1);
		trace = &sPoolTrace[idx & (TRACE_CNT - 1)];
		trace->ops = FREE_OUT;
		trace->ptr = 0;
		trace->head = sPoolHead;
#endif
	}
	
private:
	inline static char *RealAlloc(void)
	{
		char *ptr = new char[size * POOL_ALLOC_GRAN];

		for (int i = 0; i < POOL_ALLOC_GRAN - 1; ++i) {
#ifdef DEBUG_POOL
			uint32_t idx = ATOMIC_FETCH_AND_ADD(&sPoolTraceIdx, 1);
			PoolTrace *trace = &sPoolTrace[idx & (TRACE_CNT - 1)];
			trace->ops = REAL_ALLOC;
			trace->ptr = ptr;
			trace->head = sPoolHead;
#endif
			SList::Push(&sPoolHead, (SList *)ptr);
			ptr += size;
		}

#ifdef DEBUG_POOL
		uint32_t idx = ATOMIC_FETCH_AND_ADD(&sPoolTraceIdx, 1);
		PoolTrace *trace = &sPoolTrace[idx & (TRACE_CNT - 1)];
		trace->ops = REAL_ALLOC;
		trace->ptr = ptr;
		trace->head = sPoolHead;

		ATOMIC_ADD_AND_FETCH(&sRealAlloc, 1);
#endif

		return ptr;
	}

	static uint64_t sPoolHead;

#ifdef DEBUG_POOL
private:
	enum PoolOps {
		ALLOC_IN,
		ALLOC_OUT,
		FREE_IN,
		FREE_OUT,
		REAL_ALLOC,
		POOL_OPS_MAX,
	};

	static const char *PoolOpsStr[POOL_OPS_MAX];

	struct PoolTrace {
		PoolOps ops;
		char *ptr;
		uint64_t head;
	};

	static const uint32_t TRACE_CNT = 0x100;

	static CPoolList sList;

	static PoolTrace sPoolTrace[TRACE_CNT];
	static uint32_t sPoolTraceIdx;

	static uint32_t sRealAlloc;
	static uint32_t sAlloc;
	static uint32_t sFree;

	inline static void Reset(void)
	{
		sAlloc = 0;
		sFree = 0;
	}

	inline static void ShowUsage(void)
	{
		if (sAlloc == sFree) {
			printf(COLOR_GREEN);
		} else {
			printf(COLOR_RED);
		}

		printf("[0x%04x]:  RealAlloc: %04d, Alloc: %04d, Free: %04d"
#ifdef DEBUG_POOL_ALLOC
			   ", %s"
#endif
			   COLOR_NONE "\n",
			   size, sRealAlloc, sAlloc, sFree
#ifdef DEBUG_POOL_ALLOC
			   ,TYPE_NAME(T)
#endif
			  );
	}

	inline static void ShowTrace(void)
	{
		printf("Totally %d entries\n", sPoolTraceIdx);
		printf("PoolOps    Pointer             Head\n");
		uint32_t i = (sPoolTraceIdx > TRACE_CNT) ?
			(sPoolTraceIdx - TRACE_CNT) : 0;

		for (; i < sPoolTraceIdx; ++i) {
			PoolTrace *trace = &sPoolTrace[i & (TRACE_CNT - 1)];

			printf("%s 0x%016lx  0x%016lx\n",
				   PoolOpsStr[trace->ops],
				   (uint64_t)trace->ptr, trace->head);
		}
	}
#endif
};

#ifdef DEBUG_POOL

POOL_BASE_TEMPLATE
const char *CPoolBase<POOL_BASE_TEMPLATE_IMPL>::PoolOpsStr[] = {
	"ALLOC_IN  ",
	"ALLOC_OUT ",
	"FREE_IN   ",
	"FREE_OUT  ",
	"REAL_ALLOC",
};

POOL_BASE_TEMPLATE
CPoolList CPoolBase<POOL_BASE_TEMPLATE_IMPL>::sList = {Reset, ShowUsage, ShowTrace, size, nullptr};

POOL_BASE_TEMPLATE
typename CPoolBase<POOL_BASE_TEMPLATE_IMPL>::PoolTrace CPoolBase<POOL_BASE_TEMPLATE_IMPL>::sPoolTrace[TRACE_CNT];

POOL_BASE_TEMPLATE
uint32_t CPoolBase<POOL_BASE_TEMPLATE_IMPL>::sPoolTraceIdx = CPoolStastics<>::RegisterPool(&sList);

POOL_BASE_TEMPLATE
uint32_t CPoolBase<POOL_BASE_TEMPLATE_IMPL>::sRealAlloc = 0;

POOL_BASE_TEMPLATE
uint32_t CPoolBase<POOL_BASE_TEMPLATE_IMPL>::sAlloc = 0;

POOL_BASE_TEMPLATE
uint32_t CPoolBase<POOL_BASE_TEMPLATE_IMPL>::sFree = 0;

#endif

POOL_BASE_TEMPLATE
uint64_t CPoolBase<POOL_BASE_TEMPLATE_IMPL>::sPoolHead = 0;

#ifdef DEBUG_POOL_ALLOC
#define DEFINE_POOL_BASE(name, s, type) \
	static constexpr int _s = s > sizeof(SList) ? s : sizeof(SList); \
	typedef class CPoolBase<_s, type> name
#else
#define DEFINE_POOL_BASE(name, s, type) \
	static constexpr int _s = s > sizeof(SList) ? s : sizeof(SList); \
	typedef class CPoolBase<_s> name
#endif

#endif /* POOL_BASE_HPP__ */

