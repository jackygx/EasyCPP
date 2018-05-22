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

#ifndef __SLIST_HPP__
#define __SLIST_HPP__

#include <Atomic.hpp>
#include <Common/Typedef.hpp>

#ifdef BIT32
#define CNT			(0x0000000100000000UL)
#define CNT_MASK	(0xFFFFFFFF00000000UL)
#else
#define CNT			(0x0001000000000000UL)
#define CNT_MASK	(0xFFFF000000000000UL)
#endif

#define GET_ADDR(pData) (((uint64_t)(pData)) & (~(CNT_MASK)))
#define GET_CNT(pData) (((uint64_t)(pData)) & CNT_MASK)
#define INC_CNT(pData) (GET_CNT(pData) + CNT)

class SList {
public:
	inline SList(void) :
		mNext(nullptr)
	{
		/* Does nothing */
	}

	inline static void Push(uint64_t *pHead, SList *pNode)
	{
		do {
			uint64_t tmp = *pHead;
			pNode->mNext = (SList *)GET_ADDR(tmp);
			uint64_t pNewHead = ((uint64_t)(pNode)) | INC_CNT(tmp);
			if (ATOMIC_COMPARE_AND_SWAP64(pHead, tmp, pNewHead))
				return;
		} while (true);
	}

	inline static SList *Pop(uint64_t *pHead)
	{
		do {
			uint64_t tmp = *pHead;
			SList *pNode = (SList *)GET_ADDR(tmp);
			if (nullptr == pNode)
				return nullptr;

			uint64_t pNewHead = ((uint64_t)(pNode->mNext)) | GET_CNT(tmp);

			if (ATOMIC_COMPARE_AND_SWAP64(pHead, tmp, pNewHead))
				return pNode;
		} while (true);
	}

private:
	SList *mNext;
};

#endif /* __SLIST_HPP__ */

