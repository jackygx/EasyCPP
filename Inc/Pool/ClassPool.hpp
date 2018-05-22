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

#ifndef __CLASS_POOL_HPP__
#define __CLASS_POOL_HPP__

#include <SharedPtr/SharedPtr.hpp>
#include "PoolBase.hpp"

template <class T>
class CClassPool
{
	DEFINE_POOL_BASE(Pool, sizeof(T));
public:
	template <class... Tn>
	static CSharedPtr<T> Alloc(Tn... params)
	{
		char *ptr = Pool::Alloc();

		T *obj = new (ptr) T(params...);

		return CSharedPtr<T>(obj, [](T *buf) {
			~buf();
			Pool::Release((char *)buf);
		});
	}
};

#endif /* __CLASS_POOL_HPP__ */

