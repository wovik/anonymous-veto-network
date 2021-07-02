cmake_minimum_required(VERSION 3.0.0)
project(veto VERSION 0.1.0)

include(CTest)
enable_testing()

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

add_executable(veto main.cpp)

target_link_libraries(veto PRIVATE OpenSSL::applink OpenSSL::SSL OpenSSL::Crypto)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
