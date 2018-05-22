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

#include "CharSplit.hpp"
#include "CharSplitRev.hpp"

void CCharSplitIter::_Begin(void)
{
	uint32_t size = mSrc->GetSize();

	if (size == 0) {
		SetEnd();
		return;
	}

	mIsEnd = false;
	mStart = 0;
	mEnd = 0;

	/* Find the first non-beginning match */
	for (; mEnd < size; ++mEnd) {
		if (mKey == mSrc[mEnd]) {
			return;
		}
	}
}

void CCharSplitIter::_Next(void)
{
	uint32_t size = mSrc->GetSize();

	/* size == mEnd: Previous entity is the last entity. */
	if (size == mEnd) {
		SetEnd();
		return;
	}

	/* mEnd < size */
	mStart = mEnd + 1;
	/* mStart <= size */

	/* Find next */
	for (mEnd = mStart; mEnd < size; ++mEnd) {
		if (mKey == mSrc[mEnd]) {
			return;
		}
	}
}

void CCharSplitIter::_Rest(void)
{
	uint32_t size = mSrc->GetSize();

	if (mEnd == size) {
		mStart = size;
		return;
	}

	mStart = mEnd + 1;
	mEnd = size;
}

CString::IteratorPtr CCharSplitIter::_Reverse(void)
{
	return CCharSplitRevIterPtr(mSrc, mKey);
}

