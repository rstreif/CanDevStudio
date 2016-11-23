find_package(Qt5SerialBus REQUIRED)

set(srcs
    component.cpp
)

add_executable(common ${srcs})
