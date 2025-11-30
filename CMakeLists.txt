cmake_minimum_required(VERSION 3.12)
project(PeculiarCompiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 添加子目录（src）
add_subdirectory(src)

# --------------------------
# CPack 配置（适配 Windows Visual Studio 生成器）
# --------------------------
if (CMAKE_BUILD_TYPE STREQUAL "Release")
  include(CPack)
  set(CPACK_GENERATOR "ZIP;TGZ")

  # 固定产物输出目录
  set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/package")

  # 适配 Windows Visual Studio 生成器：强制指定打包目标和安装路径
  if (WIN32 AND MSVC)
    # 明确告诉 CPack 打包 Release 配置的目标（Visual Studio 默认多配置）
    set(CPACK_BUILD_CONFIGURATION "Release")
    # 确保安装路径正确（避免 Windows 下路径问题）
    set(CPACK_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/package/install")
  elseif (APPLE)
    set(CPACK_GENERATOR "DragNDrop")
  else ()
    set(CPACK_GENERATOR "TGZ;DEB")
  endif ()

  # 包信息配置
  set(CPACK_PACKAGE_NAME "PeculiarCompiler")
  set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}" CACHE STRING "项目版本号" FORCE)
  if ("${CPACK_PACKAGE_VERSION}" STREQUAL "")
    set(CPACK_PACKAGE_VERSION "1.0.0")  # 版本号默认值
  endif ()
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Peculiar Compiler - 一个自定义编译器项目")
  set(CPACK_PACKAGE_CONTACT "hugoz <你的邮箱@example.com>")
  set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_HOST_SYSTEM_PROCESSOR}")

  # 打包目标配置（适配 src 中的可执行目标）
  set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR}" "${PROJECT_NAME}" "Runtime" "/")
endif ()