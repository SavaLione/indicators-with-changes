/*
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * Copyright (c) 2020, Savely Pototsky (SavaLione)
 * Copyright (c) 2019, Pranav
 * Based on: indicators (https://github.com/p-ranav/indicators)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * @file
 * @brief Cursor movement
 * @author Pranav, SavaLione
 * @date 28 Dec 2020
 */
#include "indicators/cursor_movement.h"

#if defined(_MSC_VER)
#if !defined(NOMINMAX)
#define NOMINMAX
#endif
#include <io.h>
#include <windows.h>
#else
#include <iostream>
#endif

namespace indicators
{

#ifdef _MSC_VER

    void move(int x, int y)
    {
        auto hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
        if (!hStdout)
        {
            return;
        }

        CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
        GetConsoleScreenBufferInfo(hStdout, &csbiInfo);

        COORD cursor;

        cursor.X = csbiInfo.dwCursorPosition.X + x;
        cursor.Y = csbiInfo.dwCursorPosition.Y + y;
        SetConsoleCursorPosition(hStdout, cursor);
    }

    void move_up(int lines)
    {
        move(0, -lines);
    }

    void move_down(int lines)
    {
        move(0, -lines);
    }

    void move_right(int cols)
    {
        move(cols, 0);
    }

    void move_left(int cols)
    {
        move(-cols, 0);
    }

#else
    void move_up(int lines)
    {
        std::cout << "\033[" << lines << "A";
    }

    void move_down(int lines)
    {
        std::cout << "\033[" << lines << "B";
    }

    void move_right(int cols)
    {
        std::cout << "\033[" << cols << "C";
    }

    void move_left(int cols)
    {
        std::cout << "\033[" << cols << "D";
    }
#endif

} // namespace indicators