from common import generate_blockchain, hash
from utils import save_test_case, Data, TemplatedData

import random


def generate_test_case():
    num_blocks = random.randint(1, 1000)
    # Generate a random blockchain
    blockchain = generate_blockchain(num_blocks)

    # Create expected output as a list of strings
    expected_output = []
    for i, block in enumerate(blockchain):
        expected_output.append(hash(block["coins"], block["sender"], block["recipient"]))

    return blockchain, expected_output


def main():
    for n in range(1, 1000):
        blockchain, expected_output = generate_test_case()
        save_test_case(
            key="hash",
            n=n,
            templated=[
                TemplatedData(
                    "source",
                    template="{sender} {recipient} {coins} {timestamp}",
                    data=blockchain
                ),
                Data(
                    "expected",
                    expected_output
                )
            ])

    print("finished")


if __name__ == "__main__":
    main()
