import subprocess

from testcase import RunnerCase


def test_target(target, test_case: RunnerCase):
    for command in test_case.commands:
        command_result = command.runner()
        assert command_result
