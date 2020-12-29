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
#include "indicators/progress_spinner.h"

namespace indicators
{
    void ProgressSpinner::set_option(const details::Setting<std::string, details::ProgressBarOption::postfix_text> &setting)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        get_value<details::ProgressBarOption::postfix_text>() = setting.value;
        if (setting.value.length() > get_value<details::ProgressBarOption::max_postfix_text_len>())
        {
            get_value<details::ProgressBarOption::max_postfix_text_len>() = setting.value.length();
        }
    }

    void ProgressSpinner::set_option(details::Setting<std::string, details::ProgressBarOption::postfix_text> &&setting)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        get_value<details::ProgressBarOption::postfix_text>() = std::move(setting).value;
        auto &new_value = get_value<details::ProgressBarOption::postfix_text>();
        if (new_value.length() > get_value<details::ProgressBarOption::max_postfix_text_len>())
        {
            get_value<details::ProgressBarOption::max_postfix_text_len>() = new_value.length();
        }
    }

    void ProgressSpinner::set_progress(size_t value)
    {
        {
            std::lock_guard<std::mutex> lock{mutex_};
            progress_ = value;
        }
        save_start_time();
        print_progress();
    }

    void ProgressSpinner::tick()
    {
        {
            std::lock_guard<std::mutex> lock{mutex_};
            progress_ += 1;
        }
        save_start_time();
        print_progress();
    }

    size_t ProgressSpinner::current()
    {
        std::lock_guard<std::mutex> lock{mutex_};
        return std::min(progress_, size_t(get_value<details::ProgressBarOption::max_progress>()));
    }

    void ProgressSpinner::mark_as_completed()
    {
        get_value<details::ProgressBarOption::completed>() = true;
        print_progress();
    }

    void ProgressSpinner::save_start_time()
    {
        auto &show_elapsed_time = get_value<details::ProgressBarOption::show_elapsed_time>();
        auto &show_remaining_time = get_value<details::ProgressBarOption::show_remaining_time>();
        auto &saved_start_time = get_value<details::ProgressBarOption::saved_start_time>();
        if ((show_elapsed_time || show_remaining_time) && !saved_start_time)
        {
            start_time_point_ = std::chrono::high_resolution_clock::now();
            saved_start_time = true;
        }
    }

    void ProgressSpinner::print_progress()
    {
        std::lock_guard<std::mutex> lock{mutex_};

        auto &os = get_value<details::ProgressBarOption::stream>();

        const auto max_progress = get_value<details::ProgressBarOption::max_progress>();
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_time_point_);

        if (get_value<details::ProgressBarOption::foreground_color>() != Color::unspecified)
        {
            details::set_stream_color(os, get_value<details::ProgressBarOption::foreground_color>());
        }

        for (auto &style : get_value<details::ProgressBarOption::font_styles>())
        {
            details::set_font_style(os, style);
        }

        os << get_value<details::ProgressBarOption::prefix_text>();
        if (get_value<details::ProgressBarOption::spinner_show>())
        {
            os << get_value<details::ProgressBarOption::spinner_states>()[index_ % get_value<details::ProgressBarOption::spinner_states>().size()];
        }

        if (get_value<details::ProgressBarOption::show_percentage>())
        {
            os << " " << std::size_t(progress_ / double(max_progress) * 100) << "%";
        }

        if (get_value<details::ProgressBarOption::show_elapsed_time>())
        {
            os << " [";
            details::write_duration(os, elapsed);
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
            auto eta = std::chrono::nanoseconds(
                progress_ > 0
                    ? static_cast<long long>(std::ceil(float(elapsed.count()) *
                                                       max_progress / progress_))
                    : 0);
            auto remaining = eta > elapsed ? (eta - elapsed) : (elapsed - eta);
            details::write_duration(os, remaining);
            os << "]";
        }
        else
        {
            if (get_value<details::ProgressBarOption::show_elapsed_time>())
            {
                os << "]";
            }
        }

        if (get_value<details::ProgressBarOption::max_postfix_text_len>() == 0)
        {
            get_value<details::ProgressBarOption::max_postfix_text_len>() = 10;
        }

        os << " " << get_value<details::ProgressBarOption::postfix_text>() << std::string(get_value<details::ProgressBarOption::max_postfix_text_len>(), ' ') << "\r";
        os.flush();
        index_ += 1;
        if (progress_ > max_progress)
        {
            get_value<details::ProgressBarOption::completed>() = true;
        }

        if (get_value<details::ProgressBarOption::completed>())
        {
            os << termcolor::reset << std::endl;
        }
    }
} // namespace indicators