from flask import Flask, request, jsonify
from threading import Thread
from time import sleep
from configparser import ConfigParser

from logic.TCPServer import TCPServer
from logic.UDPServer import UDPServer
from logic.BenchmarkData import BenchmarkData


CONFIG_FILE_PATH = "../configuration/config.ini"

config_parser = ConfigParser()
config_parser.read(CONFIG_FILE_PATH)

HOST = config_parser['default']['HOST']
PORT = int(config_parser['default']['PORT'])

TCP_SERVER_PORT = int(config_parser['default']['TCP_SERVER_PORT'])
TCP_STREAM_SERVER_PORT = int(config_parser['default']['TCP_STREAM_SERVER_PORT'])
UDP_SERVER_PORT = int(config_parser['default']['UDP_SERVER_PORT'])
UDP_STREAM_SERVER_PORT = int(config_parser['default']['UDP_STREAM_SERVER_PORT'])

BENCHMARK_FILE_PATH = config_parser['default']['BENCHMARK_FILE_PATH']
CHUNK_SIZE = int(config_parser['default']['CHUNK_SIZE'])

CATEGORIES = ['tcp_server', 'tcp_stream_server', 'udp_server', 'udp_stream_server']


app = Flask(__name__)


@app.get("/addresses")
def get_addresses():
    return jsonify({
        'tcp_server': {
            'host': HOST,
            'port': TCP_SERVER_PORT
        },
        'tcp_stream_server': {
            'host': HOST,
            'port': TCP_STREAM_SERVER_PORT
        },
        'udp_server': {
            'host': HOST,
            'port': UDP_SERVER_PORT
        },
        'udp_stream_server': {
            'host': HOST,
            'port': UDP_STREAM_SERVER_PORT
        }
    })

@app.get("/benchmark_data")
def get_benchmark_data():
    sleep(0.1)

    client_ip = request.remote_addr
    response = {}

    for category in CATEGORIES:
        data = BenchmarkData.get_data(client_ip, category)
        response[category] = {
            'timestamp': data[0],
            'bytes_count': data[1]
        }

    BenchmarkData.remove_data(client_ip)

    return jsonify(response)


def launch_tcp_server():
    tcp_server = TCPServer(HOST, TCP_SERVER_PORT, BENCHMARK_FILE_PATH, CHUNK_SIZE, True)
    tcp_server.run()

def launch_tcp_stream_server():
    tcp_stream_server = TCPServer(HOST, TCP_STREAM_SERVER_PORT, BENCHMARK_FILE_PATH, CHUNK_SIZE, False)
    tcp_stream_server.run()

def launch_udp_server():
    udp_server = UDPServer(HOST, UDP_SERVER_PORT, BENCHMARK_FILE_PATH, CHUNK_SIZE, True)
    udp_server.run()

def launch_udp_stream_server():
    udp_stream_server = UDPServer(HOST, UDP_STREAM_SERVER_PORT, BENCHMARK_FILE_PATH, CHUNK_SIZE, False)
    udp_stream_server.run()


if __name__ == "__main__":
    launch_tcp_server_thread = Thread(target=launch_tcp_server)
    launch_tcp_server_thread.start()

    launch_tcp_stream_server_thread = Thread(target=launch_tcp_stream_server)
    launch_tcp_stream_server_thread.start()

    launch_udp_server_thread = Thread(target=launch_udp_server)
    launch_udp_server_thread.start()

    launch_udp_stream_server_thread = Thread(target=launch_udp_stream_server)
    launch_udp_stream_server_thread.start()

    app.run(port=PORT)