SET (sources
    main.cpp
)

SET (headers
)

add_executable (vis ${sources} ${headers})
target_link_libraries (vis ${LIBS})