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

#include <EasyCpp.hpp>

void DebugDump(const CConstStringPtr &str, uint32_t shead, uint32_t stail)
{
	uint32_t size = str->GetSize();
	shead = shead < size ? shead : size;

	for (uint32_t i = 0, j = 0; i < shead; ++i, ++j) {
		uint8_t byte = str->Dump<uint8_t>(i);
		putchar(HexMap[byte >> 4]);
		putchar(HexMap[byte & 0xF]);

		if (15 == j) {
			putchar('\n');
			j = -1;
		} else {
			putchar(' ');
		}
	}

	if (stail > 0) {
		putchar('\n');
		putchar('.');
		putchar('.');
		putchar('.');
		putchar('\n');

		stail = stail < size ? size - stail : 0;

		for (uint32_t i = stail, j = 0; i < size; ++i, ++j) {
			uint8_t byte = str->Dump<uint8_t>(i);
			putchar(HexMap[byte >> 4]);
			putchar(HexMap[byte & 0xF]);

			if (15 == j) {
				putchar('\n');
				j = -1;
			} else {
				putchar(' ');
			}
		}
	}

	putchar('\n');
}

