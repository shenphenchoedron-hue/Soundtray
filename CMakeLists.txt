cmake_minimum_required(VERSION 3.22)

# Static MSVC runtime -> no Visual C++ redistributable needed on Windows.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS version")

project(Soundtray VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

# ---------------------------------------------------------------- JUCE
# Pass -DFETCHCONTENT_SOURCE_DIR_JUCE=/path/to/JUCE to use a local checkout.
include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.4
    GIT_SHALLOW    TRUE)
FetchContent_MakeAvailable(JUCE)

# ---------------------------------------------------------------- App
juce_add_gui_app(Soundtray
    PRODUCT_NAME          "Soundtray"
    COMPANY_NAME          "Soundtray"
    BUNDLE_ID             "io.github.shenphenchoedronhue.soundtray"
    VERSION               ${PROJECT_VERSION}
    ICON_BIG              "${CMAKE_CURRENT_SOURCE_DIR}/packaging/icon.png"
    MICROPHONE_PERMISSION_ENABLED FALSE)

target_sources(Soundtray PRIVATE
    Source/Main.cpp
    Source/MainComponent.cpp)

target_compile_definitions(Soundtray PRIVATE
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:Soundtray,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:Soundtray,JUCE_VERSION>"
    JUCE_USE_MP3AUDIOFORMAT=1
    JUCE_USE_FLAC=1
    JUCE_USE_OGGVORBIS=1
    JUCE_USE_CURL=0
    JUCE_WEB_BROWSER=0
    JUCE_LOAD_CURL_SYMBOLS_LAZILY=1
    JUCE_REPORT_APP_USAGE=0
    JUCE_STRICT_REFCOUNTEDPOINTER=1)

# Only the modules we actually need -> small binary.
target_link_libraries(Soundtray
    PRIVATE
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_gui_basics
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

if(MSVC)
    target_compile_options(Soundtray PRIVATE /utf-8 $<$<CONFIG:Release>:/O1>)
else()
    target_compile_options(Soundtray PRIVATE $<$<CONFIG:Release>:-Os>)
    if(APPLE)
        target_link_options(Soundtray PRIVATE $<$<CONFIG:Release>:-Wl,-dead_strip>)
    else()
        target_compile_options(Soundtray PRIVATE -ffunction-sections -fdata-sections)
        target_link_options(Soundtray PRIVATE $<$<CONFIG:Release>:-Wl,--gc-sections>)
    endif()
endif()

# ---------------------------------------------------------------- Install / packaging
if(APPLE)
    install(TARGETS Soundtray BUNDLE DESTINATION . COMPONENT app)
elseif(WIN32)
    install(TARGETS Soundtray RUNTIME DESTINATION . COMPONENT app)
else()
    install(TARGETS Soundtray RUNTIME DESTINATION bin COMPONENT app)
    install(FILES packaging/linux/soundtray.desktop DESTINATION share/applications COMPONENT app)
    install(FILES packaging/icon.png DESTINATION share/icons/hicolor/512x512/apps RENAME soundtray.png COMPONENT app)
endif()

set(CPACK_PACKAGE_NAME                "Soundtray")
set(CPACK_PACKAGE_VENDOR              "Soundtray")
set(CPACK_PACKAGE_VERSION             ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Soundtray - en lille og let playlist-afspiller")
set(CPACK_PACKAGE_INSTALL_DIRECTORY   "Soundtray")
set(CPACK_PACKAGE_EXECUTABLES         "Soundtray" "Soundtray")
set(CPACK_PACKAGE_CONTACT             "shenphenchoedron-hue <shenphenchoedron-hue@users.noreply.github.com>")
# Package only our own files (not JUCE's tools/headers).
set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};Soundtray;app;/")
set(SOUNDTRAY_PKG_SUFFIX "" CACHE STRING "Suffix used in package file names (fx ubuntu22.04)")

if(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "Soundtray")
    set(CPACK_PACKAGE_FILE_NAME "Soundtray-${PROJECT_VERSION}-macOS")
elseif(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_NSIS_DISPLAY_NAME "Soundtray")
    set(CPACK_NSIS_PACKAGE_NAME "Soundtray")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "Soundtray.exe")
    set(CPACK_NSIS_MUI_FINISHPAGE_RUN "Soundtray.exe")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_CREATE_DESKTOP_LINKS "Soundtray")
    set(CPACK_PACKAGE_FILE_NAME "Soundtray-${PROJECT_VERSION}-Windows-Setup")
else()
    set(CPACK_GENERATOR "DEB")
    set(CPACK_STRIP_FILES ON)
    set(CPACK_DEBIAN_PACKAGE_NAME "soundtray")
    set(CPACK_DEBIAN_PACKAGE_SECTION "sound")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    # JUCE loads X11 libraries at runtime, so list them explicitly.
    set(CPACK_DEBIAN_PACKAGE_DEPENDS
        "libx11-6, libxext6, libxinerama1, libxrandr2, libxcursor1, libxrender1, libfreetype6, libfontconfig1, libasound2 | libasound2t64")
    set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "zenity")
    set(CPACK_DEBIAN_FILE_NAME "soundtray_${PROJECT_VERSION}_${SOUNDTRAY_PKG_SUFFIX}_amd64.deb")
endif()

include(CPack)
