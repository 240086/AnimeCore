cmake_minimum_required(VERSION 3.16)
project(AnimeCore)

# 解决你看到的 CMP0167 警告（新版本 CMake 推荐用法）
cmake_policy(SET CMP0167 NEW)

set(CMAKE_CXX_STANDARD 17)

# --- 1. 必须先寻找依赖，才能产生 :: 符号的目标 ---
find_package(Boost REQUIRED COMPONENTS system thread)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(yaml-cpp REQUIRED)

# --- 2. 收集源码 ---
file(GLOB_RECURSE CORE_SOURCES "src/*.cpp")

# --- 3. 定义库目标 ---
add_library(AnimeCore STATIC ${CORE_SOURCES})

# --- 4. 配置包含路径 (关键：使用 PUBLIC 让引用者自动获取 include) ---
target_include_directories(AnimeCore PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# --- 5. 链接依赖 ---
# PUBLIC: 意味着任何链接 AnimeCore 的项目（如 Gateway）也会自动链接这些库
# 这是解决你之前 undefined reference 的根本办法
target_link_libraries(AnimeCore
    PUBLIC
        Boost::boost
        Boost::system
        spdlog::spdlog
        fmt::fmt
        yaml-cpp::yaml-cpp
)

# Windows 下的额外支持
if(WIN32)
    target_link_libraries(AnimeCore PUBLIC ws2_32 mswsock)
endif()