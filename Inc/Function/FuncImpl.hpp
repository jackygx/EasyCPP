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

#ifndef __FUNC_IMPL_HPP__
#define __FUNC_IMPL_HPP__

#include <Meta/Meta.hpp>
#include <Pool/PoolBase.hpp>

template <class T>
class CFunc;

template <class Ret, class... Args>
class CFunc<Ret(Args...)>
{
private:
	void *mThis;
	void (*mDestructor)(void *);
	Ret (*mCall)(void *, Args...);

public:
	template <class Fn,
			 ENABLE_IF(IS_SAME_RET(Fn, Ret)),
			 ENABLE_IF(IS_SAME_ARGS(Fn, Args...))>
	CFunc(const Fn &fn) :
		mThis(nullptr),
		mDestructor(nullptr),
		mCall(nullptr)
	{
		Alloc(fn);
	}

	CFunc(void *fn) :
		mThis(fn),
		mDestructor(nullptr),
		mCall(nullptr)
	{
		typedef Ret (*Fn)(Args...);

		/* Save function call */
		mCall = [](void *_this, Args... args) {
			Fn fn = (Fn)_this;
			return fn(args...);
		};
	}

	~CFunc(void)
	{
		if (NULL != mDestructor) {
			mDestructor(mThis);
		}
	}

	inline Ret operator () (Args... args) const
	{
		return mCall(mThis, args...);
	}

private:
	template <class Fn,
           ENABLE_IF(!IS_LAMBDA(Fn))>
	inline void Alloc(const Fn &fn)
	{
		mThis = (void *)&fn;
		/* Save function call */
		mCall = [](void *_this, Args... args) {
			Fn *fn = (Fn *)_this;
			return fn(args...);
		};
	}

	template <class Lambda,
           ENABLE_IF(IS_LAMBDA(Lambda)),
           ENABLE_IFEQ(sizeof(Lambda), 1)>
	inline void Alloc(const Lambda &fn)
	{
		typedef CLASS_TYPE(Lambda) LambdaType;
		mThis = (void *)&fn;
		/* Save function call */
		mCall = [](void *_this, Args... args) {
			LambdaType *lambda = (LambdaType *)_this;
			return lambda->operator()(args...);
		};
	}

	template <class Lambda,
           ENABLE_IF(IS_LAMBDA(Lambda)),
           ENABLE_IFNEQ(sizeof(Lambda), 1)>
	inline void Alloc(const Lambda &fn)
	{
		typedef CLASS_TYPE(Lambda) LambdaType;

		/* Create Lambda class and construct it.
		 * Mainly to construct all the closures. */
		DEFINE_POOL_BASE(Pool, sizeof(fn));
		mThis = (void *)Pool::Alloc();
		new (mThis) LambdaType(fn);

		/* Save destructor */
		mDestructor = [](void *_this) {
			LambdaType *lambda = (LambdaType *)_this;
			lambda->~LambdaType();
			Pool::Release((char *)_this);
		};

		/* Save function call */
		mCall = [](void *_this, Args... args) {
			LambdaType *lambda = (LambdaType *)_this;
			return lambda->operator()(args...);
		};
	}
};

template<class Ret, class... Args>
struct deduce_function<CSharedPtr<CFunc<Ret(Args...)>>>
{
	using ret_type = Ret;
	using class_type = void;
	static const int nParam = sizeof...(Args);
	using args_type = pack<Args...>;
	using const_type = std::false_type();
	using is_func = std::true_type();
};

#endif /* __FUNC_IMPL_HPP__ */

