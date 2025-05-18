macro(load_asm file)
    get_filename_component(BASE_NAME ${ASM_FILE} NAME_WE)
    set(OBJ_FILE ${BUILD_DIR}/${BASE_NAME}.o)

    set(asm_start "${ASM_START_METHOD}_${BASE_NAME}")

    add_custom_command(
            OUTPUT ${OBJ_FILE}
            COMMAND as -g -o ${OBJ_FILE} ${CMAKE_SOURCE_DIR}/${ASM_FILE} ${CMAKE_SOURCE_DIR}/returnable.asm
            && objcopy --redefine-sym _start=${asm_start} ${OBJ_FILE} ${OBJ_FILE}
            DEPENDS ${ASM_FILE}
            COMMENT "Assembling ${ASM_FILE} -> ${OBJ_FILE}"
    )
#    set_source_files_properties(runner/modules/${BASE_NAME}.cpp PROPERTIES COMPILE_DEFINITIONS "ASM_START_METHOD=${asm_start}")

    message(STATUS "asm_start=${asm_start}")

    list(APPEND ASM_OBJECTS ${OBJ_FILE})
endmacro()

