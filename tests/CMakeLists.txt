include(GoogleTest)

function(add_algorithm_test algorithm)
    set(target "${algorithm}_test")
    add_executable(${target} "${algorithm}_test.cpp")
    target_link_libraries(${target} PRIVATE GTest::gtest_main)

    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()

    gtest_discover_tests(${target})
endfunction()

add_algorithm_test(aho_corasick)
add_algorithm_test(dijkstra)
add_algorithm_test(dsu)
add_algorithm_test(kahn)
add_algorithm_test(kmp)
add_algorithm_test(lis)
add_algorithm_test(primes)
add_algorithm_test(trie)
