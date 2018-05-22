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

#ifndef __MAP_HPP__
#define __MAP_HPP__

#include <map>

#include "Iterator.hpp"

template <class Key, class Val>
class CMap
{
	typedef std::map<Key, Val> Map;
private:
	Map mMap;

public:
	inline int GetSize(void)
	{
		return mMap.size();
	}

	inline void Clear(void)
	{
		return mMap.clear();
	}

	inline void Set(const Key &key, const Val &val)
	{
		mMap.insert(std::pair<Key, Val>(key, val));
	}

	inline const Val &Get(const Key &key) const
	{
		return mMap[key];
	}

	inline Val &Get(const Key &key)
	{
		return mMap[key];
	}

	inline Val &operator [] (const Key &key)
	{
		return mMap[key];
	}

	DEFINE_ITERATOR(Iterator, Key, Val);
	class Iterator:
		public IteratorBase
	{
		friend IteratorBase;
	private:
		Map &mMap;
		typename Map::iterator mIter;

	public:
		Iterator(Map &map) :
			mMap(map)
		{
			/* Does nothing */
		}

	private:
		inline void _Begin(void)
		{
			mIter = mMap.begin();
		}

		inline bool _End(void) const
		{
			return mIter == mMap.end();
		}

		inline void _Next(void)
		{
			++mIter;
		}

		template <class Fn, class... Tn>
		inline decltype(auto) _Get(const Fn &fn, const Tn & ... tn)
		{
			return fn(mIter->first, mIter->second, tn...);
		}

		template <class Fn>
		inline decltype(auto) _GetShare(const Fn &fn)
		{
			return fn(mIter->first, mIter->second, IteratorBase::Share());
		}

		inline void _Find(const Key &key)
		{
			mIter = mMap.find(key);
		}

		inline void _Erase(void)
		{
			mMap.erase(mIter);
		}
	};

	inline IteratorPtr Iter(void)
	{
		return IteratorPtr(mMap);
	}
};

#endif /* __MAP_HPP__ */

