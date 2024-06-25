import json
import subprocess
import os


def run_command(command: str):
    subprocess.run(command, shell=True)


def load_settings():
    settings_file = open("./settings.json", "r")
    return json.load(settings_file)


def change_permission(file):
    os.chmod(file, 0o755)
