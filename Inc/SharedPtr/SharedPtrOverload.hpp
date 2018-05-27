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

#ifndef __SHARED_PTR_OVERLOAD_HPP__
#define __SHARED_PTR_OVERLOAD_HPP__

template <class T1, class T2,
		 ENABLE_IF(!has_member_operator_is_equal<T1 &&, T2 &&>),
		 ENABLE_IF(has_member_operator_is_equal<T2 &&, const T1 &&>)>
inline bool operator == (T1 &&t1, T2 &&t2)
{
	bool ret;

	SPTR_DEBUG("++[<%s>(%p)] == [<%s>(%p)] => [<%s>] == [<%s>]",
			   TYPE_NAME(T1), &t1, TYPE_NAME(T2), &t2,
			   TYPE_NAME(T2), TYPE_NAME(T1));

	ret = (t2.operator == (std::forward<decltype(t1)>(t1)));

	SPTR_DEBUG("--[<%s>(%p)] == [<%s>(%p)] => [<%s>] == [<%s>], result: %d",
			   TYPE_NAME(T1), &t1, TYPE_NAME(T2), &t2,
			   TYPE_NAME(T2), TYPE_NAME(T1), ret);

	return ret;
}

template <class T1, class T2>
inline bool operator != (T1 &&t1, T2 &&t2)
{
	bool ret;

	SPTR_DEBUG("++[<%s>(%p)] != [<%s>(%p)]",
			   TYPE_NAME(T1), &t1, TYPE_NAME(T2), &t2);

	ret = !(t1 == std::forward<decltype(t2)>(t2));

	SPTR_DEBUG("--[<%s>(%p)] != [<%s>(%p)], result: %d",
			   TYPE_NAME(T1), &t1, TYPE_NAME(T2), &t2, ret);

	return ret;
}

#endif /* __SHARED_PTR_OVERLOAD_HPP__ */

