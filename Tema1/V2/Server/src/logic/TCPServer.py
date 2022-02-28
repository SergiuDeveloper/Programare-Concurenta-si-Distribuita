import socket

from threading import Thread, Lock
from time import time

from .BenchmarkData import BenchmarkData


class TCPServer:
    def __init__(self, host, port, benchmark_file_path, chunk_size, ack):
        self.__host = host
        self.__port = port
        self.__running = False
        self.__running_lock = Lock()
        self.__benchmark_file_path = benchmark_file_path
        self.__chunk_size = chunk_size
        self.__ack = ack

    def run(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.bind((self.__host, self.__port))
            sock.listen()

            print(f"Listening on {self.__host}:{self.__port}")

            self.__set_running(True)

            while self.__get_running():
                conn, addr = sock.accept()
                handle_client_thread = Thread(target=self.__handle_client, args=(conn, addr,))
                handle_client_thread.start()

    def stop(self):
        self.__set_running(False)

    def __handle_client(self, conn, addr):
        client_ip = addr[0]

        print(f"{client_ip} connected")

        bytes_count = 0

        with open(self.__benchmark_file_path, "wb") as benchmark_file:
            while self.__get_running() and conn:
                chunk = conn.recv(self.__chunk_size)

                if len(chunk) == 0:
                    print(f"{client_ip} sent {bytes_count} bytes")
                    conn.close()

                    category = 'tcp_server' if self.__ack else 'tcp_stream_server'
                    BenchmarkData.add_data(client_ip, category, time(), bytes_count)

                    return

                benchmark_file.write(chunk)

                bytes_count += len(chunk)

                if self.__ack:
                    self.__acknowledge(conn)

    def __acknowledge(self, conn):
        try:
            ack_bytes = bytearray()
            ack_bytes.append(1)
            conn.send(ack_bytes)
        except:
            pass

    def __set_running(self, running):
        self.__running_lock.acquire()
        self.__running = running
        self.__running_lock.release()

    def __get_running(self):
        self.__running_lock.acquire()
        running = self.__running
        self.__running_lock.release()
        return running