add_executable(simplemd)

ecm_add_qml_module(simplemd
    URI org.kde.simplemd
)

target_sources(simplemd
    PRIVATE
        main.cpp
        documentcontroller.cpp
        documentcontroller.h
        documentassets.cpp
        documentassets.h
        imageresolver.cpp
        imageresolver.h
        editorhelper.cpp
        editorhelper.h
        bracketmatchhighlighter.cpp
        bracketmatchhighlighter.h
        llmclient.cpp
        llmclient.h
        previewhelper.cpp
        previewhelper.h
        printhelper.cpp
        printhelper.h
)

get_filename_component(PREVIEW_RES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../resources/preview" ABSOLUTE)

file(GLOB katex_fonts CONFIGURE_DEPENDS "${PREVIEW_RES_DIR}/vendor/fonts/*.woff2")

get_filename_component(_project_root "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
set(_app_icon_png "${_project_root}/icons/128-apps-org.kde.simplemd.png")
set_source_files_properties("${_app_icon_png}" PROPERTIES QT_RESOURCE_ALIAS "org.kde.simplemd.png")

qt_add_resources(simplemd "app_icons"
    PREFIX "/icons"
    FILES "${_app_icon_png}"
)

qt_add_resources(simplemd "preview_assets"
    PREFIX "/preview"
    BASE "${PREVIEW_RES_DIR}"
    FILES
        "${PREVIEW_RES_DIR}/preview.html"
        "${PREVIEW_RES_DIR}/vendor/marked.min.js"
        "${PREVIEW_RES_DIR}/vendor/katex.min.js"
        "${PREVIEW_RES_DIR}/vendor/katex.min.css"
        "${PREVIEW_RES_DIR}/vendor/contrib/mhchem.min.js"
        "${PREVIEW_RES_DIR}/vendor/mermaid.min.js"
        ${katex_fonts}
)

ecm_target_qml_sources(simplemd
    SOURCES
        Main.qml
        SplitEditor.qml
        DocumentOutlinePanel.qml
        MarkdownEditor.qml
        StyledScrollBar.qml
        documentoutline.js
        editorhelpers.js
        editorfonts.js
)

target_link_libraries(simplemd
    PRIVATE
        Qt6::Quick
        Qt6::Qml
        Qt6::Gui
        Qt6::Network
        Qt6::QuickControls2
        Qt6::Widgets
        Qt6::PrintSupport
        Qt6::Pdf
        Qt6::WebEngineQuick
        Qt6::WebChannel
        KF6::I18n
        KF6::CoreAddons
        KF6::IconThemes
)

install(TARGETS simplemd ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
