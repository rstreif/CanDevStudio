find_package(Qt5SerialBus REQUIRED)

set(srcs
    component.cpp
    ../components/socketcan.cpp
)

add_executable(common ${srcs})
