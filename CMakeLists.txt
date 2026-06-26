cmake_minimum_required(VERSION 3.16)
project(HymnBookApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

# ---- 交叉编译特殊处理 ----
if(CMAKE_CROSSCOMPILING)
    # Windows moc.exe 不能在 Linux 上运行，使用本机工具
    find_program(HOST_MOC NAMES moc PATHS /usr/lib/qt6/libexec NO_DEFAULT_PATH REQUIRED)
    find_program(HOST_RCC NAMES rcc PATHS /usr/lib/qt6/libexec NO_DEFAULT_PATH REQUIRED)
    message(STATUS "Cross-compiling: using host moc=${HOST_MOC} rcc=${HOST_RCC}")

    # 为每个含 Q_OBJECT 的头文件生成 moc_xxx.cpp
    set(MOC_HEADERS
        MainWindow.h DirectoryPage.h HymnViewerPage.h AboutPage.h SettingsPage.h
        HymnManager.h ZoomableGraphicsView.h SearchDialog.h EditDialog.h HymnDelegate.h
    )
    set(MOC_CPP)
    foreach(_hdr ${MOC_HEADERS})
        # 生成 moc_ClassName.cpp
        string(REGEX REPLACE "\\.h$" ".cpp" _moc_cpp_name "${_hdr}")
        set(_moc_cpp ${CMAKE_CURRENT_BINARY_DIR}/moc_${_moc_cpp_name})
        add_custom_command(
            OUTPUT ${_moc_cpp}
            COMMAND ${HOST_MOC} -o ${_moc_cpp} ${CMAKE_CURRENT_SOURCE_DIR}/src/${_hdr}
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/${_hdr}
            COMMENT "MOC: ${_hdr}"
        )
        list(APPEND MOC_CPP ${_moc_cpp})
        set_source_files_properties(${_moc_cpp} PROPERTIES GENERATED TRUE)
    endforeach()

    # 手动运行 rcc 编译 qrc 资源
    set(RCC_CPP ${CMAKE_CURRENT_BINARY_DIR}/qrc_resources.cpp)
    add_custom_command(
        OUTPUT ${RCC_CPP}
        COMMAND ${HOST_RCC} -o ${RCC_CPP} ${CMAKE_CURRENT_SOURCE_DIR}/resources/resources.qrc
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/resources/resources.qrc
        COMMENT "RCC: resources.qrc"
    )
    set_source_files_properties(${RCC_CPP} PROPERTIES GENERATED TRUE)

    set(GENERATED_MOC_SOURCES ${MOC_CPP})
    set(GENERATED_RCC_SOURCE ${RCC_CPP})
    set(QRC_SOURCE)
else()
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(GENERATED_MOC_SOURCES)
    set(GENERATED_RCC_SOURCE)
    set(QRC_SOURCE resources/resources.qrc)
endif()

if(WIN32)
    set(APP_ICON resources/app.rc)
endif()

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/MainWindow.cpp
    src/DirectoryPage.cpp
    src/HymnViewerPage.cpp
    src/AboutPage.cpp
    src/SettingsPage.cpp
    src/HymnManager.cpp
    src/ZoomableGraphicsView.cpp
    src/SearchDialog.cpp
    src/EditDialog.cpp
    src/HymnDelegate.cpp
    src/MainWindow.h
    src/DirectoryPage.h
    src/HymnViewerPage.h
    src/AboutPage.h
    src/SettingsPage.h
    src/Hymn.h
    src/HymnManager.h
    src/ZoomableGraphicsView.h
    src/SearchDialog.h
    src/EditDialog.h
    src/HymnDelegate.h
    ${QRC_SOURCE}
    ${APP_ICON}
    ${GENERATED_MOC_SOURCES}
    ${GENERATED_RCC_SOURCE}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
)

# Windows 专属：GUI 子系统（禁止控制台窗口）
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# Copy data directory to build output
file(COPY data DESTINATION ${CMAKE_BINARY_DIR})
