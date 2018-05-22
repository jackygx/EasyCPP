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

#ifndef __TOKEN_SPLIT_HPP__
#define __TOKEN_SPLIT_HPP__

#include <EasyCpp.hpp>

DEFINE_CLASS(TokenSplitIter);

class CTokenSplitIter :
	public CString::Iterator
{
public:
	inline CTokenSplitIter(const CConstStringPtr &src);

private:
	virtual void _Begin(void);
	virtual void _Next(void);
	virtual void _Rest(void);
	virtual CString::IteratorPtr _Reverse(void);

private:
	inline bool Match(char ch);
};

inline CTokenSplitIter::CTokenSplitIter(const CConstStringPtr &src) :
	CString::Iterator(src)
{
	/* Does nothing */
}

inline bool CTokenSplitIter::Match(char ch)
{
	return ch == ' ' || ch == '\t' ||
		ch == '\r' || ch == '\n';
}

#endif /* __TOKEN_SPLIT_HPP__ */

