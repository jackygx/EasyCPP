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

#ifndef __ASYNC_PROMISE_HPP__
#define __ASYNC_PROMISE_HPP__

#include <Function/Function.hpp>
#include <Interface/Interface.hpp>
#include <SharedPtr/SharedPtr.hpp>
#include <Meta/Meta.hpp>

#include "Promisable.hpp"
#include "SyncPromise.hpp"

namespace PromiseChain
{
	class Ordered {};
	class Reversed {};
}

template <class... T>
class CPromiseChain {};

template <class Child, class FnThen>
class CPromiseChain<Child, FnThen, PromiseChain::Ordered>
{
public:
	inline CPromiseChain(const Child &child, const FnThen &fnThen) :
		mChild(child),
		mFnThen(fnThen)
	{
		PROMISE_INFO(COLOR_GREEN, "CPromiseChain<%s, %s> is created",
					 TYPE_NAME(Child),
					 TYPE_NAME(FnThen));
	}

	template <class Promise>
	inline decltype(auto) Trigger(const Promise &promise)
	{
		PROMISE_INFO(COLOR_RED, "CPromiseChain<%s, %s> is triggered",
					 TYPE_NAME(Child),
					 TYPE_NAME(FnThen));

		return mChild->Trigger(promise->Then(mFnThen));
	}

private:
	Child mChild;
	FnThen mFnThen;
};

template <class FnThen>
class CPromiseChain<FnThen, PromiseChain::Ordered>
{
public:
	inline CPromiseChain(const FnThen &fnThen) :
		mFnThen(fnThen)
	{
		PROMISE_INFO(COLOR_GREEN, "CPromiseChain<%s>(last) is created",
					 TYPE_NAME(FnThen));
	}

	template <class Promise>
	inline decltype(auto) Trigger(const Promise &promise)
	{
		PROMISE_INFO(COLOR_RED, "CPromiseChain<%s>(last) is triggered",
					 TYPE_NAME(FnThen));

		return promise->Then(mFnThen);
	}

private:
	FnThen mFnThen;
};

template <class Parent, class FnThen>
class CPromiseChain<Parent, FnThen, PromiseChain::Reversed> :
	public CEnableSharedPtr<CPromiseChain<Parent, FnThen, PromiseChain::Reversed>>
{
	typedef CPromiseChain<Parent, FnThen, PromiseChain::Reversed> CChain;
	typedef CSharedPtr<CChain> CChainPtr;
	typedef CEnableSharedPtr<CChain> CChainShare;

public:
	inline CPromiseChain(const Parent &parent, const FnThen &fnThen) :
		mParent(parent),
		mFnThen(fnThen)
	{
		PROMISE_INFO(COLOR_BLUE, "CPromiseChain<%s, %s>(Reversed) is created",
					 TYPE_NAME(Parent),
					 TYPE_NAME(FnThen));
	}

	/* The previous chain is reversed.
	 * When user stop calling Then/Catch,
	 * we start constructing the real chain */
	inline ~CPromiseChain(void)
	{
		typedef CSharedPtr<CPromiseChain<FnThen, PromiseChain::Ordered>> CChildPtr;
		if (mParent) {
			mParent->MakeChain(CChildPtr(mFnThen));
		} else {
			PROMISE_INFO(COLOR_YELLOW, "CPromiseChain<%s, %s> is ignored",
						 TYPE_NAME(Parent), TYPE_NAME(FnThen));
		}
	}

	template <class Fn>
	inline decltype(auto) Then(const Fn &fn)
	{
		typedef CSharedPtr<CPromiseChain<CChainPtr, Fn, PromiseChain::Reversed>> CChildPtr;
		return CChildPtr(mParent ? CChainShare::Share() : nullptr, fn);
	}

	template <class Fn>
	inline decltype(auto) Catch(const Fn &fn)
	{
		typedef CSharedPtr<CPromiseChain<CChainPtr, Fn, PromiseChain::Reversed>> CChildPtr;
		if (mParent) {
			return CChildPtr(CChainShare::Share(), fn);
		} else {
			PROMISE_INFO(COLOR_YELLOW, "CPromiseChain<%s, %s> call catch",
						 TYPE_NAME(Parent),
						 TYPE_NAME(FnThen));
			fn();
			return nullptr;
		}
	}

	template <class Child>
	inline void MakeChain(const Child &child)
	{
		typedef CSharedPtr<CPromiseChain<Child, FnThen, PromiseChain::Ordered>> CChildPtr;
		mParent->MakeChain(CChildPtr(child, mFnThen));
		mParent = nullptr;
	}

private:
	Parent mParent;
	FnThen mFnThen;
};

template <class... Tn>
class CAsyncPromise :
	public CPromisable,
	public CEnableSharedPtr<CAsyncPromise<Tn...>>
{
	/* CSharedPtr<CAsyncPromise<>> */
	typedef CSharedPtr<CAsyncPromise<Tn...>> CPromisePtr;
	/* Used by Share(). */
	typedef CEnableSharedPtr<CAsyncPromise<Tn...>> CPromiseShare;

public:
	/* Called by the last node that the chain is ready */
	DEFINE_FUNC(Trigger, void(const Tn & ...));
	DEFINE_FUNC(PromiseReady, void(const TriggerFn &));

public:
	/* Default constructor. A failure promise */
	inline CAsyncPromise(void) :
		mFnReady(nullptr)
	{
		PROMISE_INFO(COLOR_PURPLE, "CAsyncPromise failed");
	}

	/* The mFnReady will be called when then chain is ready.
	 * The mNext will be inited after Then is called */
	inline CAsyncPromise(const PromiseReadyFn &fnReady) :
		mFnReady(fnReady)
	{
		if (!fnReady) {
			throw ES("Input fnready is null");
		}

		PROMISE_INFO(COLOR_BLUE, "CAsyncPromise succeed");
	}

	/* Create the next node and save to mNext */
	template <class Fn>
	inline decltype(auto) Then(const Fn &fn)
	{
		typedef CSharedPtr<CPromiseChain<CPromisePtr, Fn, PromiseChain::Reversed>> CChildPtr;
		return CChildPtr(mFnReady ? CPromiseShare::Share() : nullptr, fn);
	}

	/* The promise chain is ready now.
	 * Create the chain root and give it to the initializer */
	template <class Child>
	inline void MakeChain(const Child &child)
	{
		PROMISE_INFO(COLOR_GREEN, "The Async promise chain is ready.");

		/* Because we are called in the destructor, we cannot throw error.
		 * So we catch the error, handle it and exit the program */
		try {
			/* The child should be copied so that the reference count will not be 0.
			 * When the initializer is freed, the child will be freed. */
			mFnReady([child](const Tn & ... tn) {
				PROMISE_INFO(COLOR_RED, "caller trigger the AsyncPromiseChail");
				child->Trigger(CSharedPtr<CSyncPromise<Tn...>>(tn...));
			});
		} catch (const IException *e) {
			e->Show();
			delete e;
			exit(-1);
		} catch (...) {
			PROMISE_DEBUG("Unknown error!!!");
			exit(-1);
		}
	}

private:
	PromiseReadyFn mFnReady;
};

#define DEFINE_ASYNC_PROMISE(name, ...) \
	typedef typename CAsyncPromise<__VA_ARGS__>::TriggerFn name##TriggerFn; \
	typedef CSharedPtr<CAsyncPromise<__VA_ARGS__>> C##name##PromisePtr

#endif /* __ASYNC_PROMISE_HPP__ */

