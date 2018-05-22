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

#ifndef __DEMANGLE_HPP__
#define __DEMANGLE_HPP__

#include <cxxabi.h>

template <class T, class Fn>
static inline void Demangle(const Fn &fn)
{
	int status;
	const char *name = typeid(T).name();
	char *ret = abi::__cxa_demangle(name, nullptr, nullptr, &status);

	if (nullptr == ret) {
		fn(name);
	} else {
		fn(ret);
		free(ret);
	}
}

#endif /* __DEMANGLE_HPP__ */

