/*
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * Copyright (c) 2020, Savely Pototsky (SavaLione)
 * Copyright (c) 2019, Pranav
 * Copyright (c) 2013, Ihor Kalnytskyi
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
 * @brief Terminal color
 * @author Ihor Kalnytskyi, Pranav, SavaLione
 * @date 28 Dec 2020
 */
#ifndef INDICATORS_TERMCOLOR_H
#define INDICATORS_TERMCOLOR_H

/*
    the following snippet of code detects the current OS and
    defines the appropriate macro that is used to wrap some
    platform specific things
*/
#if defined(_WIN32) || defined(_WIN64)
#define TERMCOLOR_OS_WINDOWS
#elif defined(__APPLE__)
#define TERMCOLOR_OS_MACOS
#elif defined(__unix__) || defined(__unix)
#define TERMCOLOR_OS_LINUX
#else
#error unsupported platform
#endif

/*
    This headers provides the `isatty()`/`fileno()` functions,
    which are used for testing whether a standart stream refers
    to the terminal. As for Windows, we also need WinApi funcs
    for changing colors attributes of the terminal.
*/
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
#include <unistd.h>
#elif defined(TERMCOLOR_OS_WINDOWS)
#if defined(_MSC_VER)
#if !defined(NOMINMAX)
#define NOMINMAX
#endif
#include <io.h>
#include <windows.h>
#endif
#endif

#include <iostream>
#include <cstdio>

namespace termcolor
{
    /*
        Forward declaration of the `_internal` namespace.
        All comments are below.
    */
    namespace _internal
    {
        /*
            An index to be used to access a private storage of I/O streams. See
            colorize / nocolorize I/O manipulators for details.
        */
        static int colorize_index = std::ios_base::xalloc();

        inline FILE *get_standard_stream(const std::ostream &stream);
        inline bool is_colorized(std::ostream &stream);
        inline bool is_atty(const std::ostream &stream);

#if defined(TERMCOLOR_OS_WINDOWS)
        inline void win_change_attributes(std::ostream &stream, int foreground, int background = -1);
#endif
    } // namespace _internal

    std::ostream &colorize(std::ostream &stream);
    std::ostream &nocolorize(std::ostream &stream);
    std::ostream &reset(std::ostream &stream);
    std::ostream &bold(std::ostream &stream);
    std::ostream &dark(std::ostream &stream);
    std::ostream &italic(std::ostream &stream);
    std::ostream &underline(std::ostream &stream);
    std::ostream &blink(std::ostream &stream);
    std::ostream &reverse(std::ostream &stream);
    std::ostream &concealed(std::ostream &stream);
    std::ostream &crossed(std::ostream &stream);

    template <uint8_t code>
    inline std::ostream &color(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            char command[12];
            std::snprintf(command, sizeof(command), "\033[38;5;%dm", code);
            stream << command;
#elif defined(TERMCOLOR_OS_WINDOWS)
#endif
        }
        return stream;
    }

    template <uint8_t code>
    inline std::ostream &on_color(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            char command[12];
            std::snprintf(command, sizeof(command), "\033[48;5;%dm", code);
            stream << command;
#elif defined(TERMCOLOR_OS_WINDOWS)
#endif
        }
        return stream;
    }

    template <uint8_t r, uint8_t g, uint8_t b>
    inline std::ostream &color(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            char command[20];
            std::snprintf(command, sizeof(command), "\033[38;2;%d;%d;%dm", r, g, b);
            stream << command;
#elif defined(TERMCOLOR_OS_WINDOWS)
#endif
        }
        return stream;
    }

    template <uint8_t r, uint8_t g, uint8_t b>
    inline std::ostream &on_color(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            char command[20];
            std::snprintf(command, sizeof(command), "\033[48;2;%d;%d;%dm", r, g, b);
            stream << command;
#elif defined(TERMCOLOR_OS_WINDOWS)
#endif
        }
        return stream;
    }

    std::ostream &grey(std::ostream &stream);
    std::ostream &red(std::ostream &stream);
    std::ostream &green(std::ostream &stream);
    std::ostream &yellow(std::ostream &stream);
    std::ostream &blue(std::ostream &stream);
    std::ostream &magenta(std::ostream &stream);
    std::ostream &cyan(std::ostream &stream);
    std::ostream &white(std::ostream &stream);
    std::ostream &on_grey(std::ostream &stream);
    std::ostream &on_red(std::ostream &stream);
    std::ostream &on_green(std::ostream &stream);
    std::ostream &on_yellow(std::ostream &stream);
    std::ostream &on_blue(std::ostream &stream);
    std::ostream &on_magenta(std::ostream &stream);
    std::ostream &on_cyan(std::ostream &stream);
    std::ostream &on_white(std::ostream &stream);

    /*
        Since C++ hasn't a way to hide something in the header from
        the outer access, I have to introduce this namespace which
        is used for internal purpose and should't be access from
        the user code.
    */
    namespace _internal
    {
        /*
            Since C++ hasn't a true way to extract stream handler
            from the a given `std::ostream` object, I have to write
            this kind of hack.
        */
        FILE *get_standard_stream(const std::ostream &stream);

        /*
            Say whether a given stream should be colorized or not. It's always
            true for ATTY streams and may be true for streams marked with
            colorize flag.
        */
        bool is_colorized(std::ostream &stream);

        /*
            Test whether a given `std::ostream` object refers to
            a terminal.
        */
        bool is_atty(const std::ostream &stream);

#if defined(TERMCOLOR_OS_WINDOWS)
        /*
            Change Windows Terminal colors attribute. If some
            parameter is `-1` then attribute won't changed.
        */
        void win_change_attributes(std::ostream &stream, int foreground, int background);
#endif // TERMCOLOR_OS_WINDOWS

    } // namespace _internal

} // namespace termcolor

#undef TERMCOLOR_OS_WINDOWS
#undef TERMCOLOR_OS_MACOS
#undef TERMCOLOR_OS_LINUX

#endif // INDICATORS_TERMCOLOR_H