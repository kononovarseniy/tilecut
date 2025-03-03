function(target_enable_warnings TARGET_NAME)
    target_compile_options(${TARGET_NAME} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
    )
    # https://github.com/fmtlib/fmt/issues/3540
    target_compile_definitions(${TARGET_NAME} PRIVATE _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)
endfunction()
