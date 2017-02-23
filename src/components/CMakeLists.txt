
set(SRC
    socketcan.cpp
)

add_library(components STATIC ${SRC})
target_link_libraries(components Qt5::Core Qt5::SerialBus)

