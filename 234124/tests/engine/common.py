def get_generated_test_commands(target: str, options, variables, valgrind: bool = False):
    args = options["command"].format(**variables)
    exec_command = f"./{target} {args}"
    verify_command = f"diff --strip-trailing-cr -B -Z {variables['expected']} {variables['out']}"

    if valgrind:
        exec_command = f"valgrind {exec_command}"
        verify_command = ""

    return {
        "exec_command": exec_command,
        "verify_command": verify_command
    }
