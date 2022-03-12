import json
import boto3

from configparser import ConfigParser
from random import choice
from string import ascii_uppercase


CONFIG_FILE = 'config.ini'


def read_config(config_file_path):
    with open(config_file_path) as config_file:
        config_parser = ConfigParser()
        config_parser.read_file(config_file)

        session_id_length, dynamodb_data_table = int(config_parser['DEFAULT']['SessionIdLength']), config_parser['DEFAULT']['DynamoDbDataTable']

    return session_id_length, dynamodb_data_table

def generate_session_id(session_id_length):
    session_id = ''.join(choice(ascii_uppercase) for _ in range(session_id_length))

    return session_id

def insert_session_id(dynamodb_client, dynamodb_data_table, session_id):
    try:
        dynamodb_client.put_item(
            Item={
                'sessionId': {
                    'S': session_id
                },
                'cameraId': {
                    'S': '0'
                }
            },
            ConditionExpression='attribute_not_exists(sessionId)',
            TableName=dynamodb_data_table
        )
    except Exception as boto_exception:
        print(session_id)
        print(boto_exception)
        return False

    return True


def lambda_handler(event, context):
    session_id_length, dynamodb_data_table = read_config(CONFIG_FILE)

    dynamodb_client = boto3.client('dynamodb')

    successfully_inserted = False
    while not successfully_inserted:
        session_id = generate_session_id(session_id_length)
        successfully_inserted = insert_session_id(dynamodb_client, dynamodb_data_table, session_id)

    return {
        'statusCode': 200,
        'body': json.dumps({
            'sessionId': session_id
        })
    }
