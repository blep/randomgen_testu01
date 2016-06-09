
set(SOURCES
    bbattery_runner.cpp
)

include_directories(${SMHASHER_DIR})
set(SMHASHER_SOURCES
    ${SMHASHER_DIR}/MurmurHash1.cpp ${SMHASHER_DIR}/MurmurHash1.h
    ${SMHASHER_DIR}/MurmurHash2.cpp ${SMHASHER_DIR}/MurmurHash2.h
    ${SMHASHER_DIR}/MurmurHash3.cpp ${SMHASHER_DIR}/MurmurHash3.h
   )

add_executable( bbattery_runner  ${SOURCES} ${SMHASHER_SOURCES} )
target_link_libraries( bbattery_runner pcg_random_lib ${TESTU01_LIBRARIES} )
