import random

from common import generate_blockchain
from utils import save_test_case, Data, TemplatedData


def generate_test_case():
    num_blocks = random.randint(1, 1000)
    # Generate a random blockchain
    blockchain = generate_blockchain(num_blocks)

    # Create expected output as a list of strings
    expected_output = ["BlockChain Info:"]
    for i, block in enumerate(blockchain):
        sender = block['sender']
        receiver = block['recipient']
        value = block['coins']
        timestamp = block['timestamp']

        expected_output.extend([
            f"{i + 1}.",
            f"Sender Name: {sender}",
            f"Receiver Name: {receiver}",
            f"Transaction Value: {value}",
            f"Transaction timestamp: {timestamp}"
        ])

    return blockchain, expected_output


def main():
    for n in range(1, 1000):
        blockchain, expected_output = generate_test_case()
        save_test_case(
            key="format",
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
