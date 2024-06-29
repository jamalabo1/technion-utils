from pathlib import Path

from engine import utils
from engine import common


def pytest_generate_tests(metafunc):
    test_target_data = []
    settings = utils.load_settings()
    for target in settings:
        options = settings[target]
        if "generated" in options:
            for op in options["generated"]:
                for test_case in Path(op["path"]).glob("*"):
                    path_vars = op["path_vars"]
                    variables = {key: test_case.joinpath(path_vars[key]) for key in path_vars}

                    commands = common.get_generated_test_commands(target, op, variables)
                    test_target_data.append(
                        (
                            target,
                            op["command"],
                            commands["exec_command"],
                            commands["verify_command"]
                        )
                    )

    metafunc.parametrize("target,command,exec,verify", test_target_data)
