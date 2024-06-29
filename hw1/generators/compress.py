import random

from common import generate_blockchain
from utils import TemplatedData, Data, save_test_case


def compress_blockchain(blockchain):
    expected_output = ["BlockChain Info:"]
    i = 0
    n = len(blockchain)
    real_index = 0

    while i < n:
        current_block = blockchain[i]
        sender = current_block['sender']
        recipient = current_block['recipient']
        total_coins = current_block['coins']
        last_timestamp = current_block['timestamp']

        # Look ahead to compress consecutive blocks with same sender and recipient
        while i + 1 < n and blockchain[i + 1]['sender'] == sender and blockchain[i + 1]['recipient'] == recipient:
            total_coins += blockchain[i + 1]['coins']
            # last_timestamp = blockchain[i + 1]['timestamp']
            i += 1
        real_index += 1
        expected_output.extend([
            f"{real_index}.",
            f"Sender Name: {sender}",
            f"Receiver Name: {recipient}",
            f"Transaction Value: {total_coins}",
            f"Transaction timestamp: {last_timestamp}"
        ])

        i += 1

    return expected_output


def generate_test_case():
    num_blocks = random.randint(1, 1000)

    blockchain = generate_blockchain(num_blocks)
    blockchain = sorted(blockchain, key=lambda x: x["timestamp"], reverse=True)
    expected_output = compress_blockchain(blockchain)

    return blockchain, expected_output


def main():
    for n in range(1, 1000):
        blockchain, expected_output = generate_test_case()
        save_test_case(
            key="compress",
            n=n,
            templated=[
                TemplatedData(
                    "source",
                    "{sender} {recipient} {coins} {timestamp}",
                    blockchain
                ),
                Data(
                    "expected",
                    expected_output
                )
            ])

    print("finished")


if __name__ == "__main__":
    main()
