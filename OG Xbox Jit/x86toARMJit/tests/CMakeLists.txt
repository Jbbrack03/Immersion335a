include(GoogleTest)

# Test utilities library
add_library(test_utils INTERFACE)
target_include_directories(test_utils INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

# Unit tests executable
add_executable(unit_tests
    # Utils tests
    unit/utils/test_memory_interface.cpp
    
    # Decoder tests
    unit/decoder/test_basic_decoder.cpp
    unit/decoder/test_modrm_parsing.cpp
    unit/decoder/test_arithmetic_instructions.cpp
    unit/decoder/test_sib_addressing.cpp
    unit/decoder/test_prefix_handling.cpp
    unit/decoder/test_control_flow_instructions.cpp
    
    # IR tests
    # unit/ir/test_ir_generation.cpp
    # unit/ir/test_ir_optimization.cpp
    
    # Code generator tests
    # unit/codegen/test_arithmetic.cpp
    # unit/codegen/test_memory.cpp
    # unit/codegen/test_control_flow.cpp
    # unit/codegen/test_simd.cpp
    
    # Cache tests
    # unit/cache/test_translation_cache.cpp
    
    # Add main entry point for now
    test_main.cpp
)

target_link_libraries(unit_tests
    PRIVATE
    xenoarm_jit_lib
    test_utils
    GTest::gtest_main
    GTest::gmock
)

target_include_directories(unit_tests
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Integration tests executable
add_executable(integration_tests
    # integration/test_simple_programs.cpp
    # integration/test_self_modifying_code.cpp
    # integration/test_sse_translation.cpp
    # integration/test_flag_handling.cpp
    
    # Add main entry point for now
    test_main.cpp
)

target_link_libraries(integration_tests
    PRIVATE
    xenoarm_jit_lib
    test_utils
    GTest::gtest_main
)

# Discover tests for CTest
gtest_discover_tests(unit_tests)
gtest_discover_tests(integration_tests)

# Copy test fixtures
file(COPY fixtures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Custom test targets
add_custom_target(test_verbose
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose --output-on-failure
    DEPENDS unit_tests integration_tests
)

add_custom_target(test_unit
    COMMAND $<TARGET_FILE:unit_tests>
    DEPENDS unit_tests
)

add_custom_target(test_integration
    COMMAND $<TARGET_FILE:integration_tests>
    DEPENDS integration_tests
)

# Coverage target (if enabled)
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    find_program(GCOVR gcovr)
    if(GCOVR)
        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -E make_directory coverage
            COMMAND ${GCOVR} -r ${CMAKE_SOURCE_DIR} 
                    --filter ${CMAKE_SOURCE_DIR}/src
                    --html --html-details
                    --output coverage/index.html
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS unit_tests integration_tests
        )
    endif()
endif()