import requests

from time import sleep
from configparser import ConfigParser

from logic.TCPClient import TCPClient
from logic.UDPClient import UDPClient


CONFIG_FILE_PATH = "../configuration/config.ini"

config_parser = ConfigParser()
config_parser.read(CONFIG_FILE_PATH)


HOST = config_parser['default']['HOST']
PORT = int(config_parser['default']['PORT'])

ADDRESSES_PATH = config_parser['default']['ADDRESSES_PATH']
BENCHMARK_DATA_PATH = config_parser['default']['BENCHMARK_DATA_PATH']

BENCHMARK_FILE_PATH = config_parser['default']['BENCHMARK_FILE_PATH']
CHUNK_SIZE = int(config_parser['default']['CHUNK_SIZE'])


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

    udp_server_host = addresses['udp_server']['host']
    udp_server_port = addresses['udp_server']['port']
    udp_client = UDPClient(udp_server_host, udp_server_port, BENCHMARK_FILE_PATH, CHUNK_SIZE, True)
    udp_start_timestamp, udp_expected_bytes_count = udp_client.run()

    print('Performed UDP stop-and-wait benchmark')
    sleep(1)

    udp_stream_server_host = addresses['udp_stream_server']['host']
    udp_stream_server_port = addresses['udp_stream_server']['port']
    udp_stream_client = UDPClient(udp_stream_server_host, udp_stream_server_port, BENCHMARK_FILE_PATH, CHUNK_SIZE, False)
    udp_stream_start_timestamp, udp_stream_expected_bytes_count = udp_stream_client.run()

    print('Performed UDP stream benchmark')

    benchmark_data = requests.get(f"http://{HOST}:{PORT}/{BENCHMARK_DATA_PATH}").json()
    
    tcp_end_timestamp = benchmark_data['tcp_server']['timestamp']
    tcp_received_bytes_count = benchmark_data['tcp_server']['bytes_count']

    tcp_stream_end_timestamp = benchmark_data['tcp_stream_server']['timestamp']
    tcp_stream_received_bytes_count = benchmark_data['tcp_stream_server']['bytes_count']

    udp_end_timestamp = benchmark_data['udp_server']['timestamp']
    udp_received_bytes_count = benchmark_data['udp_server']['bytes_count']

    udp_stream_end_timestamp = benchmark_data['udp_stream_server']['timestamp']
    udp_stream_received_bytes_count = benchmark_data['udp_stream_server']['bytes_count']

    print()
    print('TCP stop-and-wait:')
    print('Duration:', tcp_end_timestamp - tcp_start_timestamp, 'seconds')
    print('MBytes sent:', tcp_expected_bytes_count / 1024 / 1024, 'received:', tcp_received_bytes_count / 1024 / 1024, 'success rate ', '{:.2%}'.format(tcp_received_bytes_count / tcp_expected_bytes_count))
    print()
    print('TCP stream:')
    print('Duration:', tcp_stream_end_timestamp - tcp_stream_start_timestamp, 'seconds')
    print('MBytes sent:', tcp_stream_expected_bytes_count / 1024 / 1024, 'received:', tcp_stream_received_bytes_count / 1024 / 1024, 'success rate ', '{:.2%}'.format(tcp_stream_received_bytes_count / tcp_stream_expected_bytes_count))
    print()
    print()
    print('UDP stop-and-wait:')
    print('Duration:', udp_end_timestamp - udp_start_timestamp, 'seconds')
    print('MBytes sent:', udp_expected_bytes_count / 1024 / 1024, 'received:', udp_received_bytes_count / 1024 / 1024, 'success rate ', '{:.2%}'.format(udp_received_bytes_count / udp_expected_bytes_count))
    print()
    print('UDP stream:')
    print('Duration:', udp_stream_end_timestamp - udp_stream_start_timestamp, 'seconds')
    print('MBytes sent:', udp_stream_expected_bytes_count / 1024 / 1024, 'received:', udp_stream_received_bytes_count / 1024 / 1024, 'success rate ', '{:.2%}'.format(udp_stream_received_bytes_count / udp_stream_expected_bytes_count))
    print()