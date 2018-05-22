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

#ifndef __CAST_HPP__
#define __CAST_HPP__

#include "SharedDebug.hpp"

template <class TSrc,		/* Child */
		 class TTar,		/* Parent */
		 ENABLE_IF(IS_STATICALLY_ASSIGNABLE(TTar, TSrc))>
static inline TTar *DynamicCast(TSrc *src)
{
	return src;
}

/* DynamicCast from Parent to Child. */
template <class TSrc,		/* Parent */
		 class TTar,		/* Child */
		 ENABLE_IF(IS_DYNAMICALLY_ASSIGNABLE(TTar, TSrc))>
static inline TTar *DynamicCast(TSrc *src)
{
	TTar *ret = dynamic_cast<TTar *>(src);

	if (nullptr == ret) {
		SPTR_DEBUG("Fail to dynamic cast from (%s) to (%s)\n",
				   TYPE_NAME(TSrc), TYPE_NAME(TTar));
		throw ES("Fail to do dynamic cast");
	}

	return ret;
}


#endif /* __CAST_HPP__ */

