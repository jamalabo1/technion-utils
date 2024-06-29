import json
import subprocess
import os


def run_command(command: str, working_dir=None):
    return subprocess.run(command, cwd=working_dir, shell=True)


def load_settings():
    settings_file = open("./settings.json", "r")
    return json.load(settings_file)


def change_permission(file):
    os.chmod(file, 0o755)


def op_key(obj, key, default_val):
    return obj[key] if key in obj else default_val
