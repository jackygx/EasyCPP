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

#ifndef __IS_HPP__
#define __IS_HPP__

#include <type_traits>

#include "EnableIf.hpp"
#include "HasMember.hpp"
#include "DeduceFunc.hpp"

template <class T>
struct RemovePointer
{
	using type = T;
};

template <class T>
struct RemovePointer<T *>
{
	using type = T;
};

template <class T>
struct RemoveReference
{
	using type = T;
};

template <class T>
struct RemoveReference<T &>
{
	using type = T;
};

template <class T>
struct RemoveConst
{
	using type = T;
};

template <class T>
struct RemoveConst<const T>
{
	using type = T;
};

template <class T>
class CSharedPtr;

template <class T>
struct RemoveSharedPtr
{
	using type = T;
};

template <class T>
struct RemoveSharedPtr<CSharedPtr<T>>
{
	using type = T;
};

template <class T>
struct RemoveSharedPtr<const CSharedPtr<T>>
{
	using type = T;
};

#define REMOVE_POINTER(T) \
	typename RemovePointer<T>::type

#define REMOVE_REFERENCE(T) \
	typename RemoveReference<T>::type

#define REMOVE_CONST(T) \
	typename RemoveConst<T>::type

#define REMOVE_SHARED_PTR(T) \
	typename RemoveSharedPtr<REMOVE_REFERENCE(T)>::type

#define IS_CONST(T) \
	std::is_const<REMOVE_REFERENCE(REMOVE_POINTER(REMOVE_SHARED_PTR(T)))>

#define GET_RAW_TYPE(T) \
	REMOVE_CONST(REMOVE_REFERENCE(REMOVE_POINTER(REMOVE_SHARED_PTR(T))))

#define IS_RAW_TYPE_SAME(T1, T2) \
	std::is_same<GET_RAW_TYPE(T1), GET_RAW_TYPE(T2)>

#define IS_RAW_TYPE_BASE_OF(T1, T2) \
	std::is_base_of<GET_RAW_TYPE(T1), GET_RAW_TYPE(T2)>

class CIsStaticallyAssignable
{
/* Same type conversation or Child to Parent:
 * const     <= const
 * const     <= non-const
 * non-const <= non-const
 */
public:
	/* Target is const
	 * Source does not care */
	template <class TSrc,
			 class TTar,
			 ENABLE_IF(IS_CONST(TTar)),
			 ENABLE_IF(IS_RAW_TYPE_SAME(TSrc, TTar))>
	inline void IsStaticallyAssignable(void) {}

	/* Target is not const
	 * Source is not const */
	template <class TSrc,
			 class TTar,
			 ENABLE_IF(!IS_CONST(TTar)),
			 ENABLE_IF(!IS_CONST(TSrc)),
			 ENABLE_IF(IS_RAW_TYPE_SAME(TSrc, TTar))>
	inline void IsStaticallyAssignable(void) {}

	/* From child
	 * Target is const
	 * Source does not care */
	template <class TSrc,
			 class TTar,
			 ENABLE_IF(IS_CONST(TTar)),
			 ENABLE_IF(IS_RAW_TYPE_BASE_OF(TTar, TSrc)),
			 ENABLE_IF(!IS_RAW_TYPE_BASE_OF(TSrc, TTar))>
	inline void IsStaticallyAssignable(void) {}

	/* From child
	 * Target is not const
	 * Source is not const */
	template <class TSrc,
			 class TTar,
			 ENABLE_IF(!IS_CONST(TTar)),
			 ENABLE_IF(!IS_CONST(TSrc)),
			 ENABLE_IF(IS_RAW_TYPE_BASE_OF(TTar, TSrc)),
			 ENABLE_IF(!IS_RAW_TYPE_BASE_OF(TSrc, TTar))>
	inline void IsStaticallyAssignable(void) {}
};

HAS_MEMBER(IsStaticallyAssignable);
#define IS_STATICALLY_ASSIGNABLE(tar, src) \
	has_template_member_IsStaticallyAssignable<CIsStaticallyAssignable, pack<src, tar>>

class CIsDynamicallyAssignable
{
/* From Parent to Child:
 * const     <= const
 * const     <= non-const
 * non-const <= non-const
 */
public:
	/* From base
	 * Target is const
	 * Source does not care */
	template <class TSrc,
			 class TTar,
			 ENABLE_IF(IS_CONST(TTar)),
			 ENABLE_IF(!IS_RAW_TYPE_BASE_OF(TTar, TSrc)),
			 ENABLE_IF(IS_RAW_TYPE_BASE_OF(TSrc, TTar))>
	inline void IsDynamicallyAssignable(void) {}

	/* From base
	 * Target is not const
	 * Source is not const */
	template <class TSrc,
			 class TTar,
			 ENABLE_IF(!IS_CONST(TTar)),
			 ENABLE_IF(!IS_CONST(TSrc)),
			 ENABLE_IF(!IS_RAW_TYPE_BASE_OF(TTar, TSrc)),
			 ENABLE_IF(IS_RAW_TYPE_BASE_OF(TSrc, TTar))>
	inline void IsDynamicallyAssignable(void) {}
};

HAS_MEMBER(IsDynamicallyAssignable);
#define IS_DYNAMICALLY_ASSIGNABLE(tar, src) \
	has_template_member_IsDynamicallyAssignable<CIsDynamicallyAssignable, pack<src, tar>>

class CMaybeAssignable
{
public:
	template <class TSrc,
			 class TTar,
			 ENABLE_IF(IS_STATICALLY_ASSIGNABLE(TTar, TSrc))>
	inline void MaybeAssignable(void) {}

	template <class TSrc,
			 class TTar,
			 ENABLE_IF(IS_DYNAMICALLY_ASSIGNABLE(TTar, TSrc))>
	inline void MaybeAssignable(void) {}
};

HAS_MEMBER(MaybeAssignable);
#define MAYBE_ASSIGNABLE(tar, src) \
	has_template_member_MaybeAssignable<CMaybeAssignable, pack<src, tar>>

/**********************************************************
 *                      IsSameFunc
 **********************************************************/
#define IS_FUNC(fn) std::is_same<typename deduce_function<fn>::is_func, std::true_type()>

class CIfSameFunc
{
public:
	template <class Fn1,
			 class Fn2,
			 ENABLE_IF(IS_FUNC(Fn1)),
			 ENABLE_IF(IS_FUNC(Fn2)),
			 ENABLE_IF(IS_SAME_RET(Fn1, typename deduce_function<Fn2>::ret_type))>
			/* TODO: Args should also be compared */
	inline void IsSameFunc(void)
	{
		/* Does nothing */
	}
};

HAS_MEMBER(IsSameFunc);
#define IS_SAME_FUNC(Fn1, Fn2) \
	has_template_member_IsSameFunc<CIfSameFunc, pack<Fn1, Fn2>>

#endif /* __IS_HPP__ */

