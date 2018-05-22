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

#ifndef __ATOMIC_HPP__
#define __ATOMIC_HPP__

#define ATOMIC_COMPARE_AND_SWAP64(a, b, c) __sync_bool_compare_and_swap(a, b, c)
#define ATOMIC_COMPARE_AND_SWAP(a, b, c) __sync_bool_compare_and_swap(a, b, c)
#define ATOMIC_FETCH_AND_ADD(a, b)	__sync_fetch_and_add(a, b)
#define ATOMIC_ADD_AND_FETCH(a, b) __sync_add_and_fetch(a, b)
#define ATOMIC_SUB_AND_FETCH(a, b) __sync_sub_and_fetch(a, b)

typedef unsigned short atomic_t;

#endif /* __ATOMIC_HPP__ */

