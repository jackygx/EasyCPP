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

#ifndef __SHARED_META_HPP__
#define __SHARED_META_HPP__

#include "SharedDebug.hpp"

HAS_MEMBER(ImSharedPtr);
HAS_MEMBER(SetSharedBase);
HAS_MEMBER(Swap);

#define IS_SHARED_PTR(T) has_member_ImSharedPtr<T>

template <class TSrc,		/* Child */
		 class TTar,		/* Parent */
		 ENABLE_IF(IS_STATICALLY_ASSIGNABLE(TTar, TSrc))>
static inline TTar *DynamicCast(TSrc *src)
{
	return src;
}

/* DynamicCast from Parent to Child */
template <class TSrc,		/* Parent */
		 class TTar,		/* Child */
		 ENABLE_IF(IS_DYNAMICALLY_ASSIGNABLE(TTar, TSrc))>
static inline TTar *DynamicCast(TSrc *src)
{
	TTar *ret = dynamic_cast<TTar *>(src);

	if (nullptr == ret) {
		SPTR_DEBUG("Fail to dynamic cast from (%s) to (%s)\n",
				   TYPE_NAME(TSrc), TYPE_NAME(TTar));
	}

	return ret;
}

#define SPTR_CAN_CONVERT(TTar, TSrc) \
	has_constructor<CSharedPtr<TTar>, TSrc>

HAS_MEMBER(Convert);

#define SPTR_CAN_IMPLICIT_CONVERT(TTar, TSrc) \
	has_template_member_Convert<TSrc, pack<TTar>>

template <class TTar>
class CCanExplicitConvert
{
public:
	template <class TSrc,
			 ENABLE_IF(!SPTR_CAN_IMPLICIT_CONVERT(TTar, TSrc)),
			 ENABLE_IF(SPTR_CAN_CONVERT(TTar, TSrc))>
	inline void CanExplicitConvert(void) {}
};

HAS_MEMBER(CanExplicitConvert);
#define CAN_EXPLICIT_CONVERT(TTar, TSrc) \
	has_template_member_CanExplicitConvert<CCanExplicitConvert<TTar>, pack<TSrc>>

template <class TTar>
class CCanShare
{
public:
	template <class Arg,
			 ENABLE_IF(SPTR_CAN_IMPLICIT_CONVERT(TTar, Arg))>
	inline void CanShare(void) {}

	template <class Arg,
			 ENABLE_IF(!SPTR_CAN_IMPLICIT_CONVERT(TTar, Arg)),
			 ENABLE_IF(IS_SHARED_PTR(Arg)),
			 ENABLE_IF(MAYBE_ASSIGNABLE(TTar, REMOVE_SHARED_PTR(Arg)))>
	inline void CanShare(void) {}
};

HAS_MEMBER(CanShare);
#define SPTR_CAN_SHARE(TTar, ...) \
	has_template_member_CanShare<CCanShare<TTar>, pack<__VA_ARGS__>>

#endif /* __SHARED_META_HPP__ */

