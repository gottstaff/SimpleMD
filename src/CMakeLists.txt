add_executable(simplemd)

ecm_add_qml_module(simplemd
    URI io.github.gottstaff.SimpleMD
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
        pdfexportthemes.cpp
        pdfexportthemes.h
        appintegration.cpp
        appintegration.h
        credentialstore.cpp
        credentialstore.h
)

get_filename_component(PREVIEW_RES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../resources/preview" ABSOLUTE)

file(GLOB katex_fonts CONFIGURE_DEPENDS "${PREVIEW_RES_DIR}/vendor/fonts/*.woff2")

get_filename_component(_project_root "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
set(_app_icon_png "${_project_root}/icons/128-apps-io.github.gottstaff.SimpleMD.png")
set_source_files_properties("${_app_icon_png}" PROPERTIES QT_RESOURCE_ALIAS "io.github.gottstaff.SimpleMD.png")

qt_add_resources(simplemd "app_icons"
    PREFIX "/icons"
    FILES "${_app_icon_png}"
)

get_filename_component(PDF_THEMES_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../resources/pdf-themes.json" ABSOLUTE)
set_source_files_properties("${PDF_THEMES_FILE}" PROPERTIES QT_RESOURCE_ALIAS "pdf-themes.json")

qt_add_resources(simplemd "pdf_themes"
    PREFIX "/"
    FILES "${PDF_THEMES_FILE}"
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
        Qt6Keychain::Qt6Keychain
        KF6::I18n
        KF6::CoreAddons
        KF6::IconThemes
)

install(TARGETS simplemd ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
