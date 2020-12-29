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
 * @brief Multi progress
 * @author Pranav, SavaLione
 * @date 29 Dec 2020
 */
#ifndef INDICATORS_MULTI_PROGRESS_H
#define INDICATORS_MULTI_PROGRESS_H

#include <atomic>
#include <functional>
#include <iostream>
#include <mutex>
#include <vector>

#include <indicators/color.h>
#include <indicators/cursor_movement.h>
#include <indicators/details/stream_helper.h>

namespace indicators
{
    template <typename Indicator, size_t count>
    class MultiProgress
    {
    public:
        template <typename... Indicators, typename = typename std::enable_if<(sizeof...(Indicators) == count)>::type>
        explicit MultiProgress(Indicators &... bars)
        {
            bars_ = {bars...};
            for (auto &bar : bars_)
            {
                bar.get().multi_progress_mode_ = true;
            }
        }

        template <size_t index>
        typename std::enable_if<(index >= 0 && index < count), void>::type set_progress(size_t value)
        {
            if (!bars_[index].get().is_completed())
            {
                bars_[index].get().set_progress(value);
            }
            print_progress();
        }

        template <size_t index>
        typename std::enable_if<(index >= 0 && index < count), void>::type set_progress(float value)
        {
            if (!bars_[index].get().is_completed())
            {
                bars_[index].get().set_progress(value);
            }
            print_progress();
        }

        template <size_t index>
        typename std::enable_if<(index >= 0 && index < count), void>::type tick()
        {
            if (!bars_[index].get().is_completed())
            {
                bars_[index].get().tick();
            }
            print_progress();
        }

        template <size_t index>
        typename std::enable_if<(index >= 0 && index < count), bool>::type is_completed() const
        {
            return bars_[index].get().is_completed();
        }

    private:
        std::atomic<bool> started_{false};
        std::mutex mutex_;
        std::vector<std::reference_wrapper<Indicator>> bars_;

        bool _all_completed();

    public:
        void print_progress();
    };
} // namespace indicators

#endif // INDICATORS_MULTI_PROGRESS_H