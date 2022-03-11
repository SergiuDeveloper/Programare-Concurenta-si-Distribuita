import socket
import boto3
import numpy as np
import io

from threading import Thread
from configparser import ConfigParser


CONFIG_FILE = '../configuration/config.ini'


def read_config(config_file_path):
    with open(config_file_path) as config_file:
        config_parser = ConfigParser()
        config_parser.read_file(config_file)

        publish_server_address, publish_server_port, read_buffer_size, aws_region, object_detection_confidence_threshold = config_parser['DEFAULT']['PublishServerAddress'], int(config_parser['DEFAULT']['PublishServerPort']), int(config_parser['DEFAULT']['ReadBufferSize']), config_parser['DEFAULT']['AwsRegion'], float(config_parser['DEFAULT']['ObjectDetectionConfidenceThreshold'])

    return publish_server_address, publish_server_port, read_buffer_size, aws_region, object_detection_confidence_threshold

def get_publish_data(publish_server_address, publish_server_port, read_buffer_size, rekognition_client, object_detection_confidence_threshold):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((publish_server_address, publish_server_port))
    server_socket.listen()

    print(f'Listening on {publish_server_address}:{publish_server_port}')

    while True:
        connection, _ = server_socket.accept()

        handle_client_thread = Thread(target=handle_client, args=(connection, read_buffer_size, rekognition_client, object_detection_confidence_threshold,))
        handle_client_thread.start()

def handle_client(connection, read_buffer_size, rekognition_client, object_detection_confidence_threshold):
    frame = b''

    while True:
        data = connection.recv(read_buffer_size)
        if not data:
            connection.close()
            break

        frame += data

    print(f'Received {len(frame)} bytes')

    frame = np.frombuffer(frame, dtype='uint8')
    frame_bytes = io.BytesIO(frame).getvalue()
    
    labels = rekognition_client.detect_labels(Image={'Bytes': frame_bytes})['Labels']
    labels = [label['Name'] for label in labels if label['Confidence'] >= object_detection_confidence_threshold]
    print(labels)


if __name__ == '__main__':
    publish_server_address, publish_server_port, read_buffer_size, aws_region, object_detection_confidence_threshold = read_config(CONFIG_FILE)

    rekognition_client = boto3.client('rekognition', region_name=aws_region)
    
    get_publish_data(publish_server_address, publish_server_port, read_buffer_size, rekognition_client, object_detection_confidence_threshold)