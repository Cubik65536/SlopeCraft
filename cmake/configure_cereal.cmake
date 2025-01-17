set(SKIP_PERFORMANCE_COMPARISON ON)
set(BUILD_SANDBOX OFF)
set(BUILD_DOC OFF)
set(CEREAL_INSTALL OFF)

find_package(cereal QUIET)

if (${cereal_FOUND})
    return()
endif ()

include(FetchContent)

FetchContent_Declare(cereal
    GIT_REPOSITORY https://github.com/USCiLab/cereal
    GIT_TAG v1.3.2
    OVERRIDE_FIND_PACKAGE)

message(STATUS "Configuring cereal ...")

FetchContent_MakeAvailable(cereal)