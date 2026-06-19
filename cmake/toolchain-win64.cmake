# CMake toolchain file for cross-compiling HymnBookApp to Windows (x86_64)
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-win64.cmake ..

# Target system
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# MinGW cross-compiler (system-installed)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# Qt for Windows installation path (from aqtinstall)
set(QT_WIN_PATH /home/claw/Qt/6.2.4/mingw_64)

# Find root - restricts CMake to search for dependencies only under these paths
list(APPEND CMAKE_FIND_ROOT_PATH ${QT_WIN_PATH})

# Tell CMake to also search in the sysroot for programs (but NOT for libs/includes)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Set Qt6_DIR so CMake finds Qt6 config files
set(Qt6_DIR ${QT_WIN_PATH}/lib/cmake/Qt6)

# Host Qt tools (moc, rcc) - needed during cross-compilation
set(HOST_QT_TOOLS /usr/lib/qt6/libexec)

# Prepend Qt for Windows bin to PATH so windeployqt etc. are found
set(ENV{PATH} "${QT_WIN_PATH}/bin:$ENV{PATH}")

# Suppress Qt warnings about cross-compilation
set(CMAKE_AUTOMOC OFF)
