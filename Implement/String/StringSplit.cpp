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

void CStringSplitIter::_Begin(void)
{
	uint32_t ssize = mSrc->GetSize();
	uint32_t ksize = mKey->GetSize();

	/* src string is short than key string.
	 * No need to compare them. */
	if (ssize < ksize) {
		SetEnd();
		return;
	}

	mIsEnd = false;
	mStart = 0;

	/* Find next */
	for (mEnd = mStart; mEnd <= ssize - ksize; ++mEnd) {
		if (0 == memcmp(&mSrc[mEnd], mKey, ksize)) {
			return;
		}
	}

	/* No match is found */
	mEnd = ssize;
}

void CStringSplitIter::_Next(void)
{
	uint32_t ssize = mSrc->GetSize();
	uint32_t ksize = mKey->GetSize();

	/* src string is short than key string.
	 * No need to compare them. */
	if (ssize < ksize) {
		SetEnd();
		return;
	}

	/* ssize >= ksize */
	uint32_t dsize = ssize - ksize;
	/* dsize >= 0 */

	/* mEnd == ssize: Previous entity is the last entity.
	 * mEnd > ssize - ksize: Impossible */
	if (mEnd > dsize) {
		SetEnd();
		return;
	}

	/* mEnd <= ssize - ksize */
	mStart = mEnd + ksize;
	/* mStart <= ssize */

	/* Find next */
	for (mEnd = mStart; mEnd <= dsize; ++mEnd) {
		if (0 == memcmp(&mSrc[mEnd], mKey, ksize)) {
			return;
		}
	}

	/* No match is found */
	mEnd = ssize;
}

void CStringSplitIter::_Rest(void)
{
	uint32_t ssize = mSrc->GetSize();
	uint32_t ksize = mKey->GetSize();

	/* Already at the end */
	if (mEnd == ssize) {
		mStart = ssize;
		return;
	}

	/* Update End and start */
	mStart = mEnd + ksize;
	mEnd = ssize;
}

CString::IteratorPtr CStringSplitIter::_Reverse(void)
{
	return CStringSplitRevIterPtr(mSrc, mKey);
}

