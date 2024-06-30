from pathlib import Path

from engine import utils
from engine import common
from engine.testcase import shell_command_runner, diff_command_runner, RunnerCase


def pytest_addoption(parser):
    parser.addoption(
        "--valgrind",
        action="store_true",
        default=False,
        help="run tests with valgrind"
    )


def pytest_generate_tests(metafunc):
    test_target_data = []
    idslist = []
    settings = utils.load_settings()
    valgrind = metafunc.config.getoption("valgrind")

    for target in settings:
        utils.change_permission(target)

        options = settings[target]
        if "generated" in options:
            for op in options["generated"]:
                for test_case in Path(op["path"]).glob("*"):
                    path_vars = op["path_vars"]
                    variables = {key: test_case.joinpath(path_vars[key]) for key in path_vars}

                    gen_commands = common.get_generated_test_commands(target, test_case, op, variables, valgrind)
                    commands = [
                        shell_command_runner(gen_commands["exec_command"])
                    ]
                    if not valgrind:
                        commands.append(
                            diff_command_runner(gen_commands["verify_command"])
                        )

                    runner_case = RunnerCase(
                        op["command"],
                        commands
                    )

                    test_target_data.append(
                        (target, runner_case)
                    )
                    idslist.append(
                        f"{target}-{test_case}"
                    )

    metafunc.parametrize("target,test_case", test_target_data, ids=idslist)
