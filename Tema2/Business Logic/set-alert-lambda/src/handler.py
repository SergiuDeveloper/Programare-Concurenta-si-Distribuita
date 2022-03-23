import boto3
import json

from botocore.exceptions import ClientError
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

def set_alert(dynamodb_client, dynamodb_data_table, session_id, alert, email):
    try:
        response = dynamodb_client.update_item(
            Key={
                'sessionId': {
                    'S': session_id
                }
            },
            UpdateExpression='SET alert = :alert, email = :email',
            ExpressionAttributeValues={
                ':alert': {'BOOL': alert},
                ':email': {'S': email}
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


def lambda_handler(event, context):
    body = json.loads(event['body'])

    dynamodb_data_table = read_config(CONFIG_FILE)

    session_id = body['sessionId']
    alert = body['alert']
    email = body['email']

    dynamodb_client = boto3.client('dynamodb')

    if not get_session(dynamodb_client, dynamodb_data_table, session_id):
        raise f'Session {session_id} does not exist'

    alert = set_alert(dynamodb_client, dynamodb_data_table, session_id, alert, email)

    return {
        'statusCode': 200
    }