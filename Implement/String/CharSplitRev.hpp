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

#ifndef __CHAR_SPLIT_REV_HPP__
#define __CHAR_SPLIT_REV_HPP__

#include <EasyCpp.hpp>

DEFINE_CLASS(CharSplitRevIter);

class CCharSplitRevIter :
	public CString::Iterator
{
private:
	char mKey;

public:
	inline CCharSplitRevIter(const CConstStringPtr &src, char key);

private:
	virtual void _Begin(void);
	virtual void _Next(void);
	virtual void _Rest(void);
	virtual CString::IteratorPtr _Reverse(void);
};

inline CCharSplitRevIter::CCharSplitRevIter(const CConstStringPtr &src, char key) :
	CString::Iterator(src),
	mKey(key)
{
	/* Does nothing */
}

#endif /* __CHAR_SPLIT_REV_HPP__ */

