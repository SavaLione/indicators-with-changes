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
#include "indicators/indeterminate_progress_bar.h"

namespace indicators
{
    void IndeterminateProgressBar::set_option(const details::Setting<std::string, details::ProgressBarOption::postfix_text> &setting)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        get_value<details::ProgressBarOption::postfix_text>() = setting.value;
        if (setting.value.length() > get_value<details::ProgressBarOption::max_postfix_text_len>())
        {
            get_value<details::ProgressBarOption::max_postfix_text_len>() = setting.value.length();
        }
    }

    void IndeterminateProgressBar::set_option(details::Setting<std::string, details::ProgressBarOption::postfix_text> &&setting)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        get_value<details::ProgressBarOption::postfix_text>() = std::move(setting).value;
        auto &new_value = get_value<details::ProgressBarOption::postfix_text>();
        if (new_value.length() > get_value<details::ProgressBarOption::max_postfix_text_len>())
        {
            get_value<details::ProgressBarOption::max_postfix_text_len>() = new_value.length();
        }
    }

    void IndeterminateProgressBar::tick()
    {
        {
            std::lock_guard<std::mutex> lock{mutex_};
            if (get_value<details::ProgressBarOption::completed>())
            {
                return;
            }

            progress_ += (direction_ == Direction::forward) ? 1 : -1;
            if (direction_ == Direction::forward && progress_ == max_progress_)
            {
                // time to go back
                direction_ = Direction::backward;
            }
            else if (direction_ == Direction::backward && progress_ == 0)
            {
                direction_ = Direction::forward;
            }
        }
        print_progress();
    }

    bool IndeterminateProgressBar::is_completed()
    {
        return get_value<details::ProgressBarOption::completed>();
    }

    void IndeterminateProgressBar::mark_as_completed()
    {
        get_value<details::ProgressBarOption::completed>() = true;
        print_progress();
    }

    std::pair<std::string, size_t> IndeterminateProgressBar::get_prefix_text()
    {
        std::stringstream os;
        os << get_value<details::ProgressBarOption::prefix_text>();
        const auto result = os.str();
        const auto result_size = unicode::display_width(result);
        return {result, result_size};
    }

    std::pair<std::string, size_t> IndeterminateProgressBar::get_postfix_text()
    {
        std::stringstream os;
        os << " " << get_value<details::ProgressBarOption::postfix_text>();

        const auto result = os.str();
        const auto result_size = unicode::display_width(result);
        return {result, result_size};
    }

    void IndeterminateProgressBar::print_progress(bool from_multi_progress = false)
    {
        std::lock_guard<std::mutex> lock{mutex_};

        auto &os = get_value<details::ProgressBarOption::stream>();

        if (multi_progress_mode_ && !from_multi_progress)
        {
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

        details::IndeterminateProgressScaleWriter writer{
            os, get_value<details::ProgressBarOption::bar_width>(),
            get_value<details::ProgressBarOption::fill>(),
            get_value<details::ProgressBarOption::lead>()};

        writer.write(progress_);

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

        if (get_value<details::ProgressBarOption::completed>() && !from_multi_progress) // Don't std::endl if calling from MultiProgress
        {
            os << termcolor::reset << std::endl;
        }
    }
} // namespace indicators