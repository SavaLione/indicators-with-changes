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
 * @brief Progress spinner
 * @author Pranav, SavaLione
 * @date 29 Dec 2020
 */
#ifndef INDICATORS_PROGRESS_SPINNER_H
#define INDICATORS_PROGRESS_SPINNER_H

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

#include <indicators/color.h>
#include <indicators/setting.h>
#include <indicators/details/stream_helper.h>

namespace indicators
{
    class ProgressSpinner
    {
        using Settings =
            std::tuple<option::ForegroundColor, option::PrefixText, option::PostfixText,
                       option::ShowPercentage, option::ShowElapsedTime, option::ShowRemainingTime,
                       option::ShowSpinner, option::SavedStartTime, option::Completed,
                       option::MaxPostfixTextLen, option::SpinnerStates, option::FontStyles,
                       option::MaxProgress, option::Stream>;

    public:
        template <typename... Args, typename std::enable_if<details::are_settings_from_tuple<Settings, typename std::decay<Args>::type...>::value, void *>::type = nullptr>
        explicit ProgressSpinner(Args &&... args)
            : settings_(
                  details::get<details::ProgressBarOption::foreground_color>(option::ForegroundColor{Color::unspecified}, std::forward<Args>(args)...),
                  details::get<details::ProgressBarOption::prefix_text>(option::PrefixText{}, std::forward<Args>(args)...),
                  details::get<details::ProgressBarOption::postfix_text>(option::PostfixText{}, std::forward<Args>(args)...),
                  details::get<details::ProgressBarOption::show_percentage>(option::ShowPercentage{true}, std::forward<Args>(args)...),
                  details::get<details::ProgressBarOption::show_elapsed_time>(option::ShowElapsedTime{false}, std::forward<Args>(args)...),
                  details::get<details::ProgressBarOption::show_remaining_time>(option::ShowRemainingTime{false}, std::forward<Args>(args)...),
                  details::get<details::ProgressBarOption::spinner_show>(option::ShowSpinner{true}, std::forward<Args>(args)...),
                  details::get<details::ProgressBarOption::saved_start_time>(option::SavedStartTime{false}, std::forward<Args>(args)...),
                  details::get<details::ProgressBarOption::completed>(option::Completed{false}, std::forward<Args>(args)...),
                  details::get<details::ProgressBarOption::max_postfix_text_len>(option::MaxPostfixTextLen{0}, std::forward<Args>(args)...),
                  details::get<details::ProgressBarOption::spinner_states>(option::SpinnerStates{std::vector<std::string>{"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"}}, std::forward<Args>(args)...),
                  details::get<details::ProgressBarOption::font_styles>(option::FontStyles{std::vector<FontStyle>{}}, std::forward<Args>(args)...),
                  details::get<details::ProgressBarOption::max_progress>(option::MaxProgress{100}, std::forward<Args>(args)...),
                  details::get<details::ProgressBarOption::stream>(option::Stream{std::cout}, std::forward<Args>(args)...)) {}

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

        void set_option( const details::Setting<std::string, details::ProgressBarOption::postfix_text> &setting);

        void set_option(details::Setting<std::string, details::ProgressBarOption::postfix_text> &&setting);

        void set_progress(size_t value);

        void tick();

        size_t current();

        bool is_completed() const { return get_value<details::ProgressBarOption::completed>(); }

        void mark_as_completed();

    private:
        Settings settings_;
        size_t progress_{0};
        size_t index_{0};
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time_point_;
        std::mutex mutex_;

        template <details::ProgressBarOption id>
        auto get_value() -> decltype((details::get_value<id>(std::declval<Settings &>()).value))
        {
            return details::get_value<id>(settings_).value;
        }

        template <details::ProgressBarOption id>
        auto get_value() const -> decltype((details::get_value<id>(std::declval<const Settings &>()).value))
        {
            return details::get_value<id>(settings_).value;
        }

        void save_start_time();

    public:
        void print_progress();
    };
} // namespace indicators

#endif // INDICATORS_PROGRESS_SPINNER_H