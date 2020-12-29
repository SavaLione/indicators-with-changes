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
 * @brief Indeterminate progress bar
 * @author Pranav, SavaLione
 * @date 28 Dec 2020
 */
#ifndef INDICATORS_INDETERMINATE_PROGRESS_BAR_H
#define INDICATORS_INDETERMINATE_PROGRESS_BAR_H

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
#include <type_traits>
#include <sstream>
#include <utility>

#include <indicators/color.h>
#include <indicators/setting.h>
#include <indicators/terminal_size.h>
#include <indicators/details/stream_helper.h>

namespace indicators
{
    class IndeterminateProgressBar
    {
        using Settings = std::tuple<option::BarWidth, option::PrefixText, option::PostfixText, option::Start,
                                    option::End, option::Fill, option::Lead, option::MaxPostfixTextLen,
                                    option::Completed, option::ForegroundColor, option::FontStyles, option::Stream>;

        enum class Direction
        {
            forward,
            backward
        };

        Direction direction_{Direction::forward};

    public:
        template <typename... Args, typename std::enable_if<details::are_settings_from_tuple<Settings, typename std::decay<Args>::type...>::value, void *>::type = nullptr>
        explicit IndeterminateProgressBar(Args &&... args)
            : settings_(details::get<details::ProgressBarOption::bar_width>(option::BarWidth{100}, std::forward<Args>(args)...),
                        details::get<details::ProgressBarOption::prefix_text>(option::PrefixText{}, std::forward<Args>(args)...),
                        details::get<details::ProgressBarOption::postfix_text>(option::PostfixText{}, std::forward<Args>(args)...),
                        details::get<details::ProgressBarOption::start>(option::Start{"["}, std::forward<Args>(args)...),
                        details::get<details::ProgressBarOption::end>(option::End{"]"}, std::forward<Args>(args)...),
                        details::get<details::ProgressBarOption::fill>(option::Fill{"."}, std::forward<Args>(args)...),
                        details::get<details::ProgressBarOption::lead>(option::Lead{"<==>"}, std::forward<Args>(args)...),
                        details::get<details::ProgressBarOption::max_postfix_text_len>(option::MaxPostfixTextLen{0}, std::forward<Args>(args)...),
                        details::get<details::ProgressBarOption::completed>(option::Completed{false}, std::forward<Args>(args)...),
                        details::get<details::ProgressBarOption::foreground_color>(option::ForegroundColor{Color::unspecified}, std::forward<Args>(args)...),
                        details::get<details::ProgressBarOption::font_styles>(option::FontStyles{std::vector<FontStyle>{}}, std::forward<Args>(args)...),
                        details::get<details::ProgressBarOption::stream>(option::Stream{std::cout}, std::forward<Args>(args)...))
        {
            // starts with [<==>...........]
            // progress_ = 0

            // ends with   [...........<==>]
            //             ^^^^^^^^^^^^^^^^^ bar_width
            //             ^^^^^^^^^^^^ (bar_width - len(lead))
            // progress_ = bar_width - len(lead)
            progress_ = 0;
            max_progress_ = get_value<details::ProgressBarOption::bar_width>() -
                            get_value<details::ProgressBarOption::lead>().size() +
                            get_value<details::ProgressBarOption::start>().size() +
                            get_value<details::ProgressBarOption::end>().size();
        }

        template <typename T, details::ProgressBarOption id>
        void set_option(details::Setting<T, id> &&setting);

        template <typename T, details::ProgressBarOption id>
        void set_option(const details::Setting<T, id> &setting);

        void set_option(const details::Setting<std::string, details::ProgressBarOption::postfix_text> &setting);

        void set_option(details::Setting<std::string, details::ProgressBarOption::postfix_text> &&setting);

        void tick();

        bool is_completed();

        void mark_as_completed();

    private:
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

        size_t progress_{0};
        size_t max_progress_;
        Settings settings_;
        std::chrono::nanoseconds elapsed_;
        std::mutex mutex_;

        template <typename Indicator, size_t count>
        friend class MultiProgress;
        template <typename Indicator>
        friend class DynamicProgress;
        std::atomic<bool> multi_progress_mode_{false};

        std::pair<std::string, size_t> get_prefix_text();

        std::pair<std::string, size_t> get_postfix_text();

    public:
        void print_progress(bool from_multi_progress = false);
    };
} // namespace indicators

#endif // INDICATORS_INDETERMINATE_PROGRESS_BAR_H