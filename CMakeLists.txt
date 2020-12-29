#################################################################################
# Copyright (c) 2020, Savely Pototsky (SavaLione)
# Based on: indicators, 2019 Pranav
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#################################################################################
# Checking the minimum version
cmake_minimum_required (VERSION 3.13.1)

set(INDICATORS_PROJECT "indicators")
set(INDICATORS_PROJECT_VERSION "1.9.0.1")

project(${INDICATORS_PROJECT}
    VERSION ${INDICATORS_PROJECT_VERSION}
    LANGUAGES CXX
)

# Threads
find_package(Threads REQUIRED)

set(INDICATORS_INCLUDES
    include/indicators/details/block_progress_scale_writer.h
    include/indicators/details/indeterminate_progress_scale_writer.h
    include/indicators/details/progress_scale_writer.h
    include/indicators/details/stream_helper.h
    include/indicators/block_progress_bar.h
    include/indicators/color.h
    include/indicators/cursor_control.h
    include/indicators/cursor_movement.h
    include/indicators/display_width.h
    include/indicators/dynamic_progress.h
    include/indicators/font_style.h
    include/indicators/indeterminate_progress_bar.h
    include/indicators/multi_progress.h
    include/indicators/progress_bar.h
    include/indicators/progress_spinner.h
    include/indicators/progress_type.h
    include/indicators/setting.h
    include/indicators/termcolor.h
    include/indicators/terminal_size.h
)

set(INDICATORS_SOURCES
    src/indicators/details/block_progress_scale_writer.cpp
    src/indicators/details/indeterminate_progress_scale_writer.cpp
    src/indicators/details/progress_scale_writer.cpp
    src/indicators/details/stream_helper.cpp
    src/indicators/block_progress_bar.cpp
    src/indicators/cursor_control.cpp
    src/indicators/cursor_movement.cpp
    src/indicators/display_width.cpp
    src/indicators/dynamic_progress.cpp
    src/indicators/indeterminate_progress_bar.cpp
    src/indicators/multi_progress.cpp
    src/indicators/progress_bar.cpp
    src/indicators/progress_spinner.cpp
    src/indicators/termcolor.cpp
    src/indicators/terminal_size.cpp
)

# static or dynamic
option(INDICATORS_STATIC_LIB "${INDICATORS_PROJECT}. Build static library" ON)

# examples
option(INDICATORS_EXAMPLES "${INDICATORS_PROJECT}. Build examples" ON)

if(INDICATORS_STATIC_LIB)
	add_library(${INDICATORS_PROJECT} STATIC ${INDICATORS_SOURCES} ${INDICATORS_INCLUDES})
else()
	add_library(${INDICATORS_PROJECT} SHARED ${INDICATORS_SOURCES} ${INDICATORS_INCLUDES})
endif()

# library
add_library(${INDICATORS_PROJECT}::${INDICATORS_PROJECT} ALIAS ${INDICATORS_PROJECT})

# library include
target_include_directories(${INDICATORS_PROJECT} PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${INDICATORS_PROJECT} PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Threads
if(Threads_FOUND)
    target_link_libraries(${INDICATORS_PROJECT} Threads::Threads)
endif()

if(INDICATORS_EXAMPLES)
    add_subdirectory(examples)
endif()