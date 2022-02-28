from time import time
from tqdm import tqdm

import socket
import os


class TCPClient:
    def __init__(self, host, port, benchmark_file_path, chunk_size, ack):
        self.__host = host
        self.__port = port
        self.__benchmark_file_path = benchmark_file_path
        self.__chunk_size = chunk_size
        self.__ack = ack

    def run(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.connect((self.__host, self.__port))

            print(f"Connected to {self.__host}:{self.__port}")

            timestamp = time()
            bytes_count = 0
            chunks_count = 0

            for chunk in tqdm(self.__read_file_chunks(), total=os.path.getsize(self.__benchmark_file_path) // self.__chunk_size + 1):
                sock.send(chunk)
                if self.__ack:
                    self.__await_ack(sock)

                bytes_count += len(chunk)
                chunks_count += 1

        return timestamp, bytes_count, chunks_count

    def __await_ack(self, sock):
        while len(sock.recv(1)) < 0:
            pass

    def __read_file_chunks(self):
        with open(self.__benchmark_file_path, "rb") as benchmark_file:
            data_available = True
            while data_available:
                chunk = benchmark_file.read(self.__chunk_size)
                if not chunk:
                    data_available = False
                else:
                    yield chunk