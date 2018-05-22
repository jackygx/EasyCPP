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

#ifndef __ITERATOR_HPP__
#define __ITERATOR_HPP__

#include <Function/Function.hpp>
#include <Interface/Interface.hpp>
#include <Meta/Meta.hpp>
#include <SharedPtr/SharedPtr.hpp>
#include <Promise/Promise.hpp>

namespace Iterator {

enum ForEachControl {
	BREAK,
	CONTINUE,
	ERROR,
};

}

using namespace Iterator;

/* Base: The iterator that inherit the IterBase.
 * T:    The type of the 1st element to be iterator.
 * Tn:   The type of the nth elements to be iterator. */
template <class Base, class T, class... Tn>
class IterBase :
	public CEnableSharedPtr<Base>
{
public:
	virtual ~IterBase(void) {}

public:
	template <class Fn, class... Kn>
	inline decltype(auto) Get(const Fn &fn, const Kn & ... kn)
	{
		return ((Base *)this)->_Get(fn, kn...);
	}

	DEFINE_FUNC(VoidIter, void(const T &, const Tn & ...));
	inline void ForEach(const VoidIterFn &fn)
	{
		for (__Begin(); !__End(); __Next()) {
			Get(fn);
		}
	}

	DEFINE_FUNC(BoolIter, ForEachControl(const T &, const Tn & ...));
	inline bool ForEach(const BoolIterFn &fn)
	{
		for (__Begin(); !__End(); __Next()) {
			if (BREAK == Get(fn)) {
				return true;
			}
		}

		return false;
	}

	DEFINE_FUNC(VoidIterShare, void(const T &, const Tn & ..., const CSharedPtr<Base> &));
	inline void ForEach(const VoidIterShareFn &fn)
	{
		for (__Begin(); !__End(); __Next()) {
			Get(fn, CEnableSharedPtr<Base>::Share());
		}
	}

	DEFINE_FUNC(BoolIterShare, ForEachControl(const T &, const Tn & ..., const CSharedPtr<Base> &));
	inline bool ForEach(const BoolIterShareFn &fn)
	{
		for (__Begin(); !__End(); __Next()) {
			if (BREAK == Get(fn, CEnableSharedPtr<Base>::Share())) {
				return true;
			}
		}

		return false;
	}

	DEFINE_ASYNC_PROMISE(ForEach, T, Tn...);
	inline CForEachPromisePtr ForEach(void)
	{
		/* We need to hold an instance for the SharedPtr.
		 * Otherwise, it will be freed */
		auto inst(CEnableSharedPtr<Base>::Share());

		return CForEachPromisePtr([inst](const ForEachTriggerFn &fn) {
			for (inst->__Begin(); !inst->__End(); inst->__Next()) {
				inst->Get([&](const T &t, const Tn & ... tn) {
					fn(t, tn...);
				});
			}
		});
	}

	inline bool End(void)
	{
		return __End();
	}

	inline void Begin(void)
	{
		__Begin();
	}

	DEFINE_SYNC_PROMISE(First, T, Tn...);
	inline CFirstPromisePtr First(void)
	{
		__Begin();

		return __End() ? CFirstPromisePtr() :
			Get([&](const T &t, const Tn & ... tn) {
				return CFirstPromisePtr(t, tn...);
			});
	}

	inline CSharedPtr<Base> First(const VoidIterFn &fn)
	{
		__Begin();

		if (!__End()) {
			Get(fn);
		}

		return CEnableSharedPtr<Base>::Share();
	}

	inline CSharedPtr<Base> First(const VoidIterShareFn &fn)
	{
		__Begin();

		if (!__End()) {
			Get(fn, CEnableSharedPtr<Base>::Share());
		}

		return CEnableSharedPtr<Base>::Share();
	}

	template <class Fn,
			 ENABLE_IF(!std::is_convertible<Fn, VoidIterFn>)>
	inline auto First(const Fn &fn)
	->decltype(std::declval<Fn>()(std::declval<T>(), std::declval<Tn>()...))
	{
		__Begin();

		if (!__End()) {
			return Get(fn);
		}

		return nullptr;
	}

	template <class Fn,
			 ENABLE_IF(!std::is_convertible<Fn, VoidIterShareFn>)>
	inline auto First(const Fn &fn)
	->decltype(std::declval<Fn>()(std::declval<T>(), std::declval<Tn>()...,
								  CEnableSharedPtr<Base>::Share()))
	{
		__Begin();

		if (!__End()) {
			return Get(fn, CEnableSharedPtr<Base>::Share());
		}

		return nullptr;
	}

	inline void Next(void)
	{
		__Next();
	}

	inline CSharedPtr<Base> Next(const VoidIterFn &fn)
	{
		__Next();

		if (!__End()) {
			Get(fn);
		}
		return CEnableSharedPtr<Base>::Share();
	}

	inline CSharedPtr<Base> Next(const VoidIterShareFn &fn)
	{
		__Next();
		if (!__End()) {
			Get(fn, CEnableSharedPtr<Base>::Share());
		}
		return CEnableSharedPtr<Base>::Share();
	}

	template <class Fn,
			 ENABLE_IF(!std::is_convertible<Fn, VoidIterFn>)>
	inline auto Next(const Fn &fn)
	->decltype(std::declval<Fn>()(std::declval<T>(), std::declval<Tn>()...))
	{
		__Next();
		if (!__End()) {
			return Get(fn);
		}
		return nullptr;
	}

	template <class Fn,
			 ENABLE_IF(!std::is_convertible<Fn, VoidIterShareFn>)>
	inline auto Next(const Fn &fn)
	->decltype(std::declval<Fn>()(std::declval<T>(), std::declval<Tn>()...,
								  CEnableSharedPtr<Base>::Share()))
	{
		__Next();
		if (!__End()) {
			return Get(fn, CEnableSharedPtr<Base>::Share());
		}
		return nullptr;
	}

	inline void Rest(const VoidIterFn &fn)
	{
		__Rest();
		if (!__End()) {
			Get(fn);
		}
	}

	template <class Fn,
			 ENABLE_IF(!std::is_convertible<Fn, VoidIterFn>)>
	inline auto Rest(const Fn &fn)
	->decltype(std::declval<Fn>()(std::declval<T>(), std::declval<Tn>()...))
	{
		__Rest();
		if (!__End()) {
			return Get(fn);
		}
		return nullptr;
	}

	inline void RestEach(const VoidIterFn &fn)
	{
		for (__Next(); !__End(); __Next()) {
			Get(fn);
		}
	}

	inline bool RestEach(const BoolIterFn &fn)
	{
		for (__Next(); !__End(); __Next()) {
			if (BREAK == Get(fn)) {
				return true;
			}
		}

		return false;
	}

	inline void RestEach(const VoidIterShareFn &fn)
	{
		for (__Next(); !__End(); __Next()) {
			Get(fn, CEnableSharedPtr<Base>::Share());
		}
	}

	inline bool RestEach(const BoolIterShareFn &fn)
	{
		for (__Next(); !__End(); __Next()) {
			if (BREAK == Get(fn, CEnableSharedPtr<Base>::Share())) {
				return true;
			}
		}

		return false;
	}

	template <class Fn,
			 ENABLE_IF(!std::is_convertible<Fn, VoidIterFn>)>
	inline auto RestEach(const VoidIterFn &fn)
	->decltype(std::declval<Fn>()(std::declval<T>(), std::declval<Tn>()...))
	{
		for (__Next(); !__End(); __Next()) {
			auto tmp(Get(fn));
			if (tmp) {
				return tmp;
			}
		}

		return nullptr;
	}

	template <class Fn,
			 ENABLE_IF(!std::is_convertible<Fn, VoidIterShareFn>)>
	inline auto RestEach(const VoidIterFn &fn)
	->decltype(std::declval<Fn>()(std::declval<T>(), std::declval<Tn>()...,
								  CEnableSharedPtr<Base>::Share()))
	{
		for (__Next(); !__End(); __Next()) {
			auto tmp(Get(fn, CEnableSharedPtr<Base>::Share()));
			if (tmp) {
				return tmp;
			}
		}

		return nullptr;
	}

	DEFINE_SYNC_PROMISE(Find, T, Tn...);
	DEFINE_FUNC(CustFind, bool(const T &, const Tn & ...));

	template <class Fn,
			 ENABLE_IF(IS_SAME_FUNC(Fn, CustFindFn))>
	inline CFindPromisePtr Find(const Fn &fn)
	{
		for (__Begin(); !__End(); __Next()) {
			auto ret(Get([&](const T &t, const Tn & ... tn) {
				if (fn(t, tn...)) {
					return CFindPromisePtr(t, tn...);
				} else {
					return CFindPromisePtr(nullptr);
				}
			}));

			if (ret) {
				return ret;
			}
		}

		return CFindPromisePtr();
	}

	template <class Key,
			 class... Keyn,
			 ENABLE_IF(!IS_SAME_FUNC(Key, CustFindFn))>
	inline CFindPromisePtr Find(const Key &key, const Keyn & ... keyn)
	{
		if (__Find<Base, Key, Keyn...>(key, keyn...)) {
			return Get([&](const T &t, const Tn & ... tn) {
				return CFindPromisePtr(t, tn...);
			});
		} else {
			return CFindPromisePtr();
		}
	}

	inline bool Erase(void)
	{
		if (__End()) {
			return false;
		} else {
			((Base *)this)->_Erase();
			return true;
		}
	}

	template <class _T, class... _Tn>
	inline bool Erase(const _T &key, const _Tn & ... tn)
	{
		if (__Find<Base, _T, _Tn...>(key, tn...)) {
			((Base *)this)->_Erase();
			return true;
		} else {
			return false;
		}
	}

	template <class _T>
	inline void Insert(const _T &t, const Tn & ... tn)
	{
		__Insert<Base, _T, Tn...>(t, tn...);
	}

	template <class... _Tn>
	inline CSharedPtr<Base> Sort(const _Tn & ... tn)
	{
		__Sort<Base, _Tn...>(tn...);
		__Begin();

		return CEnableSharedPtr<Base>::Share();
	}

	HAS_MEMBER(_Reverse);
	template <class _Base = Base, ENABLE_IF(has_member__Reverse<_Base>)>
	inline auto Reverse(void)
	->decltype(std::declval<_Base *>()->_Reverse())
	{
		return ((Base *)this)->_Reverse();
	}

private:
	inline void __Begin(void)
	{
		((Base *)this)->_Begin();
	}

	inline bool __End(void) const
	{
		return ((Base *)this)->_End();
	}

	inline void __Next(void)
	{
		((Base *)this)->_Next();
	}

	inline void __Rest(void)
	{
		((Base *)this)->_Rest();
	}

	HAS_MEMBER(_Find);
	template <class _Base, class _T, class... _Tn,
			 ENABLE_IF(has_member__Find<_Base, _T, _Tn...>)>
	inline bool __Find(const _T &key, const _Tn & ... tn)
	{
		((Base *)this)->_Find(key, tn...);
		return !__End();
	}

	HAS_MEMBER(_Match);
	template <class _Base, class _T, class... _Tn,
			 ENABLE_IF(has_member__Match<_Base, _T, _Tn...>)>
	inline bool __Find(const _T &key, const _Tn & ... tn)
	{
		for (__Begin(); !__End(); __Next()) {
			if (((Base *)this)->_Match(key, tn...)) {
				return true;
			}
		}

		return false;
	}

	HAS_MEMBER(_Insert);
	template <class _Base, class _T, class... _Tn, ENABLE_IF(has_member__Insert<_Base, _T, _Tn...>)>
	inline void __Insert(const _T &t, const Tn & ... tn)
	{
		((Base *)this)->_Insert(t, tn...);
	}

	HAS_MEMBER(_Sort);
	template <class _Base, class... _Tn, ENABLE_IF(has_member__Sort<_Base, _Tn...>)>
	inline void __Sort(const _Tn & ... tn)
	{
		((Base *)this)->_Sort(tn...);
	}
};

#define DEFINE_ITERATOR(name, ...) \
	class name; \
	typedef CSharedPtr<name> name##Ptr; \
	typedef IterBase<name, ##__VA_ARGS__> name##Base

#endif /* __ITERATOR_HPP__ */

