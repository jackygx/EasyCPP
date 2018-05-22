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

#ifndef __SWITCHABLE_HPP__
#define __SWITCHABLE_HPP__

#include <Function/Function.hpp>
#include <SharedPtr/SharedPtr.hpp>

/* T: The type of the element to be compared. */
template <class T>
class CSwitchable :
	public CEnableSharedPtr<CSwitchable<T>>
{
private:
	typedef CSharedPtr<CSwitchable<T>> CSwitchPtr;

	DEFINE_FUNC(Comp, bool(const T &));

public:
	CSwitchable(const CompFn &comp) :
		mComp(comp),
		mFound(false)
	{
		/* Does nothing */
	}

	template <class Fn>
	inline CSwitchPtr Case(const T &t, const Fn &fn)
	{
		if (!mFound) {
			if (mComp(t)) {
				fn();
				mFound = true;
			}
		}

		return CEnableSharedPtr<CSwitchable<T>>::Share();
	}

	template <class Fn>
	inline void Default(const Fn &fn) const
	{
		if (!mFound) {
			fn();
		}
	}

private:
	CompFn mComp;
	bool mFound;
};

#define DEFINE_SWITCHABLE(name, T) \
	typedef CSharedPtr<CSwitchable<T>> name##SwitchPtr

#endif /* __SWITCHABLE_HPP__ */

