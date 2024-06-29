macro(install_project_binaries target)
    install(TARGETS ${target}
            DESTINATION bin
            COMPONENT ${target}
    )
endmacro()