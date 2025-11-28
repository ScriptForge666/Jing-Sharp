add_executable(peculiarc peculiarc.cpp)

if (MSVC)
    target_compile_options(peculiarc PRIVATE /bigobj)
endif()
