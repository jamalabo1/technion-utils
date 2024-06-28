import random
from typing import List


def generate_blockchain(num_blocks) -> List[dict]:
    blockchain = []
    senders = ['A', 'B', 'C', 'D']
    recipients = ['X', 'Y', 'W', 'Z']

    for i in range(num_blocks):
        block = {
            'sender': random.choice(senders),
            'recipient': random.choice(recipients),
            'coins': random.randint(0, 1000),
            'timestamp': i + 1
        }
        blockchain.append(block)

    return blockchain


def hash(key, value1, value2):
    out_size = 20
    bytes = [0] * (out_size + 1)

    # First loop
    for i in range(out_size):
        bytes[i] = key ^ (out_size - i)

    # Second loop
    for i in range(value1.__len__() * out_size):
        bytes[i % (out_size // 2)] ^= ord(value1[i % len(value1)]) ^ i

    # Third loop
    for i in range(value2.__len__() * out_size):
        bytes[(out_size // 2) + (i % out_size // 2)] ^= ord(value2[i % len(value2)]) ^ i

    # Fourth loop
    for i in range(out_size):
        bytes[i] &= 0x0f
        bytes[i] += ord('0') if bytes[i] < 10 else ord('a') - 10

    return ''.join(chr(byte) for byte in bytes[:-1])
