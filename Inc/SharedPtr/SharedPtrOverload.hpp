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

/* =====================================================================
 *							operator ==
 * ===================================================================== */
template <class T1, class T2,
		 ENABLE_IF(!has_member_operator_is_equal<T1, T2>),
		 ENABLE_IF(has_member_operator_is_equal<T2, T1>)>
inline bool operator == (const T1 &t1, const T2 &t2)
{
	SPTR_DEBUG("[<%s>(%p)] == [<%s>(%p)] => [<%s>] == [<%s>]",
			   TYPE_NAME(T1), &t1, TYPE_NAME(T2), &t2,
			   TYPE_NAME(T2), TYPE_NAME(T1));

	return t2.operator == (t1);
}

template <class T1, class T2,
		 ENABLE_IF(!has_member_operator_is_equal<T1, T2>),
		 ENABLE_IF(!has_member_operator_is_equal<T2, T1>),
		 ENABLE_IF(has_constructor<T1, T2>)>
inline bool operator == (const T1 &t1, const T2 &t2)
{
	SPTR_DEBUG("[<%s>(%p)] == [<%s>(%p)] => [<%s>] == [<%s>(%s)]",
			   TYPE_NAME(T1), &t1, TYPE_NAME(T2), &t2,
			   TYPE_NAME(T1), TYPE_NAME(T1), TYPE_NAME(T2));

	return t1 == CSharedPtr<T1>(t2);
}

template <class T1, class T2,
		 ENABLE_IF(!has_member_operator_is_equal<T1, T2>),
		 ENABLE_IF(!has_member_operator_is_equal<T2, T1>),
		 ENABLE_IF(!has_constructor<T1, T2>),
		 ENABLE_IF(has_constructor<T2, T1>)>
inline bool operator == (const T1 &t1, const T2 &t2)
{
	SPTR_DEBUG("[<%s>(%p)] == [<%s>(%p)] => [<%s>](%s) == [<%s>]",
			   TYPE_NAME(T1), &t1, TYPE_NAME(T2), &t2,
			   TYPE_NAME(T2), TYPE_NAME(T1), TYPE_NAME(T2));

	return t1 == CSharedPtr<T1>(t2);
}

/* =====================================================================
 *							operator !=
 * ===================================================================== */
template <class T1, class T2,
		 ENABLE_IF(has_operator_is_equal<T1, T2>)>
inline bool operator != (const T1 &t1, const T2 &t2)
{
	SPTR_DEBUG("[<%s>(%p)] != [<%s>(%p)]",
			   TYPE_NAME(T1), &t1, TYPE_NAME(T2), &t2);

	return !(t1 == t2);
}

template <class T1, class T2,
		 ENABLE_IF(!has_operator_is_equal<T1, T2>),
		 ENABLE_IF(has_operator_is_equal<T2, T1>)>
inline bool operator != (const T1 &t1, const T2 &t2)
{
	SPTR_DEBUG("[<%s>(%p)] != [<%s>(%p)]",
			   TYPE_NAME(T2), &t2, TYPE_NAME(T1), &t1);

	return !(t2 == t1);
}

#endif /* __SHARED_PTR_OVERLOAD_HPP__ */

