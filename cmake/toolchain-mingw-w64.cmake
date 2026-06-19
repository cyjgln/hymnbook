# CMake 工具链：Linux → Windows x86_64 交叉编译
# 使用系统 MinGW-w64 + aqtinstall 下载的 Qt6 for Windows

# 目标系统
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# MinGW 编译器（系统安装的）
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# 主机 Qt（Linux 系统安装的 — 提供 moc/rcc 等 host 工具）
set(QT_HOST_PATH "/usr")
set(Qt6HostInfo_DIR "/usr/lib/x86_64-linux-gnu/cmake/Qt6HostInfo")

# 目标 Qt（aqtinstall 下载的 Qt6 for Windows）
set(QT6_INSTALL_PATH "/tmp/qt-windows/6.2.4/mingw_64")
set(CMAKE_PREFIX_PATH "${QT6_INSTALL_PATH}")

# 让 find_package 能找到 Qt6
list(APPEND CMAKE_FIND_ROOT_PATH "${QT6_INSTALL_PATH}")

# 搜索策略
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
