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

void CCharSplitRevIter::_Begin(void)
{
	uint32_t size = mSrc->GetSize();

	if (0 == size) {
		SetEnd();
		return;
	}

	mIsEnd = false;
	mEnd = size;
	mStart = mEnd;

	/* mStart > 0 */
	do {
		uint32_t tmp = mStart - 1;

		if (mKey == mSrc[tmp]) {
			return;
		}

		mStart = tmp;
	} while (mStart > 0);

	/* Now mStart == 0 */
}

void CCharSplitRevIter::_Next(void)
{
	/* mStart == 0: Previous entity is the last entity. */
	if (0 == mStart) {
		SetEnd();
		return;
	}

	/* mStart > 0 */
	mEnd = mStart - 1;
	mStart = mEnd;
	/* mStart >= 0 */

	while (mStart > 0) {
		uint32_t tmp = mStart - 1;

		if (mKey == mSrc[tmp]) {
			return;
		}

		mStart = tmp;
	}

	/* Now mStart == 0 */
}

void CCharSplitRevIter::_Rest(void)
{
	/* mStart == 0: Previous entity is the last eneity. */
	if (0 == mStart) {
		SetEnd();
		return;
	}

	/* mStart > 0 */
	mEnd = mStart - 1;
	mStart = 0;
}

CString::IteratorPtr CCharSplitRevIter::_Reverse(void)
{
	return CCharSplitIterPtr(mSrc, mKey);
}

