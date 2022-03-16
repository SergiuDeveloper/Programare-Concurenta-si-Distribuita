import cv2
import socket
import numpy as np
import requests

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

        publish_server_address, publish_server_port, publish_timeout, write_buffer_size, session_id, get_session_endpoint, register_camera_endpoint, camera_id = config_parser['DEFAULT']['PublishServerAddress'], int(config_parser['DEFAULT']['PublishServerPort']), int(config_parser['DEFAULT']['PublishTimeout']), int(config_parser['DEFAULT']['WriteBufferSize']), config_parser['DEFAULT']['SessionId'], config_parser['DEFAULT']['GetSessionEndpoint'], config_parser['DEFAULT']['RegisterCameraEndpoint'], (config_parser['DEFAULT']['CameraId'] if 'CameraId' in config_parser['DEFAULT'] else None)

    return publish_server_address, publish_server_port, publish_timeout, write_buffer_size, session_id, get_session_endpoint, register_camera_endpoint, camera_id

def session_exists(get_session_endpoint, session_id):
    body = {
        'sessionId': session_id
    }
    response = requests.get(get_session_endpoint, json=body).json()

    session_exists = response['sessionExists']

    return session_exists

def publish_data(publish_server_address, publish_server_port, feed_data_function, publish_timeout, write_buffer_size, session_id, camera_id):
    while True:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        print(f'Contacting {publish_server_address}:{publish_server_port}')
        client_socket.connect((publish_server_address, publish_server_port))

        client_socket.sendall(session_id.encode())
        client_socket.sendall(camera_id.encode())

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

def register_camera(session_id, register_camera_endpoint, config_file_path):
    body = {
        'sessionId': session_id
    }
    response = requests.post(register_camera_endpoint, json=body).json()

    camera_id = response['cameraId']

    with open(config_file_path) as config_file:
        config_parser = ConfigParser()
        config_parser.read_file(config_file)

    with open(config_file_path, 'w') as config_file:
        config_parser['DEFAULT']['CameraId'] = camera_id
        config_parser.write(config_file)

    return camera_id


if __name__ == '__main__':
    publish_server_address, publish_server_port, publish_timeout, write_buffer_size, session_id, get_session_endpoint, register_camera_endpoint, camera_id = read_config(CONFIG_FILE)

    if not session_exists(get_session_endpoint, session_id):
        raise f'Session {session_id} does not exist'

    if camera_id is None:
        register_camera(session_id, register_camera_endpoint, CONFIG_FILE)

    read_test_data_thread = Thread(target=read_test_data, args=(TEST_DATA_FILE,))
    read_test_data_thread.start()

    client_socket = publish_data(publish_server_address, publish_server_port, feed_test_data_function, publish_timeout, write_buffer_size, session_id, camera_id)
