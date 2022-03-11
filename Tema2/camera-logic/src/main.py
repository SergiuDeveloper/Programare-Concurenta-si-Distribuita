import cv2
import socket
import numpy as np

from configparser import ConfigParser
from threading import Thread, Lock
from copy import deepcopy
from time import sleep


CONFIG_FILE = '../configuration/config.ini'
TEST_DATA_FILE = '../test-data/test.mp4'


current_frame = None
current_frame_lock = Lock()


def read_config(config_file_path):
    with open(config_file_path) as config_file:
        config_parser = ConfigParser()
        config_parser.read_file(config_file)

        publish_server_address, publish_server_port, publish_timeout, write_buffer_size = config_parser['DEFAULT']['PublishServerAddress'], int(config_parser['DEFAULT']['PublishServerPort']), int(config_parser['DEFAULT']['PublishTimeout']), int(config_parser['DEFAULT']['WriteBufferSize'])

    return publish_server_address, publish_server_port, publish_timeout, write_buffer_size

def publish_data(publish_server_address, publish_server_port, feed_data_function, publish_timeout, write_buffer_size):
    while True:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        print(f'Contacting {publish_server_address}:{publish_server_port}')
        client_socket.connect((publish_server_address, publish_server_port))

        image_bytes = feed_data_function()
        image_bytes_str = image_bytes.tobytes()
        for image_bytes_str_chunk in get_chunks(image_bytes_str, write_buffer_size):
            client_socket.sendall(image_bytes_str_chunk)
        print(f'Sent {len(image_bytes)} bytes to {publish_server_address}:{publish_server_port}')
        print()

        client_socket.close()

        sleep(publish_timeout)

def get_chunks(list, chunk_size):
    for i in range(0, len(list), chunk_size):
        yield list[i:i+chunk_size]

def get_current_frame():
    global current_frame
    global current_frame_lock

    current_frame_lock.acquire()
    value = deepcopy(current_frame)
    current_frame_lock.release()

    return value

def set_current_frame(value):
    global current_frame
    global current_frame_lock

    current_frame_lock.acquire()
    current_frame = value
    current_frame_lock.release()

def read_test_data(test_data_file):
    cap = cv2.VideoCapture(test_data_file)

    while cap.isOpened():
        success, frame = cap.read()

        if not success:
            cap = cv2.VideoCapture(test_data_file)
            success, frame = cap.read()
            if not success:
                raise 'Video read exception'

        encode_param = [int(cv2.IMWRITE_JPEG_QUALITY), 90]
        _, image_bytes = cv2.imencode('.jpg', frame, encode_param)
        image_bytes = np.array(image_bytes)
        
        set_current_frame(image_bytes)

def feed_test_data_function():
    current_frame_value = get_current_frame()
    while current_frame_value is None:
        current_frame_value = get_current_frame()

    return current_frame_value


if __name__ == '__main__':
    publish_server_address, publish_server_port, publish_timeout, write_buffer_size = read_config(CONFIG_FILE)

    read_test_data_thread = Thread(target=read_test_data, args=(TEST_DATA_FILE,))
    read_test_data_thread.start()

    client_socket = publish_data(publish_server_address, publish_server_port, feed_test_data_function, publish_timeout, write_buffer_size)
