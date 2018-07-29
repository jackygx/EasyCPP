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

#ifndef __SYNC_PROMISE_HELP_HPP__
#define __SYNC_PROMISE_HELP_HPP__

#include <Common/Macro.hpp>

#include "SyncPromise.hpp"

/************************************************
 * Help users to create the SyncPromise
 * From the parameter instead of PromiseParams.
 ************************************************/
template <class... Tn, class... En>
class CSyncPromise<pack<Tn...>, pack<En...>> :
	public CSyncPromise<CSharedPtr<CPromiseParams<Tn...>>,
						CSharedPtr<CPromiseParams<En...>>>
{
	typedef CSharedPtr<CPromiseParams<Tn...>> ParamType;
	typedef CSharedPtr<CPromiseParams<En...>> ErrorType;
	typedef CSyncPromise<ParamType, ErrorType> Parent;

public:
	inline CSyncPromise(const CPromiseBase::Ignore &) :
		Parent(CPromiseBase::Ignore())
	{
		/* Does nothing */
	}

	inline CSyncPromise(const Tn & ... params, CPromiseBase::Type = CPromiseBase::SUCCEED) :
		Parent(ParamType(params...), CPromiseBase::SUCCEED)
	{
		/* Does nothing */
	}

	inline CSyncPromise(const En & ... errors, CPromiseBase::Type = CPromiseBase::FAIL) :
		Parent(ErrorType(errors...), CPromiseBase::FAIL)
	{
		/* Does nothing */
	}
};

template <class... Tn>
class CSyncPromise<pack<Tn...>, pack<Tn...>> :
	public CSyncPromise<CSharedPtr<CPromiseParams<Tn...>>,
						CSharedPtr<CPromiseParams<Tn...>>>
{
	typedef CSharedPtr<CPromiseParams<Tn...>> Type;
	typedef CSyncPromise<Type, Type> Parent;

public:
	inline CSyncPromise(const CPromiseBase::Ignore &) :
		Parent(CPromiseBase::Ignore())
	{
		/* Does nothing */
	}

	inline CSyncPromise(const Tn & ... params, CPromiseBase::Type type) :
		Parent(Type(params...), type)
	{
		/* Does nothing */
	}
};

template <class... Tn>
class CSyncPromise<pack<Tn...>, pack<void>> :
	public CSyncPromise<CSharedPtr<CPromiseParams<Tn...>>,
						CSharedPtr<CPromiseParams<void>>>
{
	typedef CSharedPtr<CPromiseParams<Tn...>> ParamType;
	typedef CSharedPtr<CPromiseParams<void>> ErrorType;
	typedef CSyncPromise<ParamType, ErrorType> Parent;

public:
	inline CSyncPromise(const CPromiseBase::Ignore &) :
		Parent(CPromiseBase::Ignore())
	{
		/* Does nothing */
	}

	inline CSyncPromise(const Tn & ... params, CPromiseBase::Type = CPromiseBase::SUCCEED) :
		Parent(ParamType(params...), CPromiseBase::SUCCEED)
	{
		/* Does nothing */
	}

	inline CSyncPromise(CPromiseBase::Type = CPromiseBase::FAIL) :
		Parent(ErrorType(), CPromiseBase::FAIL)
	{
		/* Does nothing */
	}
};

template <class... En>
class CSyncPromise<pack<void>, pack<En...>> :
	public CSyncPromise<CSharedPtr<CPromiseParams<void>>,
						CSharedPtr<CPromiseParams<En...>>>
{
	typedef CSharedPtr<CPromiseParams<void>> ParamType;
	typedef CSharedPtr<CPromiseParams<En...>> ErrorType;
	typedef CSyncPromise<ParamType, ErrorType> Parent;

public:
	inline CSyncPromise(const CPromiseBase::Ignore &) :
		Parent(CPromiseBase::Ignore())
	{
		/* Does nothing */
	}

	inline CSyncPromise(CPromiseBase::Type = CPromiseBase::SUCCEED) :
		Parent(ParamType(), CPromiseBase::SUCCEED)
	{
		/* Does nothing */
	}

	inline CSyncPromise(const En & ... errors, CPromiseBase::Type = CPromiseBase::FAIL) :
		Parent(ErrorType(errors...), CPromiseBase::FAIL)
	{
		/* Does nothing */
	}
};

template <>
class CSyncPromise<pack<void>, pack<void>> :
	public CSyncPromise<CSharedPtr<CPromiseParams<void>>,
						CSharedPtr<CPromiseParams<void>>>
{
	typedef CSharedPtr<CPromiseParams<void>> Type;
	typedef CSyncPromise<Type, Type> Parent;

public:
	inline CSyncPromise(const CPromiseBase::Ignore &) :
		Parent(CPromiseBase::Ignore())
	{
		/* Does nothing */
	}

	inline CSyncPromise(CPromiseBase::Type type) :
		Parent(Type(), type)
	{
		/* Does nothing */
	}
};

template <class... Tn, class E>
class CSyncPromise<pack<Tn...>, E> :
	public CSyncPromise<CSharedPtr<CPromiseParams<Tn...>>,
								   TO_PROMISABLE_TYPE(E)>
{
	typedef CSharedPtr<CPromiseParams<Tn...>> ParamType;
	typedef TO_PROMISABLE_TYPE(E) ErrorType;
	typedef CSyncPromise<ParamType, ErrorType> Parent;

public:
	inline CSyncPromise(const CPromiseBase::Ignore &) :
		Parent(CPromiseBase::Ignore())
	{
		/* Does nothing */
	}

	inline CSyncPromise(const Tn & ... params, CPromiseBase::Type = CPromiseBase::SUCCEED) :
		Parent(ParamType(params...), CPromiseBase::SUCCEED)
	{
		/* Does nothing */
	}

	inline CSyncPromise(const ErrorType &errors, CPromiseBase::Type = CPromiseBase::FAIL) :
		Parent(errors, CPromiseBase::FAIL)
	{
		/* Does nothing */
	}
};

template <class E>
class CSyncPromise<pack<void>, E> :
	public CSyncPromise<CSharedPtr<CPromiseParams<void>>,
								   TO_PROMISABLE_TYPE(E)>
{
	typedef CSharedPtr<CPromiseParams<void>> ParamType;
	typedef TO_PROMISABLE_TYPE(E) ErrorType;
	typedef CSyncPromise<ParamType, ErrorType> Parent;

public:
	inline CSyncPromise(const CPromiseBase::Ignore &) :
		Parent(CPromiseBase::Ignore())
	{
		/* Does nothing */
	}

	inline CSyncPromise(CPromiseBase::Type = CPromiseBase::SUCCEED) :
		Parent(ParamType(), CPromiseBase::SUCCEED)
	{
		/* Does nothing */
	}

	inline CSyncPromise(const ErrorType &errors, CPromiseBase::Type = CPromiseBase::FAIL) :
		Parent(errors, CPromiseBase::FAIL)
	{
		/* Does nothing */
	}
};

template <class T, class... En>
class CSyncPromise<T, pack<En...>> :
	public CSyncPromise<TO_PROMISABLE_TYPE(T),
						CSharedPtr<CPromiseParams<En...>>>
{
	typedef TO_PROMISABLE_TYPE(T) ParamType;
	typedef CSharedPtr<CPromiseParams<En...>> ErrorType;
	typedef CSyncPromise<ParamType, ErrorType> Parent;

public:
	inline CSyncPromise(const CPromiseBase::Ignore &) :
		Parent(CPromiseBase::Ignore())
	{
		/* Does nothing */
	}

	inline CSyncPromise(const ParamType &promise, CPromiseBase::Type = CPromiseBase::SUCCEED) :
		Parent(promise, CPromiseBase::SUCCEED)
	{
		/* Does nothing */
	}

	inline CSyncPromise(const En & ... en, CPromiseBase::Type = CPromiseBase::FAIL) :
		Parent(ErrorType(en...), CPromiseBase::FAIL)
	{
		/* Does nothing */
	}
};

template <class T>
class CSyncPromise<T, void> :
	public CSyncPromise<TO_PROMISABLE_TYPE(T),
						CSharedPtr<CPromiseParams<void>>>
{
	typedef TO_PROMISABLE_TYPE(T) ParamType;
	typedef CSharedPtr<CPromiseParams<void>> ErrorType;
	typedef CSyncPromise<ParamType, ErrorType> Parent;

public:
	inline CSyncPromise(const CPromiseBase::Ignore &) :
		Parent(CPromiseBase::Ignore())
	{
		/* Does nothing */
	}

	inline CSyncPromise(const ParamType &promise, CPromiseBase::Type = CPromiseBase::SUCCEED) :
		Parent(promise, CPromiseBase::SUCCEED)
	{
		/* Does nothing */
	}

	inline CSyncPromise(CPromiseBase::Type = CPromiseBase::FAIL) :
		Parent(ErrorType(), CPromiseBase::FAIL)
	{
		/* Does nothing */
	}
};

#define PROMISE_PARAM_TYPE(...) \
	CSharedPtr<CPromiseParams<__VA_ARGS__>>

#define PROMISE_PACK_TYPE(...) \
	pack<__VA_ARGS__>

#define DEFINE_SYNC_PROMISE_1(name, params) \
	typedef PROMISE_PARAM_TYPE params C##name##PromiseParamsPtr; \
	typedef CSharedPtr<CSyncPromise<C##name##PromiseParamsPtr>> C##name##PromisePtr

#define DEFINE_SYNC_PROMISE_2(name, params, errors) \
	typedef PROMISE_PARAM_TYPE params C##name##PromiseParamsPtr; \
	typedef PROMISE_PARAM_TYPE errors C##name##PromiseErrorsPtr; \
	typedef PROMISE_PACK_TYPE params C##name##PromisePackParamsPtr; \
	typedef PROMISE_PACK_TYPE errors C##name##PromisePackErrorsPtr; \
	typedef CSharedPtr<CSyncPromise<C##name##PromisePackParamsPtr, \
									C##name##PromisePackErrorsPtr>> \
									C##name##PromisePtr

#define DEFINE_SYNC_PROMISE(name, ...) \
	CONNECT(DEFINE_SYNC_PROMISE_, NARG(__VA_ARGS__))(name, __VA_ARGS__)

template <class... T>
inline decltype(auto) CreateSyncPromise(T && ... t)
{
	typedef CSharedPtr<CPromiseParams<T...>> ParamType;
	typedef CSharedPtr<CSyncPromise<ParamType>> PromisePtr;

	return PromisePtr(ParamType(t...));
}

#endif /* __SYNC_PROMISE_HELP_HPP__ */

