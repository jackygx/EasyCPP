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

#ifndef __LINE_SPLIT_HPP__
#define __LINE_SPLIT_HPP__

#include <EasyCpp.hpp>

DEFINE_CLASS(LineSplitIter);

class CLineSplitIter :
	public CString::Iterator
{
private:
	uint32_t mStep;

public:
	inline CLineSplitIter(const CConstStringPtr &src);

private:
	virtual void _Begin(void);
	virtual void _Next(void);
	virtual void _Rest(void);
	virtual CString::IteratorPtr _Reverse(void);

private:
	inline bool Match(uint32_t idx);
};

inline CLineSplitIter::CLineSplitIter(const CConstStringPtr &src) :
	CString::Iterator(src),
	mStep(0)
{
	/* Does nothing */
}

inline bool CLineSplitIter::Match(uint32_t idx)
{
	uint32_t size = mSrc->GetSize();

	TRACE_ASSERT(idx < size);

	if ('\r' == mSrc[idx]) {
		++idx;

		/* Only \r\n is considered as new line. */
		if (idx < size) {
		   if ('\n' == mSrc[idx]) {
			   mStep = 2;
			   return true;
		   }
		}
		/* \r\r or \r* is not considered as new line */
		return false;
	} else if ('\n' == mSrc[idx]) {
		++idx;

		/* \n\r belongs to one match */
		if ('\r' == mSrc[idx]) {
			mStep = 2;
		} else {
			mStep = 1;
		}

		return true;
	} else {
		return false;
	}
}

#endif /* __LINE_SPLIT_HPP__ */

