
set(SRC
    candevice.cpp
)

add_library(components SHARED ${SRC})
target_link_libraries(components Qt5::Core Qt5::SerialBus)

