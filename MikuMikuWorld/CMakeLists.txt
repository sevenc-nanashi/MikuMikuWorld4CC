set(PROJECT_NAME MikuMikuWorld)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "../Depends/glad/src/glad.c"
    "Application.cpp"
    "Application.h"
    "ApplicationConfiguration.cpp"
    "ApplicationConfiguration.h"
    "Constants.h"
    "main.cpp"
    "OpenGlLoader.cpp"
)
source_group("" FILES ${no_group_source_files})

set(Audio
    "Audio/AudioManager.cpp"
    "Audio/AudioManager.h"
    "Audio/Sound.cpp"
    "Audio/Sound.h"
    "Audio/Waveform.h"
)
source_group("Audio" FILES ${Audio})

set(Audio__Lib
    "../Depends/stb_vorbis/stb_vorbis.c"
	"../Depends/miniaudio/miniaudio.h"
)
source_group("Audio\\Lib" FILES ${Audio__Lib})

set(Graphics
    "Background.cpp"
    "Background.h"
    "Jacket.cpp"
    "Jacket.h"
)
source_group("Graphics" FILES ${Graphics})

set(IO
    "InputBinding.cpp"
    "InputBinding.h"
    "IO.cpp"
    "IO.h"
    "jsonIO.cpp"
    "JsonIO.h"
    "ResourceManager.cpp"
    "ResourceManager.h"
)
source_group("IO" FILES ${IO})

set(IO__File
    "BinaryReader.cpp"
    "BinaryReader.h"
    "BinaryWriter.cpp"
    "BinaryWriter.h"
    "File.cpp"
    "File.h"
)
source_group("IO\\File" FILES ${IO__File})

set(Imgui
    "ImGui/imconfig.h"
    "ImGui/imgui.cpp"
    "ImGui/imgui.h"
    "ImGui/imgui_demo.cpp"
    "ImGui/imgui_draw.cpp"
    "ImGui/imgui_impl_glfw.cpp"
    "ImGui/imgui_impl_glfw.h"
    "ImGui/imgui_impl_opengl3.cpp"
    "ImGui/imgui_impl_opengl3.h"
    "ImGui/imgui_internal.h"
    "ImGui/imgui_stdlib.cpp"
    "ImGui/imgui_stdlib.h"
    "ImGui/imgui_tables.cpp"
    "ImGui/imgui_widgets.cpp"
    "ImGui/imstb_rectpack.h"
    "ImGui/imstb_textedit.h"
    "ImGui/imstb_truetype.h"
)
source_group("Imgui" FILES ${Imgui})

set(Misc
    "Math.cpp"
    "Math.h"
    "Stopwatch.cpp"
    "Stopwatch.h"
    "Utilities.cpp"
    "Utilities.h"
)
source_group("Misc" FILES ${Misc})

set(Presets
    "NotesPreset.cpp"
    "NotesPreset.h"
)
source_group("Presets" FILES ${Presets})

set(Rendering
    "Rendering/AnchorType.h"
    "Rendering/Camera.cpp"
    "Rendering/Camera.h"
    "Rendering/Renderer.cpp"
    "Rendering/Renderer.h"
    "Rendering/Shader.cpp"
    "Rendering/Shader.h"
    "Rendering/VertexBuffer.cpp"
    "Rendering/VertexBuffer.h"
)
source_group("Rendering" FILES ${Rendering})

set(Rendering__Primitives
    "Rendering/Quad.h"
)
source_group("Rendering\\Primitives" FILES ${Rendering__Primitives})

set(Rendering__Texture
    "Rendering/Framebuffer.cpp"
    "Rendering/Framebuffer.h"
    "Rendering/Sprite.cpp"
    "Rendering/Sprite.h"
    "Rendering/Texture.cpp"
    "Rendering/Texture.h"
)
source_group("Rendering\\Texture" FILES ${Rendering__Texture})

set(Resources
    "resource.h"
)
source_group("Resources" FILES ${Resources})

set(Score
    "Score.cpp"
    "Score.h"
    "ScoreConverter.cpp"
    "ScoreConverter.h"
    "ScoreStats.cpp"
    "ScoreStats.h"
    "Tempo.cpp"
    "Tempo.h"
)
source_group("Score" FILES ${Score})

set(ScoreEditor
    "HistoryManager.cpp"
    "HistoryManager.h"
    "ScoreContext.cpp"
    "ScoreContext.h"
    "ScoreEditor.cpp"
    "ScoreEditor.h"
    "ScoreEditorTimeline.cpp"
    "ScoreEditorTimeline.h"
    "ScoreEditorWindows.cpp"
    "ScoreEditorWindows.h"
    "TimelineMode.h"
)
source_group("ScoreEditor" FILES ${ScoreEditor})

set(Score__Notes
    "Note.cpp"
    "Note.h"
    "NoteTypes.h"
)
source_group("Score\\Notes" FILES ${Score__Notes})

set(Score__SUS
    "SUS.h"
    "SusExporter.cpp"
    "SusExporter.h"
    "SusParser.cpp"
    "SusParser.h"
)
source_group("Score\\SUS" FILES ${Score__SUS})

set(UI
    "Colors.h"
    "IconsFontAwesome5.h"
    "ImGuiManager.cpp"
    "ImGuiManager.h"
    "UI.cpp"
    "UI.h"
)
source_group("UI" FILES ${UI})

set(UI__i18n
    "Language.cpp"
    "Language.h"
    "Localization.cpp"
    "Localization.h"
)
source_group("UI\\i18n" FILES ${UI__i18n})

set(ALL_FILES
    ${no_group_source_files}
    ${Audio}
    ${Audio__Lib}
    ${Graphics}
    ${IO}
    ${IO__File}
    ${Imgui}
    ${Misc}
    ${Presets}
    ${Rendering}
    ${Rendering__Primitives}
    ${Rendering__Texture}
    ${Resources}
    ${Score}
    ${ScoreEditor}
    ${Score__Notes}
    ${Score__SUS}
    ${UI}
    ${UI__i18n}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Project1)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../Depends/DirectXMath-master;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Depends/glad/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Depends/GLFW/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Depends/stb_image;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Depends/miniaudio;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Depends/json;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Depends/stb_vorbis;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Depends;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Depends/openssl/include"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG;"
            "DEBUG"
        ">"
        "_CRT_SECURE_NO_WARNINGS;"
        "_ENABLE_EXTENDED_ALIGNED_STORAGE;"
        ";"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "WIN32;"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                /Gy;
                /Ot
            >
            /permissive-;
            /std:c++17;
            /sdl;
            /W1;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /SUBSYSTEM:CONSOLE;
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /SUBSYSTEM:WINDOWS;
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    endif()
endif()

################################################################################
# Post build events
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    add_custom_command_if(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMANDS
        COMMAND   $<CONFIG:Debug> xcopy "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>res\\" "$<SHELL_PATH:${OUTPUT_DIRECTORY}>\\res\\" /E /Y
        COMMAND   $<CONFIG:Debug> xcopy "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>\\..\\Depends\\openssl\\bin\\*.dll" "$<SHELL_PATH:${OUTPUT_DIRECTORY}>" /E /Y
        COMMAND $<CONFIG:Release> xcopy "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>res\\" "$<SHELL_PATH:${OUTPUT_DIRECTORY}>\\res\\" /E /Y
        COMMAND $<CONFIG:Release> xcopy "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>\\..\\Depends\\openssl\\bin\\*.dll" "$<SHELL_PATH:${OUTPUT_DIRECTORY}>" /E /Y
    )
endif()

################################################################################
# Dependencies
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "glfw3;"
        "opengl32;"
        "Version;"
        "libcrypto;"
        "libssl"
    )
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../Depends/GLFW/lib;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Depends/openssl/lib"
    )
endif()

