cmake_minimum_required(VERSION 3.31)
project(libstrophe_cpp_test)

set(CMAKE_CXX_STANDARD 23)

# Assume you installed libstrophe to ~/.local
set(LIBSTROPHE_PATH "$ENV{HOME}/.local")

# Include path for headers
include_directories("${LIBSTROPHE_PATH}/include")

# Library path for the linker
link_directories("${LIBSTROPHE_PATH}/lib")

# Add your executable
add_executable(libstrophe_cpp_test main.cpp
        libstrophe_cpp.h
        libstrophe_cpp.cpp
        xmpp_stanza.h
        xmpp_stanza.cpp
        handler_callback.h)

# Link libstrophe
target_link_libraries(libstrophe_cpp_test strophe)

# Optional: embed the runtime path so you don't need LD_LIBRARY_PATH
set_target_properties(libstrophe_cpp_test PROPERTIES
        INSTALL_RPATH "${LIBSTROPHE_PATH}/lib"
)

set(CMAKE_INSTALL_RPATH "$ENV{HOME}/.local/lib")
