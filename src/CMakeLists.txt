
set(SOURCES
    bbattery_runner.cpp
)


add_executable( bbattery_runner  ${SOURCES} )
target_link_libraries( bbattery_runner pcg_random_lib ${TESTU01_LIBRARIES} )
