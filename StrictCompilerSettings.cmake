cmake_minimum_required(VERSION 3.16)

include_guard()

function(enable_strict_compiler_settings target)
	set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
	if(CMAKE_CXX_COMPILER_ID MATCHES MSVC) # MSVC
		target_compile_options(${target} PRIVATE /W4 /WX /permissive-)
	elseif(CMAKE_CXX_COMPILER_ID MATCHES GNU) # GCC or MinGW
		target_compile_options(${target} PRIVATE -Wall -Wextra -Werror -pedantic -pedantic-errors -Wold-style-cast -Wno-error=unused-parameter -Wno-error=unused-local-typedefs)
	elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang AND CMAKE_CXX_SIMULATE_ID MATCHES MSVC) # Clang-CL
		target_compile_options(${target} PRIVATE /W4 -Wextra -Werror -Wold-style-cast )
	elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang) # Clang
		target_compile_options(${target} PRIVATE -Wall -Wextra -Werror -pedantic -pedantic-errors -Wold-style-cast -Wno-error=unused-private-field -Wno-error=unused-parameter -Wno-error=unused-local-typedef)
	else()
		message(WARNING "Unknown Compiler")
	endif()
endfunction()
