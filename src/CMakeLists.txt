FILE(GLOB c_files "*.c")
FILE(GLOB header_files "*.h")

include_directories(../t-digest-c/src/)

set( qoo_SOURCES ${qoo_SOURCES}
    simple-qoo.c simple-qoo.h
    ringbuffer.c ringbuffer.h
    tdigest-extras.c tdigest-extras.h
    ../t-digest-c/src/tdigest.c ../t-digest-c/src/tdigest.h
)

if (BUILD_SHARED)
    add_library(qoo SHARED ${qoo_SOURCES})
    target_link_libraries(qoo m)
    target_include_directories(qoo SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    set_target_properties(qoo PROPERTIES PUBLIC_HEADER "${header_files}")
    install(TARGETS qoo DESTINATION lib${LIB_SUFFIX} PUBLIC_HEADER DESTINATION include)
endif(BUILD_SHARED)

if (BUILD_STATIC) 
    add_library(qoo_static STATIC ${qoo_SOURCES}) 
    target_link_libraries(qoo_static m)
    target_include_directories(qoo_static SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    set_target_properties(qoo_static PROPERTIES PUBLIC_HEADER "${header_files}")
    install(TARGETS qoo_static DESTINATION lib${LIB_SUFFIX} PUBLIC_HEADER DESTINATION include)
endif(BUILD_STATIC)