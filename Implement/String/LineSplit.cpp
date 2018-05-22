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

#include "LineSplit.hpp"
#include "LineSplitRev.hpp"

void CLineSplitIter::_Begin(void)
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
		if (Match(mEnd)) {
			return;
		}
	}
}

void CLineSplitIter::_Next(void)
{
	uint32_t size = mSrc->GetSize();

	/* mEnd == size: Previous entity is the last entity. */
	if (mEnd == size) {
		SetEnd();
		return;
	}

	/* mEnd == size - mStep: src[size - mStep] == '\r' or '\n'.
	 *                       Last line is an empty line. */
	if (mEnd >= size - mStep) {
		mStart = size;
		mEnd = size;
		return;
	}

	/* mEnd < size - mStep */
	mStart = mEnd + mStep;
	/* mStart < size.
	 * mStart -> next to the previous match. */

	/* Find the first non-beginning match */
	for (mEnd = mStart; mEnd < size; ++mEnd) {
		if (Match(mEnd)) {
			return;
		}
	}
}

void CLineSplitIter::_Rest(void)
{
	uint32_t size = mSrc->GetSize();

	/* mEnd == size: Previous entity is the last entity. */
	if (mEnd == size) {
		SetEnd();
		return;
	}

	/* mEnd == size - mStep: src[size - mStep] == '\r' or '\n'.
	 *                       Last line is an empty line. */
	if (mEnd >= size - mStep) {
		mStart = size;
		mEnd = size;
		return;
	}

	/* mEnd < size - mStep */
	mStart = mEnd + mStep;
	/* mStart < size.
	 * mStart -> next to the previous match. */
	mEnd = size;
}

CString::IteratorPtr CLineSplitIter::_Reverse(void)
{
	return CLineSplitRevIterPtr(mSrc);
}

