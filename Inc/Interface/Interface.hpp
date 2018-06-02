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

#ifndef __INTERFACE_HPP__
#define __INTERFACE_HPP__

#include <SharedPtr/SharedPtr.hpp>

#define DEFINE_INTERFACE(name) \
	class I##name; \
	typedef CSharedPtr<I##name> I##name##Ptr; \
	typedef CWeakPtr<I##name> I##name##WeakPtr; \
	typedef CSharedPtr<const I##name> IConst##name##Ptr; \
	typedef CWeakPtr<const I##name> IConst##name##WeakPtr;\
	typedef CSharedToken<I##name> I##name##Token; \
	typedef CSharedToken<const I##name> IConst##name##Token

#define DEFINE_CLASS(name) \
	class C##name; \
	typedef CSharedPtr<C##name> C##name##Ptr; \
	typedef CWeakPtr<C##name> C##name##WeakPtr; \
	typedef CSharedPtr<const C##name> CConst##name##Ptr; \
	typedef CWeakPtr<const C##name> CConst##name##WeakPtr; \
	typedef CSharedToken<C##name> C##name##Token; \
	typedef CSharedToken<const C##name> CConst##name##Token

#endif /* __INTERFACE_HPP__ */

