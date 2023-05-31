set(AppName VisualCraft)

configure_file(${CMAKE_SOURCE_DIR}/cmake/deploy_qt.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake
    @ONLY)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    install(TARGETS VisualCraft
        RUNTIME DESTINATION .
    )

    install(FILES vc-config.json
        DESTINATION .)

    # Run windeployqt at build time
    add_custom_target(Windeployqt-VisualCraft
        COMMAND ${SlopeCraft_Qt_windeployqt_executable} VisualCraft.exe ${SlopeCraft_windeployqt_flags_build}
        COMMAND_EXPAND_LISTS
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS VisualCraft)
    add_dependencies(SC_deploy_all Windeployqt-VisualCraft)

    # Run windeployqt at install time
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake)
    return()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    install(TARGETS VisualCraft
        RUNTIME DESTINATION bin
    )

    install(FILES vc-config.json
        DESTINATION bin)

    # Install platforms and imageformats plugins
    include(${CMAKE_SOURCE_DIR}/cmake/install_plugins.cmake)

    return()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    include(${CMAKE_SOURCE_DIR}/VisualCraftL/setup_zip_names.cmake)
    install(TARGETS VisualCraft
        RUNTIME DESTINATION .
        BUNDLE DESTINATION .)

    # Install icons
    file(GLOB SlopeCraft_Icon
        ${CMAKE_SOURCE_DIR}/VisualCraft/others/VisualCraft.icns)
    install(FILES ${SlopeCraft_Icon}
        DESTINATION VisualCraft.app/Contents/Resources)

    # Install config json file, VisualCraft will try to find it by ./vc-config.json
    install(FILES vc-config.json
        DESTINATION VisualCraft.app/Contents/MacOS)

    # Install zips. In vccl-config.json or vc-config.json, they are referred like ./Blocks_VCL/Vanilla_1_19_3.zip
    install(FILES ${VCL_app_files}
        DESTINATION VisualCraft.app/Contents/MacOS/Blocks_VCL)

    # Run macdeployqt at install time
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake)

    return()
endif()