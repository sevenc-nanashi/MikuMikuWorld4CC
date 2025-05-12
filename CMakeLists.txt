cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(MikuMikuWorld)
set(depends_files "./Depends/glad/src/glad.c"
                  "./Depends/stb_vorbis/stb_vorbis.c")
set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}/MikuMikuWorld")
file(GLOB_RECURSE ALL_FILES "${source_dir}/*.cpp" "${source_dir}/*.h"
     "${source_dir}/*.hpp" "${source_dir}/*.c")

# ##############################################################################
# Target
# ##############################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES} ${depends_files})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/./Depends/DirectXMath-master/Inc;"
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
set(ADDITIONAL_LIBRARY_DEPENDENCIES "glfw3;" "opengl32;" "Version;"
                                    "libcrypto;" "libssl")
target_link_libraries(${PROJECT_NAME}
                      PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")
