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

#ifndef __PROMISE_PARAMS_HPP__
#define __PROMISE_PARAMS_HPP__

#include <Function/Function.hpp>
#include <Interface/Interface.hpp>
#include <SharedPtr/SharedPtr.hpp>
#include <Meta/Meta.hpp>

#include "PromiseBase.hpp"
#include "Thenable.hpp"
#include "Catchable.hpp"

#define TO_PROMISABLE_TYPE(t) \
	REMOVE_ARRAY(REMOVE_REFERENCE(t))

template <class... T>
class CPromiseParams {};

template <>
class CPromiseParams<void> :
	public CThenable,
	public CCatchable
{
public:
	inline CPromiseParams(void)
	{
		PROMISE_DEBUG("CPromiseParams<void>");
	}

	template <class Fn>
	inline decltype(auto) Then(const Fn &fn)
	{
		return fn();
	}

	template <class Fn>
	inline decltype(auto) Catch(const Fn &fn)
	{
		return fn();
	}
};

template <>
class CPromiseParams<> :
	public CThenable,
	public CCatchable
{
public:
	inline CPromiseParams(void)
	{
		PROMISE_DEBUG("CPromiseParams<void>");
	}

	template <class Fn>
	inline decltype(auto) Then(const Fn &fn)
	{
		return ThenPromise(fn);
	}

	template <class Fn>
	inline decltype(auto) Catch(const Fn &fn)
	{
		return CatchPromise(fn);
	}

protected:
	template <class Fn,
			 class... _Tn>
	inline decltype(auto) ThenPromise(const Fn &fn, _Tn && ... tn)
	{
		return fn(tn...);
	}

	template <class Fn,
			 class... _Tn>
	inline decltype(auto) CatchPromise(const Fn &fn, _Tn && ... tn)
	{
		return fn(tn...);
	}
};

template <class T, class... Tn>
class CPromiseParams<T, Tn...> :
	public CPromiseParams<Tn...>
{
	typedef CPromiseParams<Tn...> Parent;
	typedef TO_PROMISABLE_TYPE(T) _T;

public:
	inline CPromiseParams(const _T &t,
						  const TO_PROMISABLE_TYPE(Tn) & ... tn) :
		Parent(tn...),
		mArg(t)
	{
		PROMISE_DEBUG("CPromiseParams<%s>", TYPE_NAME(T));
	}

	template <class Fn>
	inline decltype(auto) Then(const Fn &fn)
	{
		return ThenPromise(fn);
	}

	template <class Fn>
	inline decltype(auto) Catch(const Fn &fn)
	{
		return CatchPromise(fn);
	}

protected:
	template <class Fn,
			 class... _Tn,
			 class K = _T,
			 ENABLE_IF(IS_PROMISABLE(K))>
	inline decltype(auto) ThenPromise(const Fn &fn, _Tn && ... tn)
	{
		return mArg->Then([&](const auto & ... _tn) {
			return Parent::ThenPromise(fn, tn..., _tn...);
		});
	}

	template <class Fn,
			 class... _Tn,
			 class K = _T,
			 ENABLE_IF(!IS_PROMISABLE(K))>
	inline decltype(auto) ThenPromise(const Fn &fn, _Tn && ... tn)
	{
		return Parent::ThenPromise(fn, tn..., mArg);
	}

	template <class Fn,
			 class... _Tn,
			 class K = _T,
			 ENABLE_IF(IS_PROMISABLE(K))>
	inline decltype(auto) CatchPromise(const Fn &fn, _Tn && ... tn)
	{
		return mArg->Catch([&](const auto & ... _tn) {
			return Parent::CatchPromise(fn, tn..., _tn...);
		});
	}

	template <class Fn,
			 class... _Tn,
			 class K = _T,
			 ENABLE_IF(!IS_PROMISABLE(K))>
	inline decltype(auto) CatchPromise(const Fn &fn, _Tn && ... tn)
	{
		return Parent::CatchPromise(fn, tn..., mArg);
	}

private:
	_T mArg;
};

#endif /* __PROMISE_PARAMS_HPP__ */

