from typing import List, Callable

from engine.utils import run_command


class RunnerCommand:
    runner: Callable[[], bool] = None

    def __init__(self, runner):
        self.runner = runner


class RunnerCase:
    key: str
    commands: List[RunnerCommand]

    def __init__(self, key, commands):
        self.key = key
        self.commands = commands

    def __str__(self):
        return self.key


def shell_command_runner(command) -> RunnerCommand:
    return RunnerCommand(
        lambda: run_command(command) == 0
    )


def diff_command_runner(command) -> RunnerCommand:
    return RunnerCommand(
        lambda: diff_command_lambda(command)
    )


def diff_command_lambda(command):
    result = run_command(command, return_output=True)
    return result == b''
