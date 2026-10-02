cmake_minimum_required(VERSION 3.14)
project(quddus_salako_musiclinten CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Only RateLimiter is built for now; the other classes are still skeletons.
add_executable(rate_limiter_tests
  tests/RateLimiterTest.cpp
  RateLimiter.cpp
)
target_link_libraries(rate_limiter_tests GTest::gtest_main GTest::gmock)

include(GoogleTest)
gtest_discover_tests(rate_limiter_tests)
