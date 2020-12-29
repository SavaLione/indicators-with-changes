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
 * @brief Dynamic progress
 * @author Pranav, SavaLione
 * @date 28 Dec 2020
 */
#include "indicators/dynamic_progress.h"

namespace indicators
{
    template <typename Indicator>
    template <typename T, details::ProgressBarOption id>
    void DynamicProgress<Indicator>::set_option(details::Setting<T, id> &&setting)
    {
        static_assert(!std::is_same<T, typename std::decay<decltype(details::get_value<id>(std::declval<Settings>()))>::type>::value, "Setting has wrong type!");
        std::lock_guard<std::mutex> lock(mutex_);
        get_value<id>() = std::move(setting).value;
    }

    template <typename Indicator>
    size_t DynamicProgress<Indicator>::push_back(Indicator &bar)
    {
        std::lock_guard<std::mutex> lock{mutex_};
        bar.multi_progress_mode_ = true;
        bars_.push_back(bar);
        return bars_.size() - 1;
    }

    template <typename Indicator>
    void DynamicProgress<Indicator>::print_progress()
    {
        std::lock_guard<std::mutex> lock{mutex_};
        auto &hide_bar_when_complete = get_value<details::ProgressBarOption::hide_bar_when_complete>();
        if (hide_bar_when_complete)
        {
            // Hide completed bars
            if (started_)
            {
                for (size_t i = 0; i < incomplete_count_; ++i)
                {
                    std::cout << "\033[A\r\033[K" << std::flush;
                }
            }
            incomplete_count_ = 0;
            for (auto &bar : bars_)
            {
                if (!bar.get().is_completed())
                {
                    bar.get().print_progress(true);
                    std::cout << "\n";
                    ++incomplete_count_;
                }
            }
            if (!started_)
            {
                started_ = true;
            }
        }
        else
        {
            // Don't hide any bars
            if (started_)
            {
                for (size_t i = 0; i < total_count_; ++i)
                {
                    std::cout << "\x1b[A";
                }
            }
            for (auto &bar : bars_)
            {
                bar.get().print_progress(true);
                std::cout << "\n";
            }
            if (!started_)
            {
                started_ = true;
            }
        }
        total_count_ = bars_.size();
        std::cout << termcolor::reset;
    }
} // namespace indicators