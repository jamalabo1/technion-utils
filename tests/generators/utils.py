from __future__ import annotations

import os
from typing import List


def format_data(template: str, data) -> List[str]:
    result: List[str] = []
    for d in data:
        result.append(
            template.format(**d)
        )
    return result


class Data:
    key: str
    data = []

    def __init__(self, key, data):
        self.key = key
        self.data = data


class TemplatedData(Data):
    template: str

    def __init__(self, key, template, data):
        super().__init__(key, data)
        self.template = template


def save_test_case(key: str,
                   n: int,
                   templated: List[Data | TemplatedData]):
    for d in templated:
        cdir = f"out/{key}/{n}"
        os.makedirs(cdir, exist_ok=True)
        f = open(f"{cdir}/{key}.{d.key}", "w")
        if isinstance(d, TemplatedData):
            fr = format_data(d.template, d.data)
        else:
            fr = d.data
        f.write("\n".join(fr))
