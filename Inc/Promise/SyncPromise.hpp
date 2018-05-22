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

#include <Function/Function.hpp>
#include <Interface/Interface.hpp>
#include <SharedPtr/SharedPtr.hpp>
#include <Meta/Meta.hpp>

#include "Promisable.hpp"
#include "FinalPromise.hpp"

template <class... Tn>
class CSyncPromise {};

template <>
class CSyncPromise<> :
	public CPromisable
{
public:
	inline CSyncPromise(bool result = false) :
		mSucceed(result)
	{
		PROMISE_DEBUG("CSyncPromise %d", result);
	}

	template <class Fn,
			 ENABLE_IF(IS_PROMISABLE(FN_RET_TYPE(Fn)))>
	inline decltype(auto) Then(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromise Then");

		if (mSucceed) {
			return fn();
		} else {
			return FN_RET_TYPE(Fn)();
		}
	}

	template <class Fn,
			 ENABLE_IF(!IS_PROMISABLE(FN_RET_TYPE(Fn)))>
	inline decltype(auto) Then(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromise Then final");

		typedef CSharedPtr<CFinalPromise<FN_RET_TYPE(Fn)>> CFinalPromisePtr;

		if (mSucceed) {
			return ThenFinalPromise(fn);
		} else {
			return CFinalPromisePtr();
		}
	}

protected:
	template <class Fn, ENABLE_IF(std::is_void<FN_RET_TYPE(Fn)>)>
	inline decltype(auto) ThenFinalPromise(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromise ThenFinalPromise<void>");

		typedef CSharedPtr<CFinalPromise<FN_RET_TYPE(Fn)>> CFinalPromisePtr;

		fn();

		return CFinalPromisePtr(true);
	}

	template <class Fn, ENABLE_IF(!std::is_void<FN_RET_TYPE(Fn)>)>
	inline decltype(auto) ThenFinalPromise(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromise ThenFinalPromise<non-void>");

		typedef CSharedPtr<CFinalPromise<FN_RET_TYPE(Fn)>> CFinalPromisePtr;

		return CFinalPromisePtr(fn());
	}

protected:
	bool mSucceed;
};

template <class T>
class CSyncPromise<T> :
	public CPromisable
{
public: /* Constructor */
	/* Default constructor: a failure promise */
	inline CSyncPromise(void) :
		mSucceed(false),
		mArg(nullptr)
	{
		PROMISE_DEBUG("CSyncPromise<%s> fail", TYPE_NAME(T));
	}

	/* A succeed promise */
	inline CSyncPromise(const T &t) :
		mSucceed(true),
		mArg(t)
	{
		PROMISE_DEBUG("CSyncPromise<%s> succeed", TYPE_NAME(T));
	}

public: /* Implicit convertible */
	/* Implicit to mArg */
	template <class TTar = T,
			 ENABLE_IF(IS_STATICALLY_ASSIGNABLE(TTar, T))>
	inline TTar Convert(void)
	{
		if (!mArg) {
			throw ES("Try to get a failure promis");
		}

		return mArg;
	}

	/* Implicit to mArg->Convert (overload) */
	template <class TTar,
			 ENABLE_IF(SPTR_CAN_IMPLICIT_CONVERT(TTar, T))>
	inline TTar Convert(void)
	{
		if (!mArg) {
			throw ES("Try to get a failure promis");
		}

		return mArg.template Convert<TTar>();
	}

public: /* Then */
	template <class Fn,
			 ENABLE_IF(IS_PROMISABLE(FN_RET_TYPE(Fn)))>
	inline decltype(auto) Then(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromise<%s> Then", TYPE_NAME(T));

		if (mSucceed) {
			return ThenPromise(fn);
		} else {
			return FN_RET_TYPE(Fn)();
		}
	}

	template <class Fn,
			 ENABLE_IF(!IS_PROMISABLE(FN_RET_TYPE(Fn)))>
	inline decltype(auto) Then(const Fn &fn)
	{
		PROMISE_DEBUG("CSyncPromise<%s> Then final", TYPE_NAME(T));

		typedef CSharedPtr<CFinalPromise<FN_RET_TYPE(Fn)>> CFinalPromisePtr;

		if (mSucceed) {
			return ThenFinalPromise(fn);
		} else {
			return CFinalPromisePtr();
		}
	}

protected:
	template <class Fn, class... _Tn>
	inline decltype(auto) ThenPromise(const Fn &fn, const _Tn & ... tn)
	{
		PROMISE_DEBUG("CSyncPromise<%s> ThenPromise", TYPE_NAME(T));
		return fn(tn..., mArg);
	}

	template <class Fn,
			 class... _Tn,
			 ENABLE_IF(std::is_void<FN_RET_TYPE(Fn)>)>
	inline decltype(auto) ThenFinalPromise(const Fn &fn, const _Tn & ... tn)
	{
		PROMISE_DEBUG("CSyncPromise<%s> ThenFinalPromise<void>", TYPE_NAME(T));

		typedef CSharedPtr<CFinalPromise<FN_RET_TYPE(Fn)>> CFinalPromisePtr;

		fn(tn..., mArg);

		return CFinalPromisePtr(true);
	}

	template <class Fn,
			 class... _Tn,
			 ENABLE_IF(!std::is_void<FN_RET_TYPE(Fn)>)>
	inline decltype(auto) ThenFinalPromise(const Fn &fn, const _Tn & ... tn)
	{
		PROMISE_DEBUG("CSyncPromise<%s> ThenFinalPromise<non-void>", TYPE_NAME(T));

		typedef CSharedPtr<CFinalPromise<FN_RET_TYPE(Fn)>> CFinalPromisePtr;

		return CFinalPromisePtr(fn(tn..., mArg));
	}

protected:
	bool mSucceed;

private:
	T mArg;
};

template <class T, class T1, class... Tn>
class CSyncPromise<T, T1, Tn...> :
	public CSyncPromise<T1, Tn...>
{
public:
	inline CSyncPromise(void) :
		mArg(nullptr),
		CSyncPromise<T1, Tn...>()
	{
		PROMISE_DEBUG("CSyncPromise<%s, Tn...> fail", TYPE_NAME(T));
	}

	inline CSyncPromise(const T &t, const T1 &t1, const Tn & ... tn) :
		mArg(t),
		CSyncPromise<T1, Tn...>(t1, tn...)
	{
		PROMISE_DEBUG("CSyncPromise<%s, Tn...> succeed", TYPE_NAME(T));
	}

	template <class Fn, class... _Tn,
			 ENABLE_IF(IS_PROMISABLE(FN_RET_TYPE(Fn)))>
	inline decltype(auto) Then(const Fn &fn, const _Tn & ... tn)
	{
		PROMISE_DEBUG("CSyncPromise<%s, Tn...> Then", TYPE_NAME(T));

		if (CSyncPromise<T1, Tn...>::mSucceed) {
			return ThenPromise(fn, tn...);
		} else {
			return FN_RET_TYPE(Fn)();
		}
	}

	template <class Fn, class... _Tn,
			 ENABLE_IF(!IS_PROMISABLE(FN_RET_TYPE(Fn)))>
	inline decltype(auto) Then(const Fn &fn, const _Tn & ... tn)
	{
		PROMISE_DEBUG("CSyncPromise<%s, Tn...> Then final", TYPE_NAME(T));

		typedef CSharedPtr<CFinalPromise<FN_RET_TYPE(Fn)>> CFinalPromisePtr;

		if (CSyncPromise<T1, Tn...>::mSucceed) {
			return ThenFinalPromise(fn, tn...);
		} else {
			return CFinalPromisePtr();
		}
	}

protected:
	template <class Fn, class... _Tn>
	inline decltype(auto) ThenPromise(const Fn &fn, const _Tn & ... tn)
	{
		return CSyncPromise<T1, Tn...>::ThenPromise(fn, tn..., mArg);
	}

	template <class Fn,
			 class... _Tn,
			 ENABLE_IF(std::is_void<FN_RET_TYPE(Fn)>)>
	inline decltype(auto) ThenFinalPromise(const Fn &fn, const _Tn & ... tn)
	{
		typedef CSharedPtr<CFinalPromise<FN_RET_TYPE(Fn)>> CFinalPromisePtr;

		CSyncPromise<T1, Tn...>::ThenPromise(fn, tn..., mArg);

		return CFinalPromisePtr(true);
	}

	template <class Fn,
			 class... _Tn,
			 ENABLE_IF(!std::is_void<FN_RET_TYPE(Fn)>)>
	inline decltype(auto) ThenFinalPromise(const Fn &fn, const _Tn & ... tn)
	{
		typedef CSharedPtr<CFinalPromise<FN_RET_TYPE(Fn)>> CFinalPromisePtr;

		return CFinalPromisePtr(CSyncPromise<T1, Tn...>::ThenPromise(fn, tn..., mArg));
	}

private:
	T mArg;
};

#define DEFINE_SYNC_PROMISE(name, ...) \
	typedef CSharedPtr<CSyncPromise<__VA_ARGS__>> C##name##PromisePtr;

#endif /* __SYNC_PROMISE_HPP__ */

