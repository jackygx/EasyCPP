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

#include <stdio.h>

#include <Interface/Debug/IDebug.hpp>

class CDebugPrint :
	public IDebug
{
public:
	CDebugPrint(void) {}
	virtual ~CDebugPrint(void) {}

	virtual void Put(char ch)
	{
		putchar(ch);
	}

	virtual void Puts(const char *buf, uint32_t size)
	{
		for (uint32_t i = 0; i < size; ++i)
			putchar(buf[i]);
	}
};

IDebugPtr debugger(new CDebugPrint);

