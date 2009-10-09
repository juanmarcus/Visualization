SET (sources
    Exception.cpp
)

SET (headers
    Exception.h
)

SET (full_sources
    ${sources}
    ${headers}
)

ADD_LIBRARY(error STATIC ${full_sources})
TARGET_LINK_LIBRARIES (error ${LIBS})