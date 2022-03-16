import asyncio
import websockets
import json
import boto3

from configparser import ConfigParser


CONFIG_FILE = 'config.ini'


frames_s3_bucket_name = None
s3_client = None


@asyncio.coroutine
def handle_client(websocket):
    global frames_s3_bucket_name
    global s3_client

    request = yield from websocket.recv()
    request = json.loads(request)

    session_id = request['sessionId']
    camera_id = request['cameraId']

    frame_paths = get_all_frames(s3_client, frames_s3_bucket_name, session_id, camera_id)
    frame_paths.sort(reverse=True)
    if len(frame_paths) == 0:
        return

    frame_path = frame_paths[0]
    data = s3.get_object(Bucket=bucket, Key=frame_path)
    frame_content = data['Body'].read()
    frame_content = frame_content.decode()

    websocket.send(frame_content)


def read_config(config_file_path):
    with open(config_file_path) as config_file:
        config_parser = ConfigParser()
        config_parser.read_file(config_file)

        frame_serve_address, frame_serve_port, frames_s3_bucket_name = config_parser['DEFAULT']['FrameServeAddress'], int(config_parser['DEFAULT']['FrameServePort']), config_parser['DEFAULT']['FramesS3BucketName']

    return frame_serve_address, frame_serve_port, frames_s3_bucket_name

def get_all_frames(s3_client, bucket, session_id, camera_id):
    s3_objects = s3_client.list_objects_v2(Bucket=bucket, StartAfter=f'{session_id}/{camera_id}')
    frame_paths = [frame_object['Key'] for frame_object in s3_objects['Contents']]

    return frame_paths


if __name__ == '__main__':
    frame_serve_address, frame_serve_port, frames_s3_bucket_name = read_config(CONFIG_FILE)
    s3_client = boto3.client('s3')

    start_server = websockets.serve(handle_client, frame_serve_address, frame_serve_port)

    asyncio.get_event_loop().run_until_complete(start_server)
    asyncio.get_event_loop().run_forever()