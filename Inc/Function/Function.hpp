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

#ifndef __FUNCTION_HPP__
#define __FUNCTION_HPP__

#include <SharedPtr/SharedPtr.hpp>

#include "FuncImpl.hpp"

#define DEFINE_FUNC(name, ...) \
	typedef CSharedPtr<CFunc<__VA_ARGS__>> name##Fn

DEFINE_FUNC(Runnable, void(void));
DEFINE_FUNC(OnDie, void(void));

#endif /* __FUNCTION_HPP__ */

