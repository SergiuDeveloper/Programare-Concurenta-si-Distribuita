import boto3
import json

from configparser import ConfigParser


CONFIG_FILE = 'config.ini'


def read_config(config_file_path):
    with open(config_file_path) as config_file:
        config_parser = ConfigParser()
        config_parser.read_file(config_file)

        dynamodb_data_table = config_parser['DEFAULT']['DynamoDbDataTable']

    return dynamodb_data_table

def get_session(dynamodb_client, dynamodb_data_table, session_id):
    query_results = dynamodb_client.query(
        TableName=dynamodb_data_table,
        KeyConditionExpression='sessionId = :SESSION_ID',
        ExpressionAttributeValues={':SESSION_ID': {'S': session_id}}
    )

    return len(query_results['Items']) > 0

def get_cameras(dynamodb_client, dynamodb_data_table, session_id):
    response = dynamodb_client.get_item(
        Key={
            'sessionId': {
                'S': session_id
            }
        },
        TableName=dynamodb_data_table
    )

    camera_ids = response['Item']['cameraIds']['L']
    camera_ids = [camera_id['S'] for camera_id in camera_ids]
    return camera_ids


def lambda_handler(event, context):
    body = json.loads(event['body'])

    dynamodb_data_table = read_config(CONFIG_FILE)

    session_id = body['sessionId']

    dynamodb_client = boto3.client('dynamodb')

    if not get_session(dynamodb_client, dynamodb_data_table, session_id):
        raise f'Session {session_id} does not exist'

    camera_ids = get_cameras(dynamodb_client, dynamodb_data_table, session_id)

    return {
        'statusCode': 200,
        'body': json.dumps({
            'cameraIds': camera_ids
        })
    }