set(CPU -mcpuc=cortex-m4 -mstructure-size-boundary=8)
set(FPU -mfpu=fpv4-sp-d16 -mfloat-abi=hard)

set(CMAKE_ASM_FLAGS -mthumb ${CPU} ${FPU} -mlittle-endian -MD)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS
        -mthumb
        ${CPU}
        ${FPU}
        -mlittle-endian
        -fno-exceptions
        -Wall
        -pedantic
        -DPART_${PART}
)
# -std=c99?
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS
        -mthumb
        ${CPU}
        ${FPU}
        -O0
        -mlittle-endian
        -fno-exceptions
        -fno-unwind-tables
        -fno-rtti
        -frepo
        -Wall
        -pedantic
        -DPART-${PART}
        -ffreestanding
        -mtpcs-frame
        -mtpcs-leaf-frame
        -flto
)
# -std=c++11

set(CMAKE_STATIC_LINKER_FLAGS --gc-sections)
set(CMAKE_EXE_LINKER_FLAGS --gc-sections)

set(LIBGCC ``)
