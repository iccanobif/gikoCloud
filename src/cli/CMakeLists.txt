add_executable(cli cli.cpp connectionwrapper.cpp)
target_link_libraries(cli net Qt5::Core)
