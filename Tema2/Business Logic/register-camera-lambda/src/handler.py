import json
import boto3

from botocore.exceptions import ClientError
from configparser import ConfigParser
from random import choice
from string import ascii_uppercase


CONFIG_FILE = 'config.ini'


def read_config(config_file_path):
    with open(config_file_path) as config_file:
        config_parser = ConfigParser()
        config_parser.read_file(config_file)

        camera_id_length, dynamodb_data_table = int(config_parser['DEFAULT']['CameraIdLength']), config_parser['DEFAULT']['DynamoDbDataTable']

    return camera_id_length, dynamodb_data_table

def generate_camera_id(camera_id_length):
    camera_id = ''.join(choice(ascii_uppercase) for _ in range(camera_id_length))

    return camera_id

def get_session(dynamodb_client, dynamodb_data_table, session_id):
    query_results = dynamodb_client.query(
        TableName=dynamodb_data_table,
        KeyConditionExpression='sessionId = :SESSION_ID',
        ExpressionAttributeValues={':SESSION_ID': {'S': session_id}}
    )

    return len(query_results['Items']) > 0

def insert_camera_id(dynamodb_client, dynamodb_data_table, session_id, camera_id):
    try:
        response = dynamodb_client.update_item(
            Key={
                'sessionId': {
                    'S': session_id
                }
            },
            UpdateExpression='SET cameraIds = list_append(cameraIds, :cameraId)',
            ExpressionAttributeValues={
                ':cameraId': {'L': [{'S': camera_id}]},
            },
            ReturnValues='UPDATED_NEW',
            TableName=dynamodb_data_table
        )
    except ClientError as e:
        if e.response['Error']['Code'] == 'ConditionalCheckFailedException':
            return False
        else:
            raise e
    except Exception as e:
        raise e

    return True


def lambda_handler(event, context):
    body = json.loads(event['body'])

    camera_id_length, dynamodb_data_table = read_config(CONFIG_FILE)

    session_id = body['sessionId']

    dynamodb_client = boto3.client('dynamodb')

    if not get_session(dynamodb_client, dynamodb_data_table, session_id):
        raise f'Session {session_id} does not exist'

    successfully_inserted = False
    while not successfully_inserted:
        camera_id = generate_camera_id(camera_id_length)
        successfully_inserted = insert_camera_id(dynamodb_client, dynamodb_data_table, session_id, camera_id)

    return {
        'statusCode': 200,
        'body': json.dumps({
            'cameraId': camera_id
        })
    }