set(NET_SOURCES amf.cpp channel.cpp connection.cpp codec.cpp shared-object.cpp stage-id-hash.hpp rpc-hash.hpp stage-info.cpp)

add_custom_command(
    OUTPUT stage-id-hash.hpp
    COMMAND gperf ${CMAKE_CURRENT_SOURCE_DIR}/stage-id-hash.gperf > stage-id-hash.hpp
    DEPENDS stage-id-hash.gperf stage-info.hpp
)

add_custom_command(
    OUTPUT rpc-hash.hpp
    COMMAND gperf ${CMAKE_CURRENT_SOURCE_DIR}/rpc-hash.gperf > rpc-hash.hpp
    DEPENDS rpc-hash.gperf
)

add_library(net STATIC ${NET_SOURCES})

target_link_libraries(net Qt5::Network Qt5::Core)
