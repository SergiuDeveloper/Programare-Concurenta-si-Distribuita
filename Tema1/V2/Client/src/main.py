HOST = "127.0.0.1"
PORT = 8000

ADDRESSES_PATH = 'addresses'
BENCHMARK_DATA_PATH = 'benchmark_data'

BENCHMARK_FILE_PATH = "../data/test.mp4"
CHUNK_SIZE = 65535


import requests

from time import sleep

from logic.TCPClient import TCPClient


if __name__ == "__main__":
    addresses = requests.get(f"http://{HOST}:{PORT}/{ADDRESSES_PATH}").json()
    
    tcp_server_host = addresses['tcp_server']['host']
    tcp_server_port = addresses['tcp_server']['port']
    tcp_client = TCPClient(tcp_server_host, tcp_server_port, BENCHMARK_FILE_PATH, CHUNK_SIZE, True)
    tcp_start_timestamp, tcp_expected_bytes_count = tcp_client.run()

    print('Performed TCP stop-and-wait benchmark')
    sleep(1)

    tcp_stream_server_host = addresses['tcp_stream_server']['host']
    tcp_stream_server_port = addresses['tcp_stream_server']['port']
    tcp_stream_client = TCPClient(tcp_stream_server_host, tcp_stream_server_port, BENCHMARK_FILE_PATH, CHUNK_SIZE, False)
    tcp_stream_start_timestamp, tcp_stream_expected_bytes_count = tcp_stream_client.run()

    print('Performed TCP stream benchmark')
    sleep(1)

    benchmark_data = requests.get(f"http://{HOST}:{PORT}/{BENCHMARK_DATA_PATH}").json()
    
    tcp_end_timestamp = benchmark_data['tcp_server']['timestamp']
    tcp_received_bytes_count = benchmark_data['tcp_server']['bytes_count']

    tcp_stream_end_timestamp = benchmark_data['tcp_stream_server']['timestamp']
    tcp_stream_received_bytes_count = benchmark_data['tcp_stream_server']['bytes_count']

    print()
    print('TCP stop-and-wait:')
    print('Duration:', tcp_end_timestamp - tcp_start_timestamp, 'seconds')
    print('Bytes sent:', tcp_expected_bytes_count, 'received:', tcp_received_bytes_count, 'success rate ', '{:.2%}'.format(tcp_received_bytes_count / tcp_expected_bytes_count))
    print()
    print('TCP stream:')
    print('Duration:', tcp_stream_end_timestamp - tcp_stream_start_timestamp, 'seconds')
    print('Bytes sent:', tcp_stream_expected_bytes_count, 'received:', tcp_stream_received_bytes_count, 'success rate ', '{:.2%}'.format(tcp_stream_received_bytes_count / tcp_stream_expected_bytes_count))
    print()