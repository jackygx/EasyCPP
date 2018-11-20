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

#ifndef __SYNC_PROMISE_PARAMS_HPP__
#define __SYNC_PROMISE_PARAMS_HPP__

#include "PromiseBase.hpp"

template <class... T>
class CSyncPromise {};

/********************************************************
 * <Then> parameters
 ********************************************************/
template <class... T>
class CSyncPromiseThenParams {};


template <>
class CSyncPromiseThenParams<>
{
public:
	inline CSyncPromiseThenParams(void)
	{
		PROMISE_DEBUG("CSyncPromiseThenParams<void>");
	}

	template <class Fn>
	inline decltype(auto) Then(const Fn &fn)
	{
		return ThenPromise(fn);
	}

	/* For meta program */
	inline void IamSyncPromiseThenParam(void) const;

protected:
	template <class Fn,
			 class... Args,
			 TYPE_ALIAS(FnRetType, FN_RET_TYPE(Fn)),
			 ENABLE_IF(IS_VOID(FnRetType))>
	inline void ThenPromise(const Fn &fn, const Args & ... args)
	{
		fn(args...);
	}

	template <class Fn,
			 class... Args,
			 TYPE_ALIAS(FnRetType, FN_RET_TYPE(Fn)),
			 ENABLE_IF(!IS_VOID(FnRetType))>
	inline decltype(auto) ThenPromise(const Fn &fn, const Args & ... args)
	{
		return fn(args...);
	}
};

HAS_MEMBER(IamSyncPromiseThenParam);
#define IS_SYNC_PROMISE_THEN_PARAM(t) has_pointer_member_IamSyncPromiseThenParam<t>


template <>
class CSyncPromiseThenParams<void> : public CSyncPromiseThenParams<> {};


template <class T>
class CSyncPromiseThenParams<T> :
	public CSyncPromiseThenParams<>
{
	typedef CSyncPromiseThenParams<> Parent;

public:
	inline CSyncPromiseThenParams(const T &t) :
		mArg(t)
	{
		PROMISE_DEBUG("CSyncPromiseThenParams<%s>", TYPE_NAME(T));
	}

	template <class Fn,
			 TYPE_ALIAS(_T, T),
			 ENABLE_IF(IS_THENABLE(_T))>
	inline decltype(auto) Then(const Fn &fn)
	{
		return mArg->Then(fn);
	}

	template <class Fn,
			 TYPE_ALIAS(_T, T),
			 ENABLE_IF(!IS_THENABLE(_T))>
	inline decltype(auto) Then(const Fn &fn)
	{
		return ThenPromise(fn);
	}

	template <TYPE_ALIAS(_T, T),
			 ENABLE_IF(IS_PROMISABLE(_T))>
	inline T GetPromise(void)
	{
		return mArg;
	}

protected:
	template <class Fn, class... _Tn>
	inline decltype(auto) ThenPromise(const Fn &fn, const _Tn & ... tn)
	{
		return Parent::ThenPromise(fn, tn..., mArg);
	}

private:
	T mArg;
};

HAS_MEMBER(GetPromise);
#define IS_PARAM_PROMISE(t) has_pointer_member_GetPromise<t>


template <class T, class... Tn>
class CSyncPromiseThenParams<T, Tn...> :
	public CSyncPromiseThenParams<Tn...>
{
	typedef CSyncPromiseThenParams<Tn...> Parent;

public:
	inline CSyncPromiseThenParams(const T &t, const Tn & ... tn) :
		Parent(tn...),
		mArg(t)
	{
		PROMISE_DEBUG("CSyncPromiseThenParams<%s>", TYPE_NAME(T));
	}

	template <class Fn>
	inline decltype(auto) Then(const Fn &fn)
	{
		return ThenPromise(fn);
	}

protected:
	template <class Fn,
			 class... _Tn>
	inline decltype(auto) ThenPromise(const Fn &fn, const _Tn & ... tn)
	{
		return Parent::ThenPromise(fn, tn..., mArg);
	}

private:
	T mArg;
};


/********************************************************
 * <Catch> parameters
 ********************************************************/
template <class... T>
class CSyncPromiseCatchParams {};


template <>
class CSyncPromiseCatchParams<>
{
public:
	inline CSyncPromiseCatchParams(void)
	{
		PROMISE_DEBUG("CSyncPromiseCatchParams<void>");
	}

	template <class Fn>
	inline decltype(auto) Catch(const Fn &fn)
	{
		return CatchPromise(fn);
	}

protected:
	template <class Fn,
			 class... _Tn,
			 TYPE_ALIAS(FnRetType, FN_RET_TYPE(Fn)),
			 ENABLE_IF(IS_VOID(FnRetType))>
	inline void CatchPromise(const Fn &fn, const _Tn & ... tn)
	{
		fn(tn...);
	}

	/* If the Fn does not return, the Promise is recovered. */
	template <class Fn,
			 class... _Tn,
			 TYPE_ALIAS(FnRetType, FN_RET_TYPE(Fn)),
			 ENABLE_IF(!IS_VOID(FnRetType))>
	inline decltype(auto) CatchPromise(const Fn &fn, const _Tn & ... tn)
	{
		return fn(tn...);
	}
};


template <>
class CSyncPromiseCatchParams<void> : public CSyncPromiseCatchParams<> {};


template <class T, class... Tn>
class CSyncPromiseCatchParams<T, Tn...> :
	public CSyncPromiseCatchParams<Tn...>
{
	typedef CSyncPromiseCatchParams<Tn...> Parent;

public:
	inline CSyncPromiseCatchParams(const T &t, const Tn & ... tn) :
		Parent(tn...),
		mArg(t)
	{
		PROMISE_DEBUG("CSyncPromiseCatchParams<%s>", TYPE_NAME(T));
	}

	template <class Fn>
	inline decltype(auto) Catch(const Fn &fn)
	{
		return CatchPromise(fn);
	}

protected:
	template <class Fn,
			 class... _Tn>
	inline decltype(auto) CatchPromise(const Fn &fn, const _Tn & ... tn)
	{
		return Parent::CatchPromise(fn, tn..., mArg);
	}

private:
	T mArg;
};

#endif /* __SYNC_PROMISE_PARAMS_HPP__ */

