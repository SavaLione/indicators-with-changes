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
#ifndef INDICATORS_DYNAMIC_PROGRESS_H
#define INDICATORS_DYNAMIC_PROGRESS_H

#include <atomic>
#include <functional>
#include <iostream>
#include <mutex>
#include <vector>

#include <indicators/color.h>
#include <indicators/setting.h>
#include <indicators/details/stream_helper.h>

namespace indicators
{
    template <typename Indicator>
    class DynamicProgress
    {
        using Settings = std::tuple<option::HideBarWhenComplete>;

    public:
        template <typename... Indicators>
        explicit DynamicProgress(Indicators &... bars)
        {
            bars_ = {bars...};
            for (auto &bar : bars_)
            {
                bar.get().multi_progress_mode_ = true;
                ++total_count_;
                ++incomplete_count_;
            }
        }

        Indicator &operator[](size_t index)
        {
            print_progress();
            std::lock_guard<std::mutex> lock{mutex_};
            return bars_[index].get();
        }

        size_t push_back(Indicator &bar);

        template <typename T, details::ProgressBarOption id>
        void set_option(details::Setting<T, id> &&setting)
        {
            static_assert(!std::is_same<T, typename std::decay<decltype(details::get_value<id>(std::declval<Settings>()))>::type>::value, "Setting has wrong type!");
            std::lock_guard<std::mutex> lock(mutex_);
            get_value<id>() = std::move(setting).value;
        }

        template <typename T, details::ProgressBarOption id>
        void set_option(const details::Setting<T, id> &setting)
        {
            static_assert(!std::is_same<T, typename std::decay<decltype(details::get_value<id>(std::declval<Settings>()))>::type>::value, "Setting has wrong type!");
            std::lock_guard<std::mutex> lock(mutex_);
            get_value<id>() = setting.value;
        }

    private:
        Settings settings_;
        std::atomic<bool> started_{false};
        std::mutex mutex_;
        std::vector<std::reference_wrapper<Indicator>> bars_;
        std::atomic<size_t> total_count_{0};
        std::atomic<size_t> incomplete_count_{0};

        template <details::ProgressBarOption id>
        auto get_value() -> decltype((details::get_value<id>(std::declval<Settings &>()).value))
        {
            return details::get_value<id>(settings_).value;
        }

        template <details::ProgressBarOption id>
        auto get_value() const
            -> decltype((details::get_value<id>(std::declval<const Settings &>()).value))
        {
            return details::get_value<id>(settings_).value;
        }

    public:
        void print_progress();
    };
} // namespace indicators

#endif // INDICATORS_DYNAMIC_PROGRESS_H