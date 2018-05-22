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

#include "TokenSplit.hpp"
#include "TokenSplitRev.hpp"

void CTokenSplitRevIter::_Begin(void)
{
	uint32_t size = mSrc->GetSize();
	uint32_t tmp;

	if (0 == size) {
		SetEnd();
		return;
	}

	mIsEnd = false;
	mEnd = size;
	tmp = mEnd - 1;

	/* Ignore the continuous match */
	while (Match(mSrc[tmp])) {
		/* Last character matches */
		if (0 == tmp) {
			SetEnd();
			return;
		}

		mEnd = tmp;
		tmp = mEnd - 1;
	}

	/* mEnd -> next to the 1st non-match */
	mStart = mEnd - 1;
	/* mStart -> 1st non-match */

	/* Find the 2nd match */
	while (mStart > 0) {
		tmp = mStart - 1;

		if (Match(mSrc[tmp])) {
			/* [mStart, mEnd] is splitted */
			return;
		}

		mStart = tmp;
	}

	/* [0, mEnd] is splitted */
}

void CTokenSplitRevIter::_Next(void)
{
	uint32_t tmp;

	/* mStart == 0: Previous entity is the last entity.
	 * mStart == 1: src[0] matches. */
	if (mStart <= 1) {
		SetEnd();
		return;
	}

	/* mStart > 1.
	 * mStart -> next to the 1st match. */
	mEnd = mStart - 1;
	/* mEnd > 0.
	 * mEnd -> 1st match */
	tmp = mEnd - 1;
	/* tmp >= 0.
	 * tmp -> prev to the 1st match. */

	/* Ignore the continuous match */
	while (Match(mSrc[tmp])) {
		/* Last character matches */
		if (0 == tmp) {
			SetEnd();
			return;
		}

		mEnd = tmp;
		tmp = mEnd - 1;
	}

	/* mEnd -> next to the 1st non-match */
	mStart = mEnd - 1;
	/* mStart -> 1st non-match */

	/* Find the 2nd match */
	while (mStart > 0) {
		tmp = mStart - 1;

		if (Match(mSrc[tmp])) {
			/* [mStart, mEnd] is splitted */
			return;
		}

		mStart = tmp;
	}

	/* [0, mEnd] is splitted */
}

void CTokenSplitRevIter::_Rest(void)
{
	uint32_t tmp;

	/* mStart == 0: Previous entity is the last entity.
	 * mStart == 1: src[0] matches. */
	if (mStart <= 1) {
		SetEnd();
		return;
	}

	/* mStart > 1.
	 * mStart -> next to the 1st match. */
	mEnd = mStart - 1;
	/* mEnd > 0.
	 * mEnd -> 1st match */
	tmp = mEnd - 1;
	/* tmp >= 0.
	 * tmp -> prev to the 1st match. */

	/* Ignore the continuous match */
	while (Match(mSrc[tmp])) {
		/* Last character matches */
		if (0 == tmp) {
			SetEnd();
			return;
		}

		mEnd = tmp;
		tmp = mEnd - 1;
	}

	/* mEnd -> next to the 1st non-match */
	mStart = 0;
}

CString::IteratorPtr CTokenSplitRevIter::_Reverse(void)
{
	return CTokenSplitIterPtr(mSrc);
}

