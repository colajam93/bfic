set(HEADER_FILES
        ../include/Optional.hpp
        ../include/String.hpp)
set(SOURCE_FILES
        ${HEADER_FILES}
        main.cpp
        )
add_executable(bfic ${SOURCE_FILES})
