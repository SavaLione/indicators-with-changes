/*
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * Copyright (c) 2020, Savely Pototsky (SavaLione)
 * Copyright (c) 2019, Pranav
 * Copyright (c) 2007, Markus Kuhn
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
 * @brief Display width
 * @author Pranav, SavaLione
 * @date 28 Dec 2020
 */
#ifndef INDICATORS_DISPLAY_WIDTH_H
#define INDICATORS_DISPLAY_WIDTH_H

#include <clocale>
#if __has_include(<codecvt>)
#include <codecvt>
#define INDICATORS_HAVE_CODECVT 1
#endif

namespace unicode
{
#if INDICATORS_HAVE_CODECVT
    namespace details
    {
        /*
            This is an implementation of wcwidth() and wcswidth() (defined in
            IEEE Std 1002.1-2001) for Unicode.

            http://www.opengroup.org/onlinepubs/007904975/functions/wcwidth.html
            http://www.opengroup.org/onlinepubs/007904975/functions/wcswidth.html

            In fixed-width output devices, Latin characters all occupy a single
            "cell" position of equal width, whereas ideographic CJK characters
            occupy two such cells. Interoperability between terminal-line
            applications and (teletype-style) character terminals using the
            UTF-8 encoding requires agreement on which character should advance
            the cursor by how many cell positions. No established formal
            standards exist at present on which Unicode character shall occupy
            how many cell positions on character terminals. These routines are
            a first attempt of defining such behavior based on simple rules
            applied to data provided by the Unicode Consortium.

            For some graphical characters, the Unicode standard explicitly
            defines a character-cell width via the definition of the East Asian
            FullWidth (F), Wide (W), Half-width (H), and Narrow (Na) classes.
            In all these cases, there is no ambiguity about which width a
            terminal shall use. For characters in the East Asian Ambiguous (A)
            class, the width choice depends purely on a preference of backward
            compatibility with either historic CJK or Western practice.
            Choosing single-width for these characters is easy to justify as
            the appropriate long-term solution, as the CJK practice of
            displaying these characters as double-width comes from historic
            implementation simplicity (8-bit encoded characters were displayed
            single-width and 16-bit ones double-width, even for Greek,
            Cyrillic, etc.) and not any typographic considerations.

            Much less clear is the choice of width for the Not East Asian
            (Neutral) class. Existing practice does not dictate a width for any
            of these characters. It would nevertheless make sense
            typographically to allocate two character cells to characters such
            as for instance EM SPACE or VOLUME INTEGRAL, which cannot be
            represented adequately with a single-width glyph. The following
            routines at present merely assign a single-cell width to all
            neutral characters, in the interest of simplicity. This is not
            entirely satisfactory and should be reconsidered before
            establishing a formal standard in this area. At the moment, the
            decision which Not East Asian (Neutral) characters should be
            represented by double-width glyphs cannot yet be answered by
            applying a simple rule from the Unicode database content. Setting
            up a proper standard for the behavior of UTF-8 character terminals
            will require a careful analysis not only of each Unicode character,
            but also of each presentation form, something the author of these
            routines has avoided to do so far.

            http://www.unicode.org/unicode/reports/tr11/

            Markus Kuhn -- 2007-05-26 (Unicode 5.0)

            Permission to use, copy, modify, and distribute this software
            for any purpose and without fee is hereby granted. The author
            disclaims all warranties with regard to this software.

            Latest version: http://www.cl.cam.ac.uk/~mgk25/ucs/wcwidth.c
        */
        struct interval
        {
            int first;
            int last;
        };

        /* auxiliary function for binary search in interval table */
        int bisearch(wchar_t ucs, const struct interval *table, int max);

        /*
            The following two functions define the column width of an ISO 10646
            character as follows:

               - The null character (U+0000) has a column width of 0.

               - Other C0/C1 control characters and DEL will lead to a return
                 value of -1.

               - Non-spacing and enclosing combining characters (general
                 category code Mn or Me in the Unicode database) have a
                 column width of 0.

               - SOFT HYPHEN (U+00AD) has a column width of 1.

               - Other format characters (general category code Cf in the Unicode
                 database) and ZERO WIDTH SPACE (U+200B) have a column width of 0.

               - Hangul Jamo medial vowels and final consonants (U+1160-U+11FF)
                 have a column width of 0.

               - Spacing characters in the East Asian Wide (W) or East Asian
                 Full-width (F) category as defined in Unicode Technical
                 Report #11 have a column width of 2.

               - All remaining characters (including all printable
                 ISO 8859-1 and WGL4 characters, Unicode control characters,
                 etc.) have a column width of 1.

            This implementation assumes that wchar_t characters are encoded
            in ISO 10646.
        */
        int mk_wcwidth(wchar_t ucs);
        int mk_wcswidth(const wchar_t *pwcs, size_t n);

        /*
            The following functions are the same as mk_wcwidth() and
            mk_wcswidth(), except that spacing characters in the East Asian
            Ambiguous (A) category as defined in Unicode Technical Report #11
            have a column width of 2. This variant might be useful for users of
            CJK legacy encodings who want to migrate to UCS without changing
            the traditional terminal character-width behaviour. It is not
            otherwise recommended for general use.
        */
        int mk_wcwidth_cjk(wchar_t ucs);

        int mk_wcswidth_cjk(const wchar_t *pwcs, size_t n);

        /* convert UTF-8 string to wstring */
        std::wstring utf8_decode(const std::string &str);

        /* convert wstring to UTF-8 string */
        std::string utf8_encode(const std::wstring &str);

    } // namespace details

#endif

    int display_width(const std::string &input);
    int display_width(const std::wstring &input);

} // namespace unicode

#endif // INDICATORS_DISPLAY_WIDTH_H