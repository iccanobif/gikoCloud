add_executable(cli cli.cpp connectionwrapper.cpp controller.cpp cliparameters.hpp)
target_link_libraries(cli net Qt5::Core)
