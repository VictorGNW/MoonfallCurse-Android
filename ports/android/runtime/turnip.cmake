option(TRIAEVUM_ANDROID_TURNIP "Support imported Mesa Turnip drivers on Android ARM64" ON)
function(triaevum_enable_turnip runtime_target)
    if(NOT TRIAEVUM_ANDROID_TURNIP)
        return()
    endif()
    include(FetchContent)
    block()
        set(BUILD_SHARED_LIBS OFF)
        FetchContent_Declare(adrenotools
            GIT_REPOSITORY https://github.com/bylaws/libadrenotools.git
            GIT_TAG 8fae8ce254dfc1344527e05301e43f37dea2df80
            EXCLUDE_FROM_ALL)
        FetchContent_MakeAvailable(adrenotools)
    endblock()

    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    find_library(vulkan_stub NAMES vulkan REQUIRED)
    set(vulkan_headers "${CMAKE_SYSROOT}/usr/include/vulkan")
    set(generator "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/generate_vulkan_dispatch.py")
    set(dispatch "${CMAKE_CURRENT_BINARY_DIR}/android_vulkan_dispatch.inc")
    add_custom_command(OUTPUT "${dispatch}"
        COMMAND "${Python3_EXECUTABLE}" "${generator}" "${CMAKE_NM}" "${vulkan_stub}" "${vulkan_headers}" "${dispatch}"
        DEPENDS "${generator}" "${vulkan_stub}" "${vulkan_headers}/vulkan_core.h" "${vulkan_headers}/vulkan_android.h"
        VERBATIM)

    add_library(triaevum_vulkan SHARED "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/android_vulkan_loader.cpp" "${dispatch}")
    target_compile_features(triaevum_vulkan PRIVATE cxx_std_17)
    target_include_directories(triaevum_vulkan PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
    target_link_libraries(triaevum_vulkan PRIVATE adrenotools dl log)
    add_dependencies(triaevum_vulkan main_hook)
    set_target_properties(triaevum_vulkan main_hook hook_impl PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/turnip-libs")

    foreach(turnip_library IN ITEMS triaevum_vulkan main_hook hook_impl)
        target_link_options(${turnip_library} PRIVATE "LINKER:-z,max-page-size=16384")
    endforeach()

    # The final runtime links the Vulkan users from the shared renderer and Forge.
    # Wrap their direct calls. SDL and NRI load the same proxy by its absolute path.
    target_link_libraries(${runtime_target} PRIVATE triaevum_vulkan)
    execute_process(COMMAND "${CMAKE_NM}" -D --defined-only --format=posix "${vulkan_stub}"
        OUTPUT_VARIABLE vulkan_exports COMMAND_ERROR_IS_FATAL ANY)
    string(REGEX MATCHALL "vk[A-Za-z0-9_]+ " vulkan_symbols "${vulkan_exports}")
    if(NOT vulkan_symbols)
        message(FATAL_ERROR "No Vulkan exports found in ${vulkan_stub}")
    endif()
    foreach(symbol IN LISTS vulkan_symbols)
        string(STRIP "${symbol}" symbol)
        target_link_options(${runtime_target} PRIVATE "LINKER:--wrap=${symbol}")
    endforeach()
endfunction()
