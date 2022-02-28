import socket

from threading import Thread, Lock
from time import time

from .BenchmarkData import BenchmarkData


class UDPServer:
    def __init__(self, host, port, benchmark_file_path, chunk_size, ack):
        self.__host = host
        self.__port = port
        self.__running = False
        self.__running_lock = Lock()
        self.__benchmark_file_path = benchmark_file_path
        self.__chunk_size = chunk_size
        self.__ack = ack

    def run(self):
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
            sock.bind((self.__host, self.__port))

            print(f"Waiting for clients on {self.__host}:{self.__port}")

            self.__set_running(True)

            while self.__get_running():
                data = sock.recvfrom(self.__chunk_size)
                chunk = data[0]
                address = data[1]
                client_ip = address[0]

                handle_client_thread = Thread(target=self.__handle_client, args=(sock, chunk, address, client_ip))
                handle_client_thread.start()

    def stop(self):
        self.__set_running(False)

    def __handle_client(self, sock, chunk, address, client_ip):
        category = 'udp_server' if self.__ack else 'udp_stream_server'
        file_name_parts = self.__benchmark_file_path.split('.')
        if len(file_name_parts) == 1:
            file_name = f"{self.__benchmark_file_path}_{category}"
        else:
            file_name = f"{'.'.join(file_name_parts[:-1])}_{category}.{file_name_parts[-1]}"

        self.__prepare_new_request(client_ip, file_name, category)

        with open(file_name, "ab") as benchmark_file:
            benchmark_file.write(chunk)

            data = BenchmarkData.get_data(client_ip, category)
            total_bytes = data[1]
            chunks_count = data[2]
            BenchmarkData.add_data(client_ip, category, time(), total_bytes + len(chunk), chunks_count + 1)

            if self.__ack:
                self.__acknowledge(sock, address)

    def __prepare_new_request(self, client_ip, file_name, category):
        if BenchmarkData.get_data(client_ip, category) is not None:
            return
        
        with open(file_name, "wb") as benchmark_file:
            pass
        BenchmarkData.add_data(client_ip, category, time(), 0, 0)

    def __acknowledge(self, sock, address):
        try:
            ack_bytes = bytearray()
            ack_bytes.append(1)
            sock.sendto(ack_bytes, address)
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