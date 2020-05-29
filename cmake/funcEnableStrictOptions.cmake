# SDL2 C++ Wrapper Copyright (c) 2020 Malte Kießling
#
# This software is provided 'as-is', without any express or implied warranty. In
# no event will the authors be held liable for any damages arising from the use
# of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it freely,
# subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not claim
#    that you wrote the original software. If you use this software in a product,
#    an acknowledgment in the product documentation would be appreciated but is
#    not required.
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.

function(enableStrictOptions target)
    target_compile_options(
        ${target}
        PRIVATE
            $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
            -Wall
            -Werror
            -pedantic-errors
            -pedantic
            -Wextra
            -Wconversion
            -Wold-style-cast
            -Wuninitialized
            -Wunreachable-code
            -Wshadow
            -Wfloat-equal
            -Weffc++
            -Wsign-conversion>
            $<$<CXX_COMPILER_ID:MSVC>:
            /W4
            /WX>)
endfunction()
