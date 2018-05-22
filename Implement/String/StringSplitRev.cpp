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

#include "StringSplit.hpp"
#include "StringSplitRev.hpp"

void CStringSplitRevIter::_Begin(void)
{
	uint32_t ssize = mSrc->GetSize();
	uint32_t ksize = mKey->GetSize();

	mIsEnd = false;
	mEnd = ssize;

	/* src string is short than key string.
	 * No need to compare them. */
	if (ssize < ksize) {
		mStart = 0;
		return;
	}

	/* ssize >= ksize */
	mStart = ssize - ksize;
	/* mStart >= 0 */

	while (0 != memcmp(&mSrc[mStart], mKey, ksize)) {

		/* Rest parts are all not matched. */
		if (0 == mStart) {
			return;
		}

		--mStart;
	}

	mStart += ksize;
}

void CStringSplitRevIter::_Next(void)
{
	uint32_t ksize = mKey->GetSize();

	/* mStart == 0:	Previous entity is the last eneity.
	 * 0 < mStart < ksize: This should not happen. */
	if (mStart < ksize) {
		SetEnd();
		return;
	}

	/* mStart >= ksize. */
	mEnd = mStart - ksize;
	/* mEnd >= 0 */

	/* Rest part is smaller than the key */
	if (mEnd < ksize) {
		mStart = 0;
		return;
	}

	/* mEnd >= ksize */
	mStart = mEnd - ksize;
	/* mStart >= 0 */

	while (0 != memcmp(&mSrc[mStart], mKey, ksize)) {

		/* Rest parts are all not matched. */
		if (0 == mStart) {
			return;
		}

		--mStart;
	}

	mStart += ksize;
}

void CStringSplitRevIter::_Rest(void)
{
	uint32_t ksize = mKey->GetSize();

	/* mStart == 0:	Previous entity is the last eneity.
	 * 0 < mStart < ksize: This should not happen. */
	if (mStart < ksize) {
		SetEnd();
		return;
	}

	/* mStart >= ksize. */
	mEnd = mStart - ksize;
	/* mEnd >= 0 */

	mStart = 0;
}

CString::IteratorPtr CStringSplitRevIter::_Reverse(void)
{
	return CStringSplitIterPtr(mSrc, mKey);
}

