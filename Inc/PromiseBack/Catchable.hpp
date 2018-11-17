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

#ifndef __CATCHABLE_HPP__
#define __CATCHABLE_HPP__

#include <Meta/Meta.hpp>

class CCatchable
{
public:
	/* For meta program */
	inline void ImCatchable(void) const {}
};

/* We only support the Catchable used in the CSharedPtr */
HAS_MEMBER(ImCatchable);
#define IS_CATCHABLE(t) has_pointer_member_ImCatchable<t>

#endif /* __CATCHABLE_HPP__ */

