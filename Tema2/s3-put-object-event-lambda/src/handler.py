import urllib.parse
import boto3

from configparser import ConfigParser


CONFIG_FILE = 'config.ini'


def read_config(config_file_path):
    with open(config_file_path) as config_file:
        config_parser = ConfigParser()
        config_parser.read_file(config_file)

        min_confidence = float(config_parser['DEFAULT']['MinConfidence'])

    return min_confidence

def get_identifiers(event):
    bucket = event['Records'][0]['s3']['bucket']['name']
    
    object_key = urllib.parse.unquote_plus(event['Records'][0]['s3']['object']['key'], encoding='utf-8')
    object_key_parts = object_key.split('/')
    session_id = object_key_parts[0]
    camera_id = object_key_parts[1]

    return bucket, session_id, camera_id

def get_all_frames(s3_client, bucket, session_id, camera_id):
    s3_objects = s3_client.list_objects_v2(Bucket=bucket, StartAfter=f'{session_id}/{camera_id}')
    frame_paths = [frame_object['Key'] for frame_object in s3_objects['Contents']]

    return frame_paths

def detect_labels(rekognition_client, bucket, frame_path, min_confidence):
    labels = rekognition_client.detect_labels(
        Image={
            'S3Object': {
                'Bucket': bucket,
                'Name': frame_path
            }
        },
        MinConfidence=min_confidence
    )['Labels']
    labels = [label['Name'] for label in labels]

    return labels


def lambda_handler(event, context):
    min_confidence = read_config(CONFIG_FILE)

    bucket, session_id, camera_id = get_identifiers(event)
    
    s3_client = boto3.client('s3')
    rekognition_client = boto3.client('rekognition')

    frame_paths = get_all_frames(s3_client, bucket, session_id, camera_id)
    frame_paths.sort(reverse=True)
    if len(frame_paths) < 2:
        return

    current_labels = set(detect_labels(rekognition_client, bucket, frame_paths[0], min_confidence))
    previous_labels = set(detect_labels(rekognition_client, bucket, frame_paths[1], min_confidence))
    new_labels = list(current_labels.difference(previous_labels))
    if len(new_labels) == 0:
        return