
set(SOURCES
    bbattery_runner.cpp
)

include_directories(${SMHASHER_DIR})
set(SMHASHER_SOURCES
    ${SMHASHER_DIR}/MurmurHash1.cpp ${SMHASHER_DIR}/MurmurHash1.h
    ${SMHASHER_DIR}/MurmurHash2.cpp ${SMHASHER_DIR}/MurmurHash2.h
    ${SMHASHER_DIR}/MurmurHash3.cpp ${SMHASHER_DIR}/MurmurHash3.h
   )

add_definitions( -DHAVE_UINT64_T ) # for siphash
include_directories(${SIPHASH_C_DIR})
set(SIPHASH_C_SOURCES
    ${SIPHASH_C_DIR}/siphash.h
    ${SIPHASH_C_DIR}/siphash.c
)

add_executable( bbattery_runner  ${SOURCES} ${SMHASHER_SOURCES} ${SIPHASH_C_SOURCES} )
target_link_libraries( bbattery_runner pcg_random_lib ${TESTU01_LIBRARIES} )
