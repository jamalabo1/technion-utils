import click
import utils
from runner import run_tests, run_valgrind


@click.command()
@click.option('--valgrind', is_flag=True, help='run valgrind checks on commands')
def cli(valgrind):
    settings = utils.load_settings()
    for target in settings:
        utils.change_permission(target)
        if valgrind:
            run_valgrind(target, settings[target])
        else:
            run_tests(target, settings[target])


if __name__ == '__main__':
    cli()
