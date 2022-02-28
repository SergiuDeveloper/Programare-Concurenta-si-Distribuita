HOST = "127.0.0.1"
PORT = 8000

TCP_SERVER_PORT = 8001
TCP_STREAM_SERVER_PORT = 8002

BENCHMARK_FILE_PATH = "../data/test.mp4"
CHUNK_SIZE = 65535

CATEGORIES = ['tcp_server', 'tcp_stream_server']


from flask import Flask, request, jsonify
from threading import Thread
from time import sleep

from logic.TCPServer import TCPServer
from logic.BenchmarkData import BenchmarkData


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

    return jsonify(response)


def launch_tcp_server():
    tcp_server = TCPServer(HOST, TCP_SERVER_PORT, BENCHMARK_FILE_PATH, CHUNK_SIZE, True)
    tcp_server.run()

def launch_tcp_stream_server():
    tcp_stream_server = TCPServer(HOST, TCP_STREAM_SERVER_PORT, BENCHMARK_FILE_PATH, CHUNK_SIZE, False)
    tcp_stream_server.run()


if __name__ == "__main__":
    launch_tcp_server_thread = Thread(target=launch_tcp_server)
    launch_tcp_server_thread.start()

    launch_tcp_stream_server_thread = Thread(target=launch_tcp_stream_server)
    launch_tcp_stream_server_thread.start()

    app.run(port=PORT)