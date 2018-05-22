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

void CLineSplitRevIter::_Begin(void)
{
	uint32_t size = mSrc->GetSize();

	if (size == 0) {
		SetEnd();
		return;
	}

	mIsEnd = false;
	mEnd = size;
	mStart = mEnd;

	/* mStart > 0 */
	do {
		uint32_t tmp = mStart - 1;

		if (Match(tmp)) {
			return;
		}

		mStart = tmp;
	} while (mStart > 0);

	/* Now mStart == 0 */
}

void CLineSplitRevIter::_Next(void)
{
	/* mStart == 0: Previous entity is the last entity. */
	if (0 == mStart) {
		SetEnd();
		return;
	}

	/* mStart == mStep: src[0] == '\r' or '\n'.
	 *                  Last line is an empty line. */
	if (mStart <= mStep) {
		mStart = 0;
		mEnd = 0;
		return;
	}

	/* mStart > mStep.
	 * mStart -> head of the prev line */
	mEnd = mStart - mStep;
	/* mEnd > 0.
	 * mEnd -> end of the cur line */
	mStart = mEnd;

	/* mStart > 0 */
	do {
		uint32_t tmp = mStart - 1;

		if (Match(tmp)) {
			return;
		}

		mStart = tmp;
	} while (mStart > 0);

	/* Now mStart == 0 */
}

void CLineSplitRevIter::_Rest(void)
{
	/* mStart == 0: Previous entity is the last entity. */
	if (0 == mStart) {
		SetEnd();
		return;
	}

	/* mStart == mStep: src[0] == '\r' or '\n'.
	 *                  Last line is an empty line. */
	if (mStart <= mStep) {
		mStart = 0;
		mEnd = 0;
		return;
	}

	/* mStart > mStep.
	 * mStart -> head of the prev line */
	mEnd = mStart - mStep;
	/* mEnd > 0.
	 * mEnd -> end of the cur line */
	mStart = 0;
}

CString::IteratorPtr CLineSplitRevIter::_Reverse(void)
{
	return CLineSplitIterPtr(mSrc);
}

