cmake_minimum_required(VERSION 3.12)
project(PeculiarCompiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 添加子目录（src）- 假设 src 目录下有你实际的 CMakeLists.txt，会声明真正的目标（如 PeculiarCompiler 可执行文件）
add_subdirectory(src)

# --------------------------
# CPack 配置（产物输出到 build/package/）
# --------------------------
if (CMAKE_BUILD_TYPE STREQUAL "Release")
  # 启用 CPack（仅当 src 中声明了可执行目标/库目标时生效）
  include(CPack)

  # 关键：指定 CPack 产物输出到 build/package/ 目录
  set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/package")

  # 按系统配置 CPack 生成器
  if (WIN32)
    set(CPACK_GENERATOR "ZIP")  # Windows 生成 ZIP 包
  elseif (APPLE)
    set(CPACK_GENERATOR "DragNDrop")  # macOS 生成 DMG 包
  else ()
    set(CPACK_GENERATOR "TGZ;DEB")  # Linux 生成 tar.gz + DEB 包
  endif ()

  # 包信息配置（适配你的项目）
  set(CPACK_PACKAGE_NAME "PeculiarCompiler")  # 包名与项目名一致
  set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")  # 版本号（若未指定，可手动写死如 "1.0.0"）
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Peculiar Compiler - 一个自定义编译器项目")
  set(CPACK_PACKAGE_CONTACT "hugoz <你的邮箱@example.com>")  # 替换为你的信息
  set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_HOST_SYSTEM_PROCESSOR}")

  # 关键：告诉 CPack 要打包的目标（替换为你 src 目录中实际的可执行目标名！）
  # 例如：如果 src 中用 add_executable(PeculiarCompiler main.cpp)，则这里写下面这行
  set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR}" "${PROJECT_NAME}" "Runtime" "/")
endif ()