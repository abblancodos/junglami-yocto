cmake_minimum_required(VERSION 3.16)
project(radxactl C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDBUS REQUIRED libsystemd)

add_executable(radxactl
    src/main.c
    src/daemon/bus.c
    src/daemon/modules/power.c
    src/daemon/modules/bluetooth.c
    src/daemon/modules/wifi.c
    src/daemon/modules/audio.c
    src/daemon/modules/aic8800.c
    src/cli/cli.c
)

target_include_directories(radxactl PRIVATE
    src
    ${SDBUS_INCLUDE_DIRS}
)

target_link_libraries(radxactl PRIVATE
    ${SDBUS_LIBRARIES}
)

target_compile_options(radxactl PRIVATE
    -Wall -Wextra -Wno-unused-parameter
)

install(TARGETS radxactl DESTINATION bin)
install(FILES radxactl.service DESTINATION lib/systemd/system)
install(FILES cr.junglami.radxactl.conf DESTINATION share/dbus-1/system.d)