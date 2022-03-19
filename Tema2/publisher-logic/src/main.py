import socket
import boto3
import numpy as np
import io

from threading import Thread
from configparser import ConfigParser
from time import time


CONFIG_FILE = 'config.ini'


def read_config(config_file_path):
    with open(config_file_path) as config_file:
        config_parser = ConfigParser()
        config_parser.read_file(config_file)

        publish_server_address, publish_server_port, read_buffer_size, aws_region, session_id_length, camera_id_length, frames_s3_bucket_name = config_parser['DEFAULT']['PublishServerAddress'], int(config_parser['DEFAULT']['PublishServerPort']), int(config_parser['DEFAULT']['ReadBufferSize']), config_parser['DEFAULT']['AwsRegion'], int(config_parser['DEFAULT']['SessionIdLength']), int(config_parser['DEFAULT']['CameraIdLength']), config_parser['DEFAULT']['FramesS3BucketName']

    return publish_server_address, publish_server_port, read_buffer_size, aws_region, session_id_length, camera_id_length, frames_s3_bucket_name

def get_publish_data(publish_server_address, publish_server_port, read_buffer_size, s3_client, session_id_length, camera_id_length, frames_s3_bucket_name):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((publish_server_address, publish_server_port))
    server_socket.listen()

    print(f'Listening on {publish_server_address}:{publish_server_port}')

    while True:
        connection, _ = server_socket.accept()

        handle_client_thread = Thread(target=handle_client, args=(connection, read_buffer_size, s3_client, session_id_length, camera_id_length, frames_s3_bucket_name,))
        handle_client_thread.start()

def handle_client(connection, read_buffer_size, s3_client, session_id_length, camera_id_length, frames_s3_bucket_name):
    frame = b''

    while True:
        data = connection.recv(read_buffer_size)
        if not data:
            connection.close()
            break

        frame += data

    session_id = frame[:session_id_length].decode()
    camera_id = frame[session_id_length:session_id_length + camera_id_length].decode()
    frame = frame[session_id_length + camera_id_length:]

    print(f'Received {len(frame)} bytes')

    frame = np.frombuffer(frame, dtype='uint8')
    frame_bytes = io.BytesIO(frame).getvalue()

    timestamp = time()
    s3_client.put_object(Body=frame_bytes, Bucket=frames_s3_bucket_name, Key=f'{session_id}/{camera_id}/{timestamp}.jpg')


if __name__ == '__main__':
    publish_server_address, publish_server_port, read_buffer_size, aws_region, session_id_length, camera_id_length, frames_s3_bucket_name = read_config(CONFIG_FILE)

    s3_client = boto3.client('s3')
    
    get_publish_data(publish_server_address, publish_server_port, read_buffer_size, s3_client, session_id_length, camera_id_length, frames_s3_bucket_name)