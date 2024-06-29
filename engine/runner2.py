import subprocess
import conftest


def test_target(target, command, exec, verify):
    print("running")
    subprocess.run(exec)
    subprocess.run(verify)
