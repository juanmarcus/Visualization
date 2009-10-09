SET (sources
    CommandLineParser.cpp
)

SET (headers
    CommandLineParser.h
)

SET (full_sources
    ${sources}
    ${headers}
)

ADD_LIBRARY(cmdline STATIC ${full_sources})
TARGET_LINK_LIBRARIES (cmdline ${LIBS})