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
 * @brief Block progress scale writer
 * @author Pranav, SavaLione
 * @date 28 Dec 2020
 */
#include "indicators/details/block_progress_scale_writer.h"

namespace indicators
{
    namespace details
    {
        std::ostream &BlockProgressScaleWriter::write(float progress)
        {
            std::string fill_text{"█"};
            std::vector<std::string> lead_characters{" ", "▏", "▎", "▍", "▌", "▋", "▊", "▉"};
            auto value = std::min(1.0f, std::max(0.0f, progress / 100.0f));
            auto whole_width = std::floor(value * bar_width);
            auto remainder_width = fmod((value * bar_width), 1.0f);
            auto part_width = std::floor(remainder_width * lead_characters.size());
            std::string lead_text = lead_characters[size_t(part_width)];
            if ((bar_width - whole_width - 1) < 0)
            {
                lead_text = "";
            }
            for (size_t i = 0; i < whole_width; ++i)
            {
                os << fill_text;
            }
            os << lead_text;
            for (size_t i = 0; i < (bar_width - whole_width - 1); ++i)
            {
                os << " ";
            }
            return os;
        }
    } // namespace details
} // namespace indicators