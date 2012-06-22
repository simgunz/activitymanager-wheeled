project(plasma-applet-activitymanager)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

set(activitymanager_SRCS
    activitywidget.cpp
    activitymanager.cpp)

kde4_add_plugin(plasma_applet_activitymanager ${activitymanager_SRCS})
target_link_libraries(plasma_applet_activitymanager ${KDE4_PLASMA_LIBS})

install(TARGETS plasma_applet_activitymanager DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-applet-activitymanager.desktop DESTINATION ${SERVICES_INSTALL_DIR})
