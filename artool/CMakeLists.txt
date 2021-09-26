set(artool_SRCS
        Main.cpp
        )

set(artool_HEADERS
        Main.h
        )

assign_source_group(${artool_SRCS} ${artool_HEADERS})

add_executable(artool ${artool_SRCS} ${artool_HEADERS})

set(artool_LIBS vcmi)
if(CMAKE_SYSTEM_NAME MATCHES FreeBSD)
    set(artool_LIBS execinfo ${artool_LIBS})
endif()
target_link_libraries(artool PRIVATE ${artool_LIBS})

target_include_directories(artool
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        )

if(WIN32)
    set_target_properties(artool
            PROPERTIES
            OUTPUT_NAME "ARTOOL"
            PROJECT_LABEL "ARTOOL"
            )
endif()

vcmi_set_output_dir(artool "")

set_target_properties(artool PROPERTIES ${PCH_PROPERTIES})
#cotire(artool)

install(TARGETS artool DESTINATION ${BIN_DIR})
