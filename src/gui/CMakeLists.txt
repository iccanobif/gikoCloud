qt5_add_resources(ui_SRCS resources.qrc)
add_executable(SimpleChat simple-chat.cpp ${ui_SRCS})
target_link_libraries(SimpleChat Qt5::Qml Qt5::Quick Qt5::QuickControls2 net)
