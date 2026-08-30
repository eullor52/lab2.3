set(CMAKE_PREFIX_PATH "C:/googletest/install")
find_package(GTest REQUIRED)

add_executable(lab_tests
    main.cpp
    test_complex.cpp
    test_fragment.cpp
    test_piecewisefunction.cpp
    test_spline.cpp
)

target_link_libraries(lab_tests PRIVATE lab_core GTest::gtest)

include(GoogleTest)
gtest_discover_tests(lab_tests)