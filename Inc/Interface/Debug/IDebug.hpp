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

#ifndef __IDEBUG_HPP__
#define __IDEBUG_HPP__

#include <Interface/Interface.hpp>

DEFINE_INTERFACE(Debug);

class IDebug
{
public:
	IDebug(void) {}
	virtual ~IDebug(void) {}

	virtual void Put(char ch) = 0;
	virtual void Puts(const char *buf, uint32_t size) = 0;
};

#endif /* __IDEBUG_H__ */

