from typing import List
import click
import utils


class TestBlock:
    commands: List
    key: str

    def __init__(self, commands, key):
        self.commands = commands
        self.key = key


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


def run_valgrind(target: str, options):
    blocks: List[TestBlock] = []

    if "explict" in options:
        for op in options["explict"]:
            exec_command = f'valgrind {gen_exec_command(target, op)}'

            key = op["key"] if "key" in op else op["command"]

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

            prefix = op['prefix']
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

    run_blocks(blocks)
