# 1. 必须声明最低版本要求
cmake_minimum_required(VERSION 3.16)

# 2. 必须声明项目名称
project(AnimeCore LANGUAGES CXX)

# --- 以下是你之前的库配置逻辑 ---

# 3. 查找依赖（作为独立项目配置时需要）
find_package(Boost REQUIRED COMPONENTS system)

# 如果有 yaml-cpp 也可以在这里 find

# 4. 定义静态库
add_library(AnimeCore STATIC)

# 5. 扫描源码并添加到目标
file(GLOB_RECURSE CORE_SRC "src/*.cpp")
target_sources(AnimeCore PRIVATE ${CORE_SRC})

# 6. 设置包含路径
target_include_directories(AnimeCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 7. 链接依赖
target_link_libraries(AnimeCore PUBLIC
    Boost::system
)