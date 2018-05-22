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

#ifndef __LIST_HPP__
#define __LIST_HPP__

#include <list>
#include <Common/Typedef.hpp>
#include <Exception.hpp>
#include "Iterator.hpp"

template <class T>
class CList
{
	typedef std::list<T> List;
	List mList;

public:
	inline uint32_t GetSize(void) const;
	inline bool Empty(void) const;
	inline void Clear(void);

	template <class Tn>
	inline void PushFront(const Tn &t);
	template <class T1, class... Tn>
	inline void PushFront(const T1 &t, const Tn &...tn);

	template <class Tn>
	inline void PushBack(const Tn &t);
	template <class T1, class... Tn>
	inline void PushBack(const T1 &t, const Tn &... tn);

	inline void operator += (const T &t);
	inline void operator += (const CList<T> &t);

	inline T PopFront(void);
	template <class T1>
	inline int PopFront(T1 &t);
	template <class T1, class... Tn>
	inline int PopFront(T1 &t, Tn &... tn);

	inline T PopBack(void);
	template <class T1>
	inline int PopBack(T1 &t);
	template <class T1, class... Tn>
	inline int PopBack(T1 &t, Tn &... tn);

	DEFINE_ITERATOR(Iterator, T);
	class Iterator :
		public IteratorBase
	{
		friend IteratorBase;
	private:
		List &mList;
		typename List::iterator mIter;
	
	public:
		inline Iterator(List &list) :
			mList(list)
		{
			/* Does nothing */
		}

	private:
		inline void _Begin(void)
		{
			mIter = mList.begin();
		}

		inline bool _End(void) const
		{
			return mIter == mList.end();
		}

		inline void _Next(void)
		{
			++mIter;
		}

		template <class Fn, class... Tn>
		inline decltype(auto) _Get(const Fn &fn, const Tn & ... tn)
		{
			return fn(*mIter, tn...);
		}

		inline void _Find(const T &t)
		{
			mIter = mList.find(t);
		}

		inline void _Erase(void)
		{
			mList.erase(mIter);
		}

		inline void _Insert(const T &t)
		{
			mList.insert(mIter, t);
		}

		template <class Fn>
		inline void _Sort(const Fn &fn)
		{
			mList.sort(fn);
		}
	};

	inline IteratorPtr Iter(void)
	{
		return IteratorPtr(mList);
	}

private:
	template <class T1>
	inline void PopFrontSafe(T1 &t);
	template <class T1, class... Tn>
	inline void PopFrontSafe(T1 &t, Tn &... tn);

	template <class T1>
	inline void PopBackSafe(T1 &t);
	template <class T1, class... Tn>
	inline void PopBackSafe(T1 &t, Tn &... tn);
};

template <class T>
inline uint32_t CList<T>::GetSize(void) const
{
	return (uint32_t)mList.size();
}

template <class T>
inline bool CList<T>::Empty(void) const
{
	return 0 == mList.size();
}

template <class T>
inline void CList<T>::Clear(void)
{
	return mList.clear();
}

template <class T>
template <class Tn>
inline void CList<T>::PushFront(const Tn &t)
{
	mList.push_front(t);
}

template <class T>
template <class T1, class... Tn>
inline void CList<T>::PushFront(const T1 &t, const Tn &...tn)
{
	mList.push_front(t);
	PushFront(tn...);
}

template <class T>
template <class Tn>
inline void CList<T>::PushBack(const Tn &t)
{
	mList.push_back(t);
}

template <class T>
template <class T1, class... Tn>
inline void CList<T>::PushBack(const T1 &t, const Tn &... tn)
{
	mList.push_back(t);
	PushBack(tn...);
}

template <class T>
inline void CList<T>::operator += (const T &t)
{
	mList.push_back(t);
}

template <class T>
inline void CList<T>::operator += (const CList<T> &t)
{
	auto &list(t.mList);
	for (auto it(list.begin()); it != list.end(); ++it) {
		mList.push_back(*it);
	}
}

template <class T>
inline T CList<T>::PopFront(void)
{
	auto t = mList.front();
	mList.pop_front();
	return t;
}

template <class T>
template <class T1>
inline int CList<T>::PopFront(T1 &t)
{
	if (mList.size() > 0) {
		t = mList.front();
		mList.pop_front();
		return 1;
	}

	return 0;
}

template <class T>
template <class T1, class... Tn>
inline int CList<T>::PopFront(T1 &t, Tn &... tn)
{
	const int num = sizeof...(tn) + 1;

	if (mList.size() >= num) {
		t = mList.front();
		mList.pop_front();
		PopFrontSafe(tn...);
		return num;
	}

	return 0;
}

template <class T>
inline T CList<T>::PopBack(void)
{
	auto t = mList.back();
	mList.pop_back();
	return t;
}

template <class T>
template <class T1>
inline int CList<T>::PopBack(T1 &t)
{
	if (mList.size() > 0) {
		t = mList.back();
		mList.pop_back();
		return 1;
	}

	return 0;
}

template <class T>
template <class T1, class... Tn>
inline int CList<T>::PopBack(T1 &t, Tn &... tn)
{
	const int num = sizeof...(tn) + 1;

	if (mList.size() >= num) {
		t = mList.back();
		mList.pop_back();
		PopBackSafe(tn...);
		return num;
	}

	return 0;
}

template <class T>
template <class T1>
inline void CList<T>::PopFrontSafe(T1 &t)
{
	t = mList.front();
	mList.pop_front();
}

template <class T>
template <class T1, class... Tn>
inline void CList<T>::PopFrontSafe(T1 &t, Tn &... tn)
{
	t = mList.front();
	mList.pop_front();
	PopFrontSafe(tn...);
}

template <class T>
template <class T1>
inline void CList<T>::PopBackSafe(T1 &t)
{
	t = mList.back();
	mList.pop_back();
}

template <class T>
template <class T1, class... Tn>
inline void CList<T>::PopBackSafe(T1 &t, Tn &... tn)
{
	t = mList.back();
	mList.pop_back();
	PopBackSafe(tn...);
}

#endif /* __LIST_HPP__ */

