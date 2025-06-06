cmake_minimum_required(VERSION 4.0.0 FATAL_ERROR)
cmake_policy(SET CMP0167 NEW)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
  add_compile_definitions(DEBUG)
  if(UNIX)
    add_compile_options(-g)
  endif()
endif()
set(CMAKE_CXX_STANDARD 20)

project(MikuMikuWorld)

# Main
set(depends_files "./Depends/glad/src/glad.c"
                  "./Depends/stb_vorbis/stb_vorbis.c")
set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}/MikuMikuWorld")
file(GLOB_RECURSE ALL_FILES "${source_dir}/*.cpp" "${source_dir}/*.h"
     "${source_dir}/*.hpp" "${source_dir}/*.c")
add_executable(${PROJECT_NAME} ${ALL_FILES} ${depends_files})
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/./Depends/glm;"
         "${CMAKE_CURRENT_SOURCE_DIR}/./Depends/glad/include;"
         "${CMAKE_CURRENT_SOURCE_DIR}/./Depends/GLFW/include;"
         "${CMAKE_CURRENT_SOURCE_DIR}/./Depends/stb_image;"
         "${CMAKE_CURRENT_SOURCE_DIR}/./Depends/miniaudio;"
         "${CMAKE_CURRENT_SOURCE_DIR}/./Depends/json;"
         "${CMAKE_CURRENT_SOURCE_DIR}/./Depends/stb_vorbis;"
         "${CMAKE_CURRENT_SOURCE_DIR}/./Depends;"
         "${CMAKE_CURRENT_SOURCE_DIR}/./Depends/openssl/include")

target_link_directories(
  ${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../Depends/GLFW/lib;"
  "${CMAKE_CURRENT_SOURCE_DIR}/../Depends/openssl/lib")

target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_DEFINE_MATH_OPERATORS)

if(EMSCRIPTEN)

elseif(MSVC)
  target_link_libraries(${PROJECT_NAME} PRIVATE opengl32 Version libcrypto.lib
                                                libssl.lib)
elseif(UNIX)
  target_link_libraries(${PROJECT_NAME} PRIVATE GL X11 pthread dl libcrypto.so
                                                libssl.so)
else()
  message(FATAL_ERROR "Unsupported platform")
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "MikuMikuWorld4CC")

if(EMSCRIPTEN)
  target_compile_options(${PROJECT_NAME} PUBLIC "--use-port=contrib.glfw3")
  target_link_options(${PROJECT_NAME} PUBLIC "--use-port=contrib.glfw3")
else()
  # Depends: GLFW
  add_subdirectory(Depends/glfw)
  target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

  # Depends: NativeMessageBox
  add_subdirectory(Depends/native_message_box)
  target_link_libraries(${PROJECT_NAME} PRIVATE nfd)

  # Depends: NativeFileDialog extended
  add_subdirectory(Depends/nativefiledialog-extended)
  target_link_libraries(${PROJECT_NAME} PRIVATE nfd)

  # Depends: whereami
  add_library(whereami STATIC
              ${CMAKE_CURRENT_SOURCE_DIR}/Depends/whereami/src/whereami.c)
  target_include_directories(
    whereami PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Depends/whereami/src)
  target_include_directories(
    ${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Depends/whereami/src)

  # Depends: whereami-cpp
  add_library(
    whereami-cpp STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Depends/whereami-cpp/source/whereami++.cpp)
  target_include_directories(
    whereami-cpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Depends/whereami-cpp/include
                        ${CMAKE_CURRENT_SOURCE_DIR}/Depends/whereami/src)
  target_link_libraries(whereami-cpp PRIVATE whereami)
  target_include_directories(
    ${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Depends/whereami-cpp/include)
  target_link_libraries(${PROJECT_NAME} PRIVATE whereami-cpp)

endif()

# Depends: ImGui
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Depends/imgui")
file(GLOB IMGUI_FILES "${IMGUI_DIR}/*.cpp" "${IMGUI_DIR}/*.h"
     "${IMGUI_DIR}/*.hpp")
add_library(
  imgui STATIC
  ${IMGUI_FILES} ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
  ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp)
target_include_directories(imgui PUBLIC "${IMGUI_DIR}")
if(EMSCRIPTEN)
  target_compile_options(imgui PUBLIC "--use-port=contrib.glfw3")
else()
  target_link_libraries(imgui PRIVATE glfw)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC "${IMGUI_DIR}")
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

# Depends: Boost
add_subdirectory(Depends/boost)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::locale)

if(EMSCRIPTEN)
  target_link_options(boost_container PRIVATE -pthread)
  target_compile_options(boost_container PRIVATE -pthread)
  target_link_options(boost_thread PRIVATE -pthread)
  target_compile_options(boost_thread PRIVATE -pthread)
  target_link_options(boost_locale PRIVATE -pthread)
  target_compile_options(boost_locale PRIVATE -pthread)
endif()

# Copy files
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/MikuMikuWorld/res"
    "${CMAKE_CURRENT_BINARY_DIR}/res")
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E echo copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/MikuMikuWorld/res"
    "${CMAKE_CURRENT_BINARY_DIR}/res")
