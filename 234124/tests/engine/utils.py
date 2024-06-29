import json
import subprocess
import os


def run_command(command: str, return_output: bool = False):
    if return_output:
        result = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE).stdout.read()
        print(result)
        return result
    result = subprocess.run(command, shell=True)
    print(result.returncode)
    return result.returncode


def load_settings():
    settings_file = open("./settings.json", "r")
    return json.load(settings_file)


def change_permission(file):
    os.chmod(file, 0o755)


def op_key(obj, key, default_val):
    return obj[key] if key in obj else default_val
