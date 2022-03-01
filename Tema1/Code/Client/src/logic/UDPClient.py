MAX_ACK_WAIT_TIME = 0.1


from time import time
from tqdm import tqdm

import socket
import os


class UDPClient:
    def __init__(self, host, port, benchmark_file_path, chunk_size, ack):
        self.__host = host
        self.__port = port
        self.__address = (host, port)
        self.__benchmark_file_path = benchmark_file_path
        self.__chunk_size = chunk_size
        self.__ack = ack

    def run(self):
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
            sock.settimeout(MAX_ACK_WAIT_TIME)

            timestamp = time()
            bytes_count = 0
            chunks_count = 0

            for chunk in tqdm(self.__read_file_chunks(), total=os.path.getsize(self.__benchmark_file_path) // self.__chunk_size + 1):
                sock.sendto(chunk, self.__address)
                if self.__ack:
                    self.__await_ack(sock)

                bytes_count += len(chunk)
                chunks_count += 1

        return timestamp, bytes_count, chunks_count

    def __await_ack(self, sock):
        try:
            while len(sock.recvfrom(1)) < 0:
                pass
        except:
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