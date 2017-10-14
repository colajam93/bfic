set(HEADER_FILES
        ../include/ArgParse.hpp
        ../include/Optional.hpp
        ../include/String.hpp)
set(SOURCE_FILES
        ${HEADER_FILES}
        main.cpp
        ArgParse.cpp
        )
add_executable(bfic ${SOURCE_FILES})
