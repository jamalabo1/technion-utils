import os
from pathlib import Path
from typing import List
import click
import utils
from utils import op_key


class TestBlock:
    commands: List
    key: str
    working_dir: str

    def __init__(self, commands, key, working_dir=None):
        self.commands = commands
        self.key = key
        self.working_dir = working_dir


def gen_exec_command(target, op):
    args = ""
    include = not ("include" in op and (op["include"] is False))
    prefix = op["prefix"]
    source = f"{prefix}.source"
    out_dist = f"{prefix}.target.out"

    if include:
        args = f" {source} {out_dist}"

    return f"./{target} {op['command']}{args}"


def run_blocks(blocks: List[TestBlock]):
    for block in blocks:
        click.secho(f"----- {block.key} -----", bold=True, fg='green')

        for command in block.commands:
            click.echo(f"running command: {command}")
            utils.run_command(command)


def gen_exec_command2(target, option):
    variables = option["vars"]
    args = option["command"].format(**variables)
    return f"./{target} {args}"


def run_valgrind(target: str, options):
    blocks: List[TestBlock] = []

    if "explict" in options:
        for op in options["explict"]:
            exec_command = f'valgrind {gen_exec_command(target, op)}'

            key = op_key(op, "key", op["command"])

            blocks.append(
                TestBlock(
                    [
                        exec_command
                    ],
                    key
                )
            )

    run_blocks(blocks)


def run_tests(target: str, options):
    blocks: List[TestBlock] = []

    if "explict" in options:
        for op in options["explict"]:
            exec_command = gen_exec_command(target, op)

            prefix = op['prefix'] if "prefix" in op else ""
            out_dist = op["out"] if "out" in op else f"{prefix}.target.out"
            expected = op["expected"] if "expected" in op else f"{prefix}.target.expected"

            verify_command = f"diff --strip-trailing-cr -B -Z {expected} {out_dist}"

            key = op["key"] if "key" in op else op["command"]

            blocks.append(
                TestBlock(
                    [
                        exec_command,
                        verify_command
                    ],
                    key
                )
            )

    if "generated" in options:
        for op in options["generated"]:
            for test_case in Path(op["path"]).glob("*"):
                path_vars = op["path_vars"]
                variables = {key: test_case.joinpath(path_vars[key]) for key in path_vars}
                args = op["command"].format(**variables)

                exec_command = f"./{target} {args}"

                verify_command = f"diff --strip-trailing-cr -B -Z {variables['expected']} {variables['out']}"

                key = op_key(op, "key", op["command"])

                blocks.append(
                    TestBlock(
                        commands=[
                            exec_command,
                            verify_command
                        ],
                        key=key
                    )
                )

    run_blocks(blocks)
