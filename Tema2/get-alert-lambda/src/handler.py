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

def get_alert(dynamodb_client, dynamodb_data_table, session_id):
    response = dynamodb_client.get_item(
        Key={
            'sessionId': {
                'S': session_id
            }
        },
        TableName=dynamodb_data_table
    )

    if 'alert' not in response['Item']:
        return False
    return response['Item']['alert']['BOOL'], response['Item']['email']['S']


def lambda_handler(event, context):
    body = json.loads(event['body'])

    dynamodb_data_table = read_config(CONFIG_FILE)

    session_id = body['sessionId']

    dynamodb_client = boto3.client('dynamodb')

    if not get_session(dynamodb_client, dynamodb_data_table, session_id):
        raise f'Session {session_id} does not exist'

    alert, email = get_alert(dynamodb_client, dynamodb_data_table, session_id)

    return {
        'statusCode': 200,
        'body': json.dumps({
            'alert': alert,
            'email': email
        })
    }