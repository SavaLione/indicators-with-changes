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
 * @brief Block progress bar
 * @author Pranav, SavaLione
 * @date 28 Dec 2020
 */
#include "indicators/block_progress_bar.h"

namespace indicators
{
    template <typename T, details::ProgressBarOption id>
    void BlockProgressBar::set_option(details::Setting<T, id> &&setting)
    {
        static_assert(!std::is_same<T, typename std::decay<decltype(details::get_value<id>(std::declval<Settings>()))>::type>::value, "Setting has wrong type!");
        std::lock_guard<std::mutex> lock(mutex_);
        get_value<id>() = std::move(setting).value;
    }

    void BlockProgressBar::set_option(const details::Setting<std::string, details::ProgressBarOption::postfix_text> &setting)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        get_value<details::ProgressBarOption::postfix_text>() = setting.value;
        if (setting.value.length() > get_value<details::ProgressBarOption::max_postfix_text_len>())
        {
            get_value<details::ProgressBarOption::max_postfix_text_len>() = setting.value.length();
        }
    }

    void BlockProgressBar::set_option(details::Setting<std::string, details::ProgressBarOption::postfix_text> &&setting)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        get_value<details::ProgressBarOption::postfix_text>() = std::move(setting).value;
        auto &new_value = get_value<details::ProgressBarOption::postfix_text>();
        if (new_value.length() > get_value<details::ProgressBarOption::max_postfix_text_len>())
        {
            get_value<details::ProgressBarOption::max_postfix_text_len>() = new_value.length();
        }
    }

    void BlockProgressBar::set_progress(float value)
    {
        {
            std::lock_guard<std::mutex> lock{mutex_};
            progress_ = value;
        }
        save_start_time();
        print_progress();
    }

    void BlockProgressBar::tick()
    {
        {
            std::lock_guard<std::mutex> lock{mutex_};
            progress_ += 1;
        }
        save_start_time();
        print_progress();
    }

    size_t BlockProgressBar::current()
    {
        std::lock_guard<std::mutex> lock{mutex_};
        return std::min(static_cast<size_t>(progress_), size_t(get_value<details::ProgressBarOption::max_progress>()));
    }

    void BlockProgressBar::mark_as_completed()
    {
        get_value<details::ProgressBarOption::completed>() = true;
        print_progress();
    }

    void BlockProgressBar::save_start_time()
    {
        auto &show_elapsed_time = get_value<details::ProgressBarOption::show_elapsed_time>();
        auto &saved_start_time = get_value<details::ProgressBarOption::saved_start_time>();
        auto &show_remaining_time = get_value<details::ProgressBarOption::show_remaining_time>();
        if ((show_elapsed_time || show_remaining_time) && !saved_start_time)
        {
            start_time_point_ = std::chrono::high_resolution_clock::now();
            saved_start_time = true;
        }
    }

    std::pair<std::string, size_t> BlockProgressBar::get_prefix_text()
    {
        std::stringstream os;
        os << get_value<details::ProgressBarOption::prefix_text>();
        const auto result = os.str();
        const auto result_size = unicode::display_width(result);
        return {result, result_size};
    }

    std::pair<std::string, size_t> BlockProgressBar::get_postfix_text()
    {
        std::stringstream os;
        const auto max_progress = get_value<details::ProgressBarOption::max_progress>();
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_time_point_);

        if (get_value<details::ProgressBarOption::show_percentage>())
        {
            os << " " << std::min(static_cast<size_t>(progress_ / max_progress * 100.0), size_t(100)) << "%";
        }

        auto &saved_start_time = get_value<details::ProgressBarOption::saved_start_time>();

        if (get_value<details::ProgressBarOption::show_elapsed_time>())
        {
            os << " [";
            if (saved_start_time)
            {
                details::write_duration(os, elapsed);
            }
            else
            {
                os << "00:00s";
            }
        }

        if (get_value<details::ProgressBarOption::show_remaining_time>())
        {
            if (get_value<details::ProgressBarOption::show_elapsed_time>())
            {
                os << "<";
            }
            else
            {
                os << " [";
            }

            if (saved_start_time)
            {
                auto eta = std::chrono::nanoseconds(
                    progress_ > 0
                        ? static_cast<long long>(std::ceil(float(elapsed.count()) *
                                                           max_progress / progress_))
                        : 0);
                auto remaining = eta > elapsed ? (eta - elapsed) : (elapsed - eta);
                details::write_duration(os, remaining);
            }
            else
            {
                os << "00:00s";
            }

            os << "]";
        }
        else
        {
            if (get_value<details::ProgressBarOption::show_elapsed_time>())
            {
                os << "]";
            }
        }

        os << " " << get_value<details::ProgressBarOption::postfix_text>();

        const auto result = os.str();
        const auto result_size = unicode::display_width(result);
        return {result, result_size};
    }

    void BlockProgressBar::print_progress(bool from_multi_progress)
    {
        std::lock_guard<std::mutex> lock{mutex_};

        auto &os = get_value<details::ProgressBarOption::stream>();

        const auto max_progress = get_value<details::ProgressBarOption::max_progress>();
        if (multi_progress_mode_ && !from_multi_progress)
        {
            if (progress_ > max_progress)
            {
                get_value<details::ProgressBarOption::completed>() = true;
            }
            return;
        }

        if (get_value<details::ProgressBarOption::foreground_color>() != Color::unspecified)
        {
            details::set_stream_color(os, get_value<details::ProgressBarOption::foreground_color>());
        }

        for (auto &style : get_value<details::ProgressBarOption::font_styles>())
        {
            details::set_font_style(os, style);
        }

        const auto prefix_pair = get_prefix_text();
        const auto prefix_text = prefix_pair.first;
        const auto prefix_length = prefix_pair.second;
        os << prefix_text;

        os << get_value<details::ProgressBarOption::start>();

        details::BlockProgressScaleWriter writer{os, get_value<details::ProgressBarOption::bar_width>()};
        writer.write(progress_ / max_progress * 100);

        os << get_value<details::ProgressBarOption::end>();

        const auto postfix_pair = get_postfix_text();
        const auto postfix_text = postfix_pair.first;
        const auto postfix_length = postfix_pair.second;
        os << postfix_text;

        // Get length of prefix text and postfix text
        const auto start_length = get_value<details::ProgressBarOption::start>().size();
        const auto bar_width = get_value<details::ProgressBarOption::bar_width>();
        const auto end_length = get_value<details::ProgressBarOption::end>().size();
        const auto terminal_width = terminal_size().second;
        // prefix + bar_width + postfix should be <= terminal_width
        const int remaining = terminal_width - (prefix_length + start_length + bar_width + end_length + postfix_length);
        if (remaining > 0)
        {
            os << std::string(remaining, ' ') << "\r";
        }
        else if (remaining < 0)
        {
            // Do nothing. Maybe in the future truncate postfix with ...
        }
        os.flush();

        if (progress_ > max_progress)
        {
            get_value<details::ProgressBarOption::completed>() = true;
        }
        if (get_value<details::ProgressBarOption::completed>() && !from_multi_progress) // Don't std::endl if calling from MultiProgress
        {
            os << termcolor::reset << std::endl;
        }
    }
} // namespace indicators