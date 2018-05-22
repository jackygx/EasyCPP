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

#ifndef __TOKEN_SPLIT_REV_HPP__
#define __TOKEN_SPLIT_REV_HPP__

#include <EasyCpp.hpp>

DEFINE_CLASS(TokenSplitRevIter);

class CTokenSplitRevIter :
	public CString::Iterator
{
public:
	inline CTokenSplitRevIter(const CConstStringPtr &src);

private:
	virtual void _Begin(void);
	virtual void _Next(void);
	virtual void _Rest(void);
	virtual CString::IteratorPtr _Reverse(void);

private:
	inline bool Match(char ch);
};

inline CTokenSplitRevIter::CTokenSplitRevIter(const CConstStringPtr &src) :
	CString::Iterator(src)
{
	/* Does nothing */
}

inline bool CTokenSplitRevIter::Match(char ch)
{
	return ch == ' ' || ch == '\t' ||
		ch == '\r' || ch == '\n';
}

#endif /* __TOKEN_SPLIT_REV_HPP__ */

