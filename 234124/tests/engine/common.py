def get_generated_test_commands(target: str, test_case:str, options, variables, valgrind: bool = False):
    args = options["command"].format(**variables)
    exec_command = f"./{target} {args}"
    verify_command = f"diff --strip-trailing-cr -B -Z {variables['expected']} {variables['out']}"

    if valgrind:
        exec_command = (f"valgrind --leak-check=full --show-leak-kinds=all "
                        f" --track-origins=yes --error-exitcode=1 --verbose --log-file={test_case}/valgrind-out.txt"
                        f" {exec_command}")
        verify_command = ""

    return {
        "exec_command": exec_command,
        "verify_command": verify_command
    }
