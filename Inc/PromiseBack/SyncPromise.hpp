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

#ifndef __SYNC_PROMISE_HPP__
#define __SYNC_PROMISE_HPP__

#include "SyncPromiseBase.hpp"

/* Prototype of the CSyncPromise */
template <class... T>
class CSyncPromise {};

/* Succeed case: multiple parameters.
 * Fail case:    N/A. */
template <class... Tn>
class CSyncPromise<pack<Tn...>> :
	public CSyncPromiseBase<CSharedPtr<CPromiseParams<Tn...>>>
{
	typedef CSharedPtr<CPromiseParams<Tn...>> ParamType;
	typedef CSyncPromiseBase<ParamType> Parent;

public:
	inline CSyncPromise(const Tn & ... tn) :
		Parent(ParamType(tn...))
	{
		/* Does nothing */
	}
};

/* Succeed case: void.
 * Fail case:    N/A. */
template <>
class CSyncPromise<pack<void>> :
	public CSyncPromiseBase<CSharedPtr<CPromiseParams<void>>>
{
	typedef CSharedPtr<CPromiseParams<void>> ParamType;
	typedef CSyncPromiseBase<ParamType> Parent;

public:
	inline CSyncPromise(void) :
		Parent(ParamType())
	{
		/* Does nothing */
	}
};

/* Succeed case: multiple parameters.
 * Fail case:    multiple parameters. (different type) */
template <class... Tn, class... En>
class CSyncPromise<pack<Tn...>, pack<En...>> :
	public CSyncPromiseBase<CSharedPtr<CPromiseParams<Tn...>>,
							CSharedPtr<CPromiseParams<En...>>>
{
	typedef CSharedPtr<CPromiseParams<Tn...>> ParamType;
	typedef CSharedPtr<CPromiseParams<En...>> ErrorType;
	typedef CSyncPromiseBase<ParamType, ErrorType> Parent;

public:
	inline CSyncPromise(const Tn & ... tn) :
		Parent(ParamType(tn...))
	{
		/* Does nothing */
	}

	inline CSyncPromise(const En & ... en) :
		Parent(ErrorType(en...))
	{
		/* Does nothing */
	}
};

/* Succeed case: multiple parameters.
 * Fail case:    multiple parameters. (sam type) */
template <class... Tn>
class CSyncPromise<pack<Tn...>, pack<Tn...>> :
	public CSyncPromiseBase<CSharedPtr<CPromiseParams<Tn...>>,
							CSharedPtr<CPromiseParams<Tn...>>>
{
	typedef CSharedPtr<CPromiseParams<Tn...>> ParamType;
	typedef CSyncPromiseBase<ParamType, ParamType> Parent;

public:
	/* Succeed case */
	inline CSyncPromise(const Tn & ... tn, bool result) :
		Parent(ParamType(tn...), result ? CPromiseBase::SUCCEED :
										  CPromiseBase::FAIL)
	{
		/* Does nothing */
	}
};

/* Succeed case: void.
 * Fail case:    multiple parameters. */
template <class... En>
class CSyncPromise<pack<void>, pack<En...>> :
	public CSyncPromiseBase<CSharedPtr<CPromiseParams<void>>,
							CSharedPtr<CPromiseParams<En...>>>
{
	typedef CSharedPtr<CPromiseParams<void>> ParamType;
	typedef CSharedPtr<CPromiseParams<En...>> ErrorType;
	typedef CSyncPromiseBase<ParamType, ErrorType> Parent;

public:
	inline CSyncPromise(void) :
		Parent(ParamType())
	{
		/* Does nothing */
	}

	inline CSyncPromise(const En & ... en) :
		Parent(ErrorType(en...))
	{
		/* Does nothing */
	}
};

/* Succeed case: multiple parameters.
 * Fail case:    void. */
template <class... Tn>
class CSyncPromise<pack<Tn...>, pack<void>> :
	public CSyncPromiseBase<CSharedPtr<CPromiseParams<Tn...>>,
							CSharedPtr<CPromiseParams<void>>>
{
	typedef CSharedPtr<CPromiseParams<Tn...>> ParamType;
	typedef CSharedPtr<CPromiseParams<void>> ErrorType;
	typedef CSyncPromiseBase<ParamType, ErrorType> Parent;

public:
	inline CSyncPromise(const Tn & ... tn) :
		Parent(ParamType(tn...))
	{
		/* Does nothing */
	}

	inline CSyncPromise(void) :
		Parent(ErrorType())
	{
		/* Does nothing */
	}
};

/* Succeed case: void.
 * Fail case:    void. */
template <>
class CSyncPromise<pack<void>, pack<void>> :
	public CSyncPromiseBase<CSharedPtr<CPromiseParams<void>>,
							CSharedPtr<CPromiseParams<void>>>
{
	typedef CSharedPtr<CPromiseParams<void>> ParamType;
	typedef CSyncPromiseBase<ParamType, ParamType> Parent;

public:
	inline CSyncPromise(bool result) :
		Parent(ParamType(), result ? CPromiseBase::SUCCEED :
									 CPromiseBase::FAIL)
	{
		/* Does nothing */
	}
};

#endif /* __SYNC_PROMISE_HPP__ */

