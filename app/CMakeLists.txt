include_directories( ${KDE4_INCLUDES} )

set(yakuake_SRCS
    main.cpp
    application.cpp
    mainwindow.cpp
    skin.cpp
    titlebar.cpp
    tabbar.cpp
    sessionstack.cpp
    session.cpp
    terminal.cpp
    splitter.cpp
    firstrundialog.cpp
    visualeventoverlay.cpp
    config/windowsettings.cpp
    config/appearancesettings.cpp
    config/skinlistdelegate.cpp
    config/autostartsettings.cpp
    config/sessionlistview.cpp
    config/sessioneditdialog.cpp
    config/sessionsettings.cpp
    config/terminalsettings.cpp
    config/connectionsettings.cpp
    config/common.cpp
    config/preferences.cpp
)

kde4_add_kcfg_files(yakuake_SRCS config/settings.kcfgc)

kde4_add_ui_files(yakuake_SRCS
    firstrundialog.ui
    config/windowsettings.ui
    config/behaviorsettings.ui
    config/appearancesettings.ui
    config/autostartsettings.ui
    config/sessioneditdialog.ui
    config/terminaleditdialog.ui
)

kde4_add_app_icon(yakuake_SRCS "icons/hi*-app-yakuake.png")

kde4_add_executable(yakuake ${yakuake_SRCS})

target_link_libraries(yakuake ${KDE4_KNOTIFYCONFIG_LIBRARY} ${KDE4_KPARTS_LIBS} ${KDE4_KNEWSTUFF3_LIBS})

if(Q_WS_X11)
  target_link_libraries(yakuake ${X11_X11_LIB})
endif(Q_WS_X11)

install(TARGETS yakuake ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES yakuake.knsrc DESTINATION ${CONFIG_INSTALL_DIR})
