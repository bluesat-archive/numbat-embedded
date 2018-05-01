# PLATFORM (ARM)
set(CMAKE_SYSTEM_NAME  Generic)
# Cross-compiler prefix, may need to change this on your system
set(COMPILER_PREFIX arm-none-eabi)
#enable_language(ASM)
# The commands for calling the compiler.
set(CMAKE_C_COMPILER  ${COMPILER_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER  ${COMPILER_PREFIX}-g++)
set(CMAKE_LINKER ${COMPILER_PREFIX}-g++)
set(CMAKE_AR ${COMPILER_PREFIX}-ar)



