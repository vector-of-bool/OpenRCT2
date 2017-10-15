add_library(openrct2-deps-iface INTERFACE)
add_library(openrct2::deps ALIAS openrct2-deps-iface)

if(MSVC)
    set(deps_zip "${CMAKE_CURRENT_BINARY_DIR}/deps.zip")
    include(ExternalProject)
    set(deps_dir "${CMAKE_CURRENT_BINARY_DIR}/deps")
    ExternalProject_Add(
        OpenRCT2-Deps
        URL "https://github.com/OpenRCT2/Dependencies/releases/download/v13/openrct2-libs-vs2017.zip"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        SOURCE_DIR "${deps_dir}"
        USES_TERMINAL_DOWNLOAD 1
        )
    add_library(openrct2-deps-static STATIC IMPORTED)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(arch x64)
    else()
        set(arch x86)
    endif()
    set(includes
        "${deps_dir}/include"
        "${deps_dir}/include/SDL2"
        )
    set(lib_file "${deps_dir}/openrct2-libs-vs2017-${arch}.lib")
    add_custom_command(OUTPUT "${lib_file}" DEPENDS OpenRCT2-Deps)
    add_custom_target(get-deps DEPENDS "${lib_file}")
    set_target_properties(openrct2-deps-static PROPERTIES
        IMPORTED_LOCATION "${lib_file}"
        INTERFACE_INCLUDE_DIRECTORIES "${includes}"
        )
    add_dependencies(openrct2-deps-static get-deps)
    target_link_libraries(openrct2-deps-iface INTERFACE openrct2-deps-static)
else()
    if(ENABLE_OPENGL)
        find_package(OpenGL REQUIRED)
        find_package(EXPAT REQUIRED)
        target_link_libraries(openrct2-deps-iface INTERFACE OpenGL::GL EXPAT::EXPAT)
        target_compile_definitions(openrct2-flags-iface INTERFACE OPENGL_NO_LINK)
    endif()
    find_package(SDL2 REQUIRED)
    find_package(SpeexDSP REQUIRED)
    find_package(jansson REQUIRED)
    find_package(PNG REQUIRED)
    find_package(ZLIB REQUIRED)
    find_package(libzip REQUIRED)
    target_link_libraries(openrct2-deps-iface
        INTERFACE
            SDL2::SDL
            SpeexDSP::SpeexDSP
            jansson::jansson
            PNG::PNG
            ZLIB::ZLIB
            libzip::libzip
        )
    set(optional_libs)
    if(ENABLE_NETWORK OR ENABLE_HTTP_TWITCH)
        find_package(CURL REQUIRED)
        list(APPEND optional_libs curl::curl)
    endif()
    if(ENABLE_TTF)
        if(UNIX AND NOT APPLE)
            # Link with fontconfig too. It must come first in the link line!
            find_package(fontconfig REQUIRED)
            list(APPEND optional_libs fontconfig::fontconfig)
        endif()
        find_package(Freetype REQUIRED)
        list(APPEND optional_libs Freetype::Freetype)
    endif()
    target_link_libraries(openrct2-deps-iface INTERFACE ${optional_libs})
endif()

if(ENABLE_NETWORK OR ENABLE_HTTP_TWITCH)
    # Link in libraries required for networking
    if(NOT MSVC)
        if(APPLE)
            # We have an alternative installation of OpenSSL for Apple, since
            # CMake won't find it otherwise
            set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
        endif()
        find_package(OpenSSL REQUIRED)
        target_link_libraries(openrct2-flags-iface INTERFACE
            OpenSSL::SSL
            OpenSSL::Crypto
            )
    endif()
    if(WIN32)
        target_link_libraries(openrct2-deps-iface INTERFACE ws2_32)
    endif()
endif()

# Add all deps to the global flags interface
target_link_libraries(openrct2-flags-iface INTERFACE openrct2-deps-iface)