#set(PART TM4C123GH6PGE)
set(PART TM4C123GH6PM)

if(${PART} MATCHES "TM4C123GH6PGE")
    message(AUTHOR_WARNING "COMPILING FOR GENERIC PCB!")
elseif(${PART} MATCHES "TM4C123GH6PM")
    message(AUTHOR_WARNING "COMPILING FOR TIVA BOARD!")
else()
    message(FATAL_ERROR "PART NOT DEFINED!")
endif()

include(${BUILD_TOOLS_DIR}/compiler_makedefs.cmake)
include(${BUILD_TOOLS_DIR}/ti_makedefs.cmake)
