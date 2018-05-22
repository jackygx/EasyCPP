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

#ifndef __STRING_SPLIT_HPP__
#define __STRING_SPLIT_HPP__

#include <EasyCpp.hpp>

DEFINE_CLASS(StringSplitIter);

class CStringSplitIter :
	public CString::Iterator
{
private:
	CConstStringPtr mKey;

public:
	inline CStringSplitIter(const CConstStringPtr &src,
							const CConstStringPtr &key);

private:
	virtual void _Begin(void);
	virtual void _Next(void);
	virtual void _Rest(void);
	virtual CString::IteratorPtr _Reverse(void);
};

inline CStringSplitIter::CStringSplitIter(const CConstStringPtr &src,
										  const CConstStringPtr &key) :
	CString::Iterator(src),
	mKey(key)
{
	TRACE_ASSERT(mKey);
}

#endif /* __STRING_SPLIT_HPP__ */

