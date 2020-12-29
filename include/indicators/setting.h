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
 * @brief Setting
 * @author Pranav, SavaLione
 * @date 28 Dec 2020
 */
#ifndef INDICATORS_SETTING_H
#define INDICATORS_SETTING_H

#include <cstddef>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <indicators/color.h>
#include <indicators/font_style.h>
#include <indicators/progress_type.h>

namespace indicators
{

    namespace details
    {

        template <bool condition>
        struct if_else;

        template <>
        struct if_else<true>
        {
            using type = std::true_type;
        };

        template <>
        struct if_else<false>
        {
            using type = std::false_type;
        };

        template <bool condition, typename True, typename False>
        struct if_else_type;

        template <typename True, typename False>
        struct if_else_type<true, True, False>
        {
            using type = True;
        };

        template <typename True, typename False>
        struct if_else_type<false, True, False>
        {
            using type = False;
        };

        template <typename... Ops>
        struct conjuction;

        template <>
        struct conjuction<> : std::true_type
        {
        };

        template <typename Op, typename... TailOps>
        struct conjuction<Op, TailOps...>
            : if_else_type<!Op::value, std::false_type, conjuction<TailOps...>>::type
        {
        };

        template <typename... Ops>
        struct disjunction;

        template <>
        struct disjunction<> : std::false_type
        {
        };

        template <typename Op, typename... TailOps>
        struct disjunction<Op, TailOps...>
            : if_else_type<Op::value, std::true_type, disjunction<TailOps...>>::type
        {
        };

        enum class ProgressBarOption
        {
            bar_width = 0,
            prefix_text,
            postfix_text,
            start,
            end,
            fill,
            lead,
            remainder,
            max_postfix_text_len,
            completed,
            show_percentage,
            show_elapsed_time,
            show_remaining_time,
            saved_start_time,
            foreground_color,
            spinner_show,
            spinner_states,
            font_styles,
            hide_bar_when_complete,
            min_progress,
            max_progress,
            progress_type,
            stream
        };

        template <typename T, ProgressBarOption Id>
        struct Setting
        {
            template <typename... Args, typename = typename std::enable_if<std::is_constructible<T, Args...>::value>::type>
            explicit Setting(Args &&... args) : value(std::forward<Args>(args)...) {}
            Setting(const Setting &) = default;
            Setting(Setting &&) = default;

            static constexpr auto id = Id;
            using type = T;

            T value{};
        };

        template <typename T>
        struct is_setting : std::false_type
        {
        };

        template <ProgressBarOption Id, typename T>
        struct is_setting<Setting<T, Id>> : std::true_type
        {
        };

        template <typename... Args>
        struct are_settings : if_else<conjuction<is_setting<Args>...>::value>::type
        {
        };

        template <>
        struct are_settings<> : std::true_type
        {
        };

        template <typename Setting, typename Tuple>
        struct is_setting_from_tuple;

        template <typename Setting>
        struct is_setting_from_tuple<Setting, std::tuple<>> : std::true_type
        {
        };

        template <typename Setting, typename... TupleTypes>
        struct is_setting_from_tuple<Setting, std::tuple<TupleTypes...>>
            : if_else<disjunction<std::is_same<Setting, TupleTypes>...>::value>::type
        {
        };

        template <typename Tuple, typename... Settings>
        struct are_settings_from_tuple
            : if_else<conjuction<is_setting_from_tuple<Settings, Tuple>...>::value>::type
        {
        };

        template <ProgressBarOption Id>
        struct always_true
        {
            static constexpr auto value = true;
        };

        template <ProgressBarOption Id, typename Default>
        Default &&get_impl(Default &&def)
        {
            return std::forward<Default>(def);
        }

        template <ProgressBarOption Id, typename Default, typename T, typename... Args>
        auto get_impl(Default && /*def*/, T &&first, Args &&... /*tail*/) -> typename std::enable_if<(std::decay<T>::type::id == Id), decltype(std::forward<T>(first))>::type
        {
            return std::forward<T>(first);
        }

        template <ProgressBarOption Id, typename Default, typename T, typename... Args>
        auto get_impl(Default &&def, T && /*first*/, Args &&... tail) -> typename std::enable_if<(std::decay<T>::type::id != Id), decltype(get_impl<Id>(std::forward<Default>(def), std::forward<Args>(tail)...))>::type
        {
            return get_impl<Id>(std::forward<Default>(def), std::forward<Args>(tail)...);
        }

        template <ProgressBarOption Id, typename Default, typename... Args, typename = typename std::enable_if<are_settings<Args...>::value, void>::type>
        auto get(Default &&def, Args &&... args) -> decltype(details::get_impl<Id>(std::forward<Default>(def), std::forward<Args>(args)...))
        {
            return details::get_impl<Id>(std::forward<Default>(def), std::forward<Args>(args)...);
        }

        template <ProgressBarOption Id>
        using StringSetting = Setting<std::string, Id>;

        template <ProgressBarOption Id>
        using IntegerSetting = Setting<std::size_t, Id>;

        template <ProgressBarOption Id>
        using BooleanSetting = Setting<bool, Id>;

        template <ProgressBarOption Id, typename Tuple, std::size_t counter = 0>
        struct option_idx;

        template <ProgressBarOption Id, typename T, typename... Settings, std::size_t counter>
        struct option_idx<Id, std::tuple<T, Settings...>, counter>
            : if_else_type<(Id == T::id),
                           std::integral_constant<std::size_t, counter>,
                           option_idx<Id, std::tuple<Settings...>, counter + 1>>::type
        {
        };

        template <ProgressBarOption Id, std::size_t counter>
        struct option_idx<Id, std::tuple<>, counter>
        {
            static_assert(always_true<(ProgressBarOption)Id>::value, "No such option was found");
        };

        template <ProgressBarOption Id, typename Settings>
        auto get_value(Settings &&settings) -> decltype((std::get<option_idx<Id, typename std::decay<Settings>::type>::value>(std::declval<Settings &&>())))
        {
            return std::get<option_idx<Id, typename std::decay<Settings>::type>::value>(std::forward<Settings>(settings));
        }

    } // namespace details

    namespace option
    {
        using BarWidth = details::IntegerSetting<details::ProgressBarOption::bar_width>;
        using PrefixText = details::StringSetting<details::ProgressBarOption::prefix_text>;
        using PostfixText = details::StringSetting<details::ProgressBarOption::postfix_text>;
        using Start = details::StringSetting<details::ProgressBarOption::start>;
        using End = details::StringSetting<details::ProgressBarOption::end>;
        using Fill = details::StringSetting<details::ProgressBarOption::fill>;
        using Lead = details::StringSetting<details::ProgressBarOption::lead>;
        using Remainder = details::StringSetting<details::ProgressBarOption::remainder>;
        using MaxPostfixTextLen = details::IntegerSetting<details::ProgressBarOption::max_postfix_text_len>;
        using Completed = details::BooleanSetting<details::ProgressBarOption::completed>;
        using ShowPercentage = details::BooleanSetting<details::ProgressBarOption::show_percentage>;
        using ShowElapsedTime = details::BooleanSetting<details::ProgressBarOption::show_elapsed_time>;
        using ShowRemainingTime = details::BooleanSetting<details::ProgressBarOption::show_remaining_time>;
        using SavedStartTime = details::BooleanSetting<details::ProgressBarOption::saved_start_time>;
        using ForegroundColor = details::Setting<Color, details::ProgressBarOption::foreground_color>;
        using ShowSpinner = details::BooleanSetting<details::ProgressBarOption::spinner_show>;
        using SpinnerStates = details::Setting<std::vector<std::string>, details::ProgressBarOption::spinner_states>;
        using HideBarWhenComplete = details::BooleanSetting<details::ProgressBarOption::hide_bar_when_complete>;
        using FontStyles = details::Setting<std::vector<FontStyle>, details::ProgressBarOption::font_styles>;
        using MinProgress = details::IntegerSetting<details::ProgressBarOption::min_progress>;
        using MaxProgress = details::IntegerSetting<details::ProgressBarOption::max_progress>;
        using ProgressType = details::Setting<ProgressType, details::ProgressBarOption::progress_type>;
        using Stream = details::Setting<std::ostream &, details::ProgressBarOption::stream>;

    } // namespace option
} // namespace indicators

#endif // INDICATORS_SETTING_H