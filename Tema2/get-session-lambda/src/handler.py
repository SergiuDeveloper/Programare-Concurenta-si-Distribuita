import json
import boto3

from configparser import ConfigParser
from boto3.dynamodb.conditions import Key


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


def lambda_handler(event, context):
    body = json.loads(event['body'])
    
    dynamodb_data_table = read_config(CONFIG_FILE)

    session_id = body['sessionId']

    dynamodb_client = boto3.client('dynamodb')

    session_exists = get_session(dynamodb_client, dynamodb_data_table, session_id)

    return {
        'statusCode': 200,
        'body': json.dumps({
            'sessionExists': session_exists
        })
    }