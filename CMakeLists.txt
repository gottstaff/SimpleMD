cmake_minimum_required(VERSION 3.20)

project(simplemd VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM 6.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMFindQmlModule)
include(ECMQmlModule)
include(ECMInstallIcons)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Quick
    Gui
    Network
    QuickControls2
    Widgets
    PrintSupport
    Pdf
    WebEngineQuick
    WebChannel
)

find_package(KF6 REQUIRED COMPONENTS
    Kirigami
    I18n
    CoreAddons
    QQC2DesktopStyle
    IconThemes
)

ecm_find_qmlmodule(org.kde.kirigami REQUIRED)

add_subdirectory(src)

install(PROGRAMS io.github.gottstaff.SimpleMD.desktop DESTINATION ${KDE_INSTALL_APPDIR})

ecm_install_icons(
    ICONS
        icons/16-apps-io.github.gottstaff.SimpleMD.png
        icons/22-apps-io.github.gottstaff.SimpleMD.png
        icons/24-apps-io.github.gottstaff.SimpleMD.png
        icons/32-apps-io.github.gottstaff.SimpleMD.png
        icons/48-apps-io.github.gottstaff.SimpleMD.png
        icons/64-apps-io.github.gottstaff.SimpleMD.png
        icons/128-apps-io.github.gottstaff.SimpleMD.png
        icons/256-apps-io.github.gottstaff.SimpleMD.png
        icons/sc-apps-io.github.gottstaff.SimpleMD.svg
    DESTINATION ${KDE_INSTALL_ICONDIR}
    THEME hicolor
)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
