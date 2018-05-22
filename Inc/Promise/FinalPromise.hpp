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

#ifndef __FINAL_PROMISE_HPP__
#define __FINAL_PROMISE_HPP__

#include <SharedPtr/SharedPtr.hpp>
#include <Meta/Meta.hpp>

template <class T>
class CFinalPromise
{
public:
	inline CFinalPromise(void) :
		mRes(false)
	{
		PROMISE_DEBUG("CFinalPromise<%s> fail", TYPE_NAME(T));
	}

	inline CFinalPromise(const T &val) :
		mRes(true), mVal(val)
	{
		PROMISE_DEBUG("CFinalPromise<%s> succeed", TYPE_NAME(T));
	}

	/* No Then */

	template <class Fn,
			 ENABLE_IF(std::is_same<T, FN_RET_TYPE(Fn)>)>
	inline T Catch(const Fn &fn)
	{
		if (mRes) {
			return mVal;
		} else {
			return fn();
		}
	}

private:
	bool mRes;
	T mVal;
};

template <class K>
class CFinalPromise<CSharedPtr<K>>
{
	typedef CSharedPtr<K> T;

public:
	inline CFinalPromise(void) :
		mVal(nullptr)
	{
		PROMISE_DEBUG("CFinalPromise<%s> fail", TYPE_NAME(T));
	}

	inline CFinalPromise(const T &val) :
		mVal(val)
	{
		PROMISE_DEBUG("CFinalPromise<%s> succeed", TYPE_NAME(T));
	}

	/* No Then */

	template <class Fn,
			 ENABLE_IF(std::is_void<FN_RET_TYPE(Fn)>)>
	inline T Catch(const Fn &fn)
	{
		PROMISE_DEBUG("CFinalPromise<%s> Catch<void>", TYPE_NAME(T));

		if (mVal) {
			return mVal;
		} else {
			fn();
			return nullptr;
		}
	}

	template <class Fn,
			 ENABLE_IF(!std::is_void<FN_RET_TYPE(Fn)>)>
	inline T Catch(const Fn &fn)
	{
		PROMISE_DEBUG("CFinalPromise<%s> Catch<non-void>", TYPE_NAME(T));

		if (mVal) {
			return mVal;
		} else {
			return fn();
		}
	}

	inline T Catch(void)
	{
		PROMISE_DEBUG("CFinalPromise<%s> Catch", TYPE_NAME(T));

		return mVal;
	}

private:
	T mVal;
};

template <>
class CFinalPromise<bool>
{
public:
	inline CFinalPromise(void) :
		mRes(false)
	{
		PROMISE_DEBUG("CFinalPromise<bool> fail");
	}

	inline CFinalPromise(bool) :
		mRes(true)
	{
		PROMISE_DEBUG("CFinalPromise<bool> succeed");
	}

	/* No Then */

	template <class Fn>
	inline bool Catch(const Fn &fn)
	{
		PROMISE_DEBUG("CFinalPromise<bool> Catch<Fn>");

		if (!mRes) {
			fn();
		}

		return mRes;
	}

	inline bool Catch(void)
	{
		PROMISE_DEBUG("CFinalPromise<bool> Catch");

		return mRes;
	}

private:
	bool mRes;
};

template <>
class CFinalPromise<void>
{
public:
	inline CFinalPromise(void) :
		mRes(false)
	{
		PROMISE_DEBUG("CFinalPromise<void> fail");
	}

	inline CFinalPromise(bool) :
		mRes(true)
	{
		PROMISE_DEBUG("CFinalPromise<void> succeed");
	}

	/* No Then */

	template <class Fn>
	inline bool Catch(const Fn &fn)
	{
		PROMISE_DEBUG("CFinalPromise<void> Catch<Fn>");

		if (!mRes) {
			fn();
		}

		return mRes;
	}

	inline bool Catch(void)
	{
		PROMISE_DEBUG("CFinalPromise<void> Catch");

		return mRes;
	}

private:
	bool mRes;
};

#endif /* __FINAL_PROMISE_HPP__ */

