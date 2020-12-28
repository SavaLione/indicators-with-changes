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
 * @author Pranav, SavaLione
 * @date 28 Dec 2020
 */
#include "indicators/termcolor.h"

#include <iostream>
#include <cstdio>

namespace termcolor
{
    inline std::ostream &colorize(std::ostream &stream)
    {
        stream.iword(_internal::colorize_index) = 1L;
        return stream;
    }

    inline std::ostream &nocolorize(std::ostream &stream)
    {
        stream.iword(_internal::colorize_index) = 0L;
        return stream;
    }

    inline std::ostream &reset(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[00m";
#elif defined(TERMCOLOR_OS_WINDOWS)
            _internal::win_change_attributes(stream, -1, -1);
#endif
        }
        return stream;
    }

    inline std::ostream &bold(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[1m";
#elif defined(TERMCOLOR_OS_WINDOWS)
#endif
        }
        return stream;
    }

    inline std::ostream &dark(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[2m";
#elif defined(TERMCOLOR_OS_WINDOWS)
#endif
        }
        return stream;
    }

    inline std::ostream &italic(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[3m";
#elif defined(TERMCOLOR_OS_WINDOWS)
#endif
        }
        return stream;
    }

    inline std::ostream &underline(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[4m";
#elif defined(TERMCOLOR_OS_WINDOWS)
#endif
        }
        return stream;
    }

    inline std::ostream &blink(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[5m";
#elif defined(TERMCOLOR_OS_WINDOWS)
#endif
        }
        return stream;
    }

    inline std::ostream &reverse(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[7m";
#elif defined(TERMCOLOR_OS_WINDOWS)
#endif
        }
        return stream;
    }

    inline std::ostream &concealed(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[8m";
#elif defined(TERMCOLOR_OS_WINDOWS)
#endif
        }
        return stream;
    }

    inline std::ostream &crossed(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[9m";
#elif defined(TERMCOLOR_OS_WINDOWS)
#endif
        }
        return stream;
    }

    /* here need template code */

    inline std::ostream &grey(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[30m";
#elif defined(TERMCOLOR_OS_WINDOWS)
            _internal::win_change_attributes(stream,
                                             0 // grey (black)
            );
#endif
        }
        return stream;
    }

    inline std::ostream &red(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[31m";
#elif defined(TERMCOLOR_OS_WINDOWS)
            _internal::win_change_attributes(stream,
                                             FOREGROUND_RED);
#endif
        }
        return stream;
    }

    inline std::ostream &green(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[32m";
#elif defined(TERMCOLOR_OS_WINDOWS)
            _internal::win_change_attributes(stream,
                                             FOREGROUND_GREEN);
#endif
        }
        return stream;
    }

    inline std::ostream &yellow(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[33m";
#elif defined(TERMCOLOR_OS_WINDOWS)
            _internal::win_change_attributes(stream,
                                             FOREGROUND_GREEN | FOREGROUND_RED);
#endif
        }
        return stream;
    }

    inline std::ostream &blue(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[34m";
#elif defined(TERMCOLOR_OS_WINDOWS)
            _internal::win_change_attributes(stream,
                                             FOREGROUND_BLUE);
#endif
        }
        return stream;
    }

    inline std::ostream &magenta(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[35m";
#elif defined(TERMCOLOR_OS_WINDOWS)
            _internal::win_change_attributes(stream,
                                             FOREGROUND_BLUE | FOREGROUND_RED);
#endif
        }
        return stream;
    }

    inline std::ostream &cyan(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[36m";
#elif defined(TERMCOLOR_OS_WINDOWS)
            _internal::win_change_attributes(stream,
                                             FOREGROUND_BLUE | FOREGROUND_GREEN);
#endif
        }
        return stream;
    }

    inline std::ostream &white(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[37m";
#elif defined(TERMCOLOR_OS_WINDOWS)
            _internal::win_change_attributes(stream,
                                             FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
#endif
        }
        return stream;
    }

    inline std::ostream &on_grey(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[40m";
#elif defined(TERMCOLOR_OS_WINDOWS)
            _internal::win_change_attributes(stream, -1,
                                             0 // grey (black)
            );
#endif
        }
        return stream;
    }

    inline std::ostream &on_red(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[41m";
#elif defined(TERMCOLOR_OS_WINDOWS)
            _internal::win_change_attributes(stream, -1,
                                             BACKGROUND_RED);
#endif
        }
        return stream;
    }

    inline std::ostream &on_green(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[42m";
#elif defined(TERMCOLOR_OS_WINDOWS)
            _internal::win_change_attributes(stream, -1,
                                             BACKGROUND_GREEN);
#endif
        }
        return stream;
    }

    inline std::ostream &on_yellow(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[43m";
#elif defined(TERMCOLOR_OS_WINDOWS)
            _internal::win_change_attributes(stream, -1,
                                             BACKGROUND_GREEN | BACKGROUND_RED);
#endif
        }
        return stream;
    }

    inline std::ostream &on_blue(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[44m";
#elif defined(TERMCOLOR_OS_WINDOWS)
            _internal::win_change_attributes(stream, -1,
                                             BACKGROUND_BLUE);
#endif
        }
        return stream;
    }

    inline std::ostream &on_magenta(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[45m";
#elif defined(TERMCOLOR_OS_WINDOWS)
            _internal::win_change_attributes(stream, -1,
                                             BACKGROUND_BLUE | BACKGROUND_RED);
#endif
        }
        return stream;
    }

    inline std::ostream &on_cyan(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[46m";
#elif defined(TERMCOLOR_OS_WINDOWS)
            _internal::win_change_attributes(stream, -1,
                                             BACKGROUND_GREEN | BACKGROUND_BLUE);
#endif
        }
        return stream;
    }

    inline std::ostream &on_white(std::ostream &stream)
    {
        if (_internal::is_colorized(stream))
        {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            stream << "\033[47m";
#elif defined(TERMCOLOR_OS_WINDOWS)
            _internal::win_change_attributes(stream, -1,
                                             BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_RED);
#endif
        }
        return stream;
    }

    namespace _internal
    {
        inline FILE *get_standard_stream(const std::ostream &stream)
        {
            if (&stream == &std::cout)
                return stdout;
            else if ((&stream == &std::cerr) || (&stream == &std::clog))
                return stderr;

            return nullptr;
        }

        inline bool is_colorized(std::ostream &stream)
        {
            return is_atty(stream) || static_cast<bool>(stream.iword(colorize_index));
        }

        inline bool is_atty(const std::ostream &stream)
        {
            FILE *std_stream = get_standard_stream(stream);

            /*
                Unfortunately, fileno() ends with segmentation fault
                if invalid file descriptor is passed. So we need to
                handle this case gracefully and assume it's not a tty
                if standard stream is not detected, and 0 is returned.
            */
            if (!std_stream)
            {
                return false;
            }

#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
            return ::isatty(fileno(std_stream));
#elif defined(TERMCOLOR_OS_WINDOWS)
            return ::_isatty(_fileno(std_stream));
#endif
        }

#if defined(TERMCOLOR_OS_WINDOWS)
        //! Change Windows Terminal colors attribute. If some
        //! parameter is `-1` then attribute won't changed.
        inline void win_change_attributes(std::ostream &stream, int foreground, int background)
        {
            // yeah, i know.. it's ugly, it's windows.
            static WORD defaultAttributes = 0;

            // Windows doesn't have ANSI escape sequences and so we use special
            // API to change Terminal output color. That means we can't
            // manipulate colors by means of "std::stringstream" and hence
            // should do nothing in this case.
            if (!_internal::is_atty(stream))
                return;

            // get terminal handle
            HANDLE hTerminal = INVALID_HANDLE_VALUE;
            if (&stream == &std::cout)
                hTerminal = GetStdHandle(STD_OUTPUT_HANDLE);
            else if (&stream == &std::cerr)
                hTerminal = GetStdHandle(STD_ERROR_HANDLE);

            // save default terminal attributes if it unsaved
            if (!defaultAttributes)
            {
                CONSOLE_SCREEN_BUFFER_INFO info;
                if (!GetConsoleScreenBufferInfo(hTerminal, &info))
                    return;
                defaultAttributes = info.wAttributes;
            }

            // restore all default settings
            if (foreground == -1 && background == -1)
            {
                SetConsoleTextAttribute(hTerminal, defaultAttributes);
                return;
            }

            // get current settings
            CONSOLE_SCREEN_BUFFER_INFO info;
            if (!GetConsoleScreenBufferInfo(hTerminal, &info))
                return;

            if (foreground != -1)
            {
                info.wAttributes &= ~(info.wAttributes & 0x0F);
                info.wAttributes |= static_cast<WORD>(foreground);
            }

            if (background != -1)
            {
                info.wAttributes &= ~(info.wAttributes & 0xF0);
                info.wAttributes |= static_cast<WORD>(background);
            }

            SetConsoleTextAttribute(hTerminal, info.wAttributes);
        }
#endif // TERMCOLOR_OS_WINDOWS
    }  // namespace _internal

} // namespace termcolor