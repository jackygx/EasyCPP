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

void CTokenSplitIter::_Begin(void)
{
	uint32_t size = mSrc->GetSize();

	if (size == 0) {
		SetEnd();
		return;
	}

	mIsEnd = false;
	mEnd = 0;

	/* Ignore the continuous match */
	while (Match(mSrc[mEnd])) {
		/* Last character that matches */
		if (mEnd == size - 1) {
			SetEnd();
			return;
		}

		++mEnd;
	}

	/* This is the first non-matched character */
	mStart = mEnd;

	/* Find the end of the non-match */
	for (++mEnd; mEnd < size; ++mEnd) {
		if (Match(mSrc[mEnd])) {
			return;
		}
	}
}

void CTokenSplitIter::_Next(void)
{
	uint32_t size = mSrc->GetSize();
	uint32_t last = size - 1;

	/* size == mEnd:     Previous entity is the last entity.
	 * size == mEnd - 1: Last character got match. */
	if (mEnd >= last) {
		SetEnd();
		return;
	}

	/* mEnd < size - 1 */
	++mEnd;
	/* mEnd < size */

	/* Ignore the continuous match */
	while (Match(mSrc[mEnd])) {
		/* Last character that matches */
		if (mEnd == last) {
			SetEnd();
			return;
		}

		++mEnd;
	}

	mStart = mEnd;

	/* Find the end of the non-match */
	for (++mEnd; mEnd < size; ++mEnd) {
		if (Match(mSrc[mEnd])) {
			return;
		}
	}
}

void CTokenSplitIter::_Rest(void)
{
	uint32_t size = mSrc->GetSize();
	uint32_t last = size - 1;

	/* size == mEnd:     Previous entity is the last entity.
	 * size == mEnd - 1: Last character got match. */
	if (mEnd >= last) {
		SetEnd();
		return;
	}

	/* mEnd < size - 1 */
	++mEnd;
	/* mEnd < size */

	/* Ignore the continuous match */
	while (Match(mSrc[mEnd])) {
		/* Last character that matches */
		if (mEnd == last) {
			SetEnd();
			return;
		}

		++mEnd;
	}

	mStart = mEnd;
	mEnd = size;
}

CString::IteratorPtr CTokenSplitIter::_Reverse(void)
{
	return CTokenSplitRevIterPtr(mSrc);
}

