def get_generated_test_commands(target: str, options, variables):
    args = options["command"].format(**variables)
    verify_command = f"diff --strip-trailing-cr -B -Z {variables['expected']} {variables['out']}"

    return {
        "exec_command": f"./{target} {args}",
        "verify_command": verify_command
    }
