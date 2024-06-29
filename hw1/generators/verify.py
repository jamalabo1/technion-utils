from common import generate_blockchain, hash
from utils import save_test_case, Data, TemplatedData

import random


def generate_test_case():
    fail = random.randint(0, 1)

    num_blocks = random.randint(1, 1000)
    # Generate a random blockchain
    blockchain = generate_blockchain(num_blocks)

    fail_index = random.randint(0, num_blocks - 1) if fail == 1 else -1
    fail_param = random.randint(0, 2)
    # Create expected output as a list of strings
    target_output = []
    for i, block in enumerate(blockchain):
        hashed = hash(block["coins"], block["sender"], block["recipient"])
        if i == fail_index:
            fc = 0
            fs = ""
            fr = ""
            if fail_param == 0:
                fc = random.randint(1, 10)
            if fail_param == 1:
                fs = random.choice([" ", "\n", "s"])
            if fail_param == 2:
                fs = random.choice([" ", "\n", "s"])
            hashed = hash(block["coins"] + fc, block["sender"] + fs, block["recipient"] + fr)
        target_output.append(hashed)
    vtype = "passed" if fail == 0 else "failed"
    expected_output = f"Verification {vtype}\n"

    return blockchain, target_output, [expected_output]


def main():
    for n in range(1, 1000):
        blockchain, target_output, expected_output = generate_test_case()
        save_test_case(
            key="verify",
            n=n,
            templated=[
                TemplatedData(
                    "source",
                    template="{sender} {recipient} {coins} {timestamp}",
                    data=blockchain
                ),
                Data(
                  "target",
                  target_output
                ),
                Data(
                    "expected",
                    expected_output
                )
            ])

    print("finished")


if __name__ == "__main__":
    main()
