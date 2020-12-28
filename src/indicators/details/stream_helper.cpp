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
 * @brief Stream helper
 * @author Pranav, SavaLione
 * @date 28 Dec 2020
 */
#include "indicators/details/stream_helper.h"

namespace indicators
{
    namespace details
    {
        inline void set_stream_color(std::ostream &os, Color color)
        {
            switch (color)
            {
            case Color::grey:
                os << termcolor::grey;
                break;
            case Color::red:
                os << termcolor::red;
                break;
            case Color::green:
                os << termcolor::green;
                break;
            case Color::yellow:
                os << termcolor::yellow;
                break;
            case Color::blue:
                os << termcolor::blue;
                break;
            case Color::magenta:
                os << termcolor::magenta;
                break;
            case Color::cyan:
                os << termcolor::cyan;
                break;
            case Color::white:
                os << termcolor::white;
                break;
            default:
                assert(false);
                break;
            }
        }

        inline void set_font_style(std::ostream &os, FontStyle style)
        {
            switch (style)
            {
            case FontStyle::bold:
                os << termcolor::bold;
                break;
            case FontStyle::dark:
                os << termcolor::dark;
                break;
            case FontStyle::italic:
                os << termcolor::italic;
                break;
            case FontStyle::underline:
                os << termcolor::underline;
                break;
            case FontStyle::blink:
                os << termcolor::blink;
                break;
            case FontStyle::reverse:
                os << termcolor::reverse;
                break;
            case FontStyle::concealed:
                os << termcolor::concealed;
                break;
            case FontStyle::crossed:
                os << termcolor::crossed;
                break;
            default:
                break;
            }
        }

        inline std::ostream &write_duration(std::ostream &os, std::chrono::nanoseconds ns)
        {
            using namespace std;
            using namespace std::chrono;
            using days = duration<int, ratio<86400>>;
            char fill = os.fill();
            os.fill('0');
            auto d = duration_cast<days>(ns);
            ns -= d;
            auto h = duration_cast<hours>(ns);
            ns -= h;
            auto m = duration_cast<minutes>(ns);
            ns -= m;
            auto s = duration_cast<seconds>(ns);
            if (d.count() > 0)
                os << setw(2) << d.count() << "d:";
            if (h.count() > 0)
                os << setw(2) << h.count() << "h:";
            os << setw(2) << m.count() << "m:" << setw(2) << s.count() << 's';
            os.fill(fill);
            return os;
        }
    } // namespace details
} // namespace indicators