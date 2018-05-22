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

#ifndef __LINE_SPLIT_REV_HPP__
#define __LINE_SPLIT_REV_HPP__

#include <EasyCpp.hpp>

DEFINE_CLASS(LineSplitRevIter);

class CLineSplitRevIter :
	public CString::Iterator
{
private:
	uint32_t mStep;

public:
	inline CLineSplitRevIter(const CConstStringPtr &src);

private:
	virtual void _Begin(void);
	virtual void _Next(void);
	virtual void _Rest(void);
	virtual CString::IteratorPtr _Reverse(void);

private:
	inline bool Match(uint32_t idx);
};

inline CLineSplitRevIter::CLineSplitRevIter(const CConstStringPtr &src) :
	CString::Iterator(src),
	mStep(0)
{
	/* Does nothing */
}

inline bool CLineSplitRevIter::Match(uint32_t idx)
{
	if ('\r' == mSrc[idx]) {
		/* Only \n\r is considered as new line. */
		if (idx > 0) {
		   if ('\n' == mSrc[idx - 1]) {
			   mStep = 2;
			   return true;
		   }
		}
		/* \r\r or \r* is not considered as new line */
		return false;
	} else if ('\n' == mSrc[idx]) {
		if (idx > 0) {
			/* \r\n is one match */
			if ('\r' == mSrc[idx - 1]) {
				mStep = 2;
				return true;
			}
		}

		mStep = 1;
		return true;
	} else {
		return false;
	}
}

#endif /* __LINE_SPLIT_REV_HPP__ */

