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

#ifndef __PROMISE_BASE_HPP__
#define __PROMISE_BASE_HPP__

#include <Function/Function.hpp>
#include <Interface/Interface.hpp>
#include <SharedPtr/SharedPtr.hpp>

#include "Thenable.hpp"
#include "Catchable.hpp"
#include "PromiseDebug.hpp"

class CPromiseBase
{
public:
	/* This struct is used to specify a situation
	 * that the previous promise has failed and
	 * the Catch function does not recover.
	 * Then the new promise will be ignore.
	 * Neither Then nor Catch method will be called. */
	struct PromiseIgnore {};

	enum PromiseType {
		SUCCEED,
		FAIL,
		IGNORE,
	};

public:
	CPromiseBase(const PromiseIgnore &) :
		mPromiseType(IGNORE)
	{
		/* Does nothing */
	}

	CPromiseBase(enum PromiseType promiseType = IGNORE) :
		mPromiseType(promiseType)
	{
		/* Does nothing */
	}

	/* For meta program */
	inline void ImPromisable(void) const {}

private:
	/* bool is treated as int/uint32_t in C++
	 * Implement this will provide a way that
	 * CPromisable be implicitly converted
	 * into uint32_t equals to 1 or 0.
	 * This will cause some issue for construction
	 * Search "Safe Bool Idiom" for more information. */
private:
	typedef void (CPromiseBase::* bool_t)() const;
	void bool_fn() const {}

public:
	/* operator bool */
	inline operator bool_t() const
	{
		return (SUCCEED == mPromiseType) ?
			&CPromiseBase::bool_fn : 0;
	}

protected:
	enum PromiseType mPromiseType;
};

/* We only support the Promisable used in the CSharedPtr */
HAS_MEMBER(ImPromisable);
#define IS_PROMISABLE(t) has_pointer_member_ImPromisable<t>

#endif /* __PROMISE_BASE_HPP__ */

