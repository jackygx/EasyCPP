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

#ifndef __THENABLE_HPP__
#define __THENABLE_HPP__

#include <Meta/Meta.hpp>

class CThenable
{
public:
	/* For meta program */
	inline void ImThenable(void) const {}
};

/* We only support the Thenable used in the CSharedPtr */
HAS_MEMBER(ImThenable);
#define IS_THENABLE(t) has_pointer_member_ImThenable<t>

#endif /* __THENABLE_HPP__ */

