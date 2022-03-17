import urllib.parse
import boto3
import base64
import os.path
import email

from configparser import ConfigParser
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email.mime.application import MIMEApplication


CONFIG_FILE = 'config.ini'


def read_config(config_file_path):
    with open(config_file_path) as config_file:
        config_parser = ConfigParser()
        config_parser.read_file(config_file)

        dynamodb_data_table, min_confidence, email_sender = config_parser['DEFAULT']['DynamoDbDataTable'], float(config_parser['DEFAULT']['MinConfidence']), config_parser['DEFAULT']['EmailSender']

    return dynamodb_data_table, min_confidence, email_sender

def get_identifiers(event):
    bucket = event['Records'][0]['s3']['bucket']['name']
    
    object_key = urllib.parse.unquote_plus(event['Records'][0]['s3']['object']['key'], encoding='utf-8')
    object_key_parts = object_key.split('/')
    session_id = object_key_parts[0]
    camera_id = object_key_parts[1]

    return bucket, session_id, camera_id

def get_all_frames(s3_client, bucket, session_id, camera_id):
    s3_objects = s3_client.list_objects(Bucket=bucket, Prefix=f'{session_id}/{camera_id}/', Delimiter='/')
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

def send_email(ses_client, s3_client, sender, recipient, session_id, camera_id, labels, bucket, frame_path):
    attachment = f'/tmp/{session_id}_{camera_id}.jpg'

    s3_client.download_file(bucket, frame_path, attachment)

    subject = f'[Alarm triggered for session {session_id} camera {camera_id}'

    body_text = f'''
        Alarm triggered for session {session_id} camera {camera_id}\r\n
        Detected labels: {labels}\r\n
    '''

    body_html = f'''
        <html>
        <body>
            <div>
                Alarm triggered for session {session_id} camera {camera_id}<br>
                Detected labels: {labels}
            </div>
        </body>
        </html>
    '''

    charset = 'utf-8'

    message = MIMEMultipart('mixed')
    message['Subject'] = subject 
    message['From'] = sender 
    message['To'] = recipient

    body = MIMEMultipart('alternative')
    textpart = MIMEText(body_text.encode(charset), 'plain', charset)
    htmlpart = MIMEText(body_html.encode(charset), 'html', charset)

    body.attach(textpart)
    body.attach(htmlpart)

    attachmentPart = MIMEApplication(open(attachment, 'rb').read())
    attachmentPart.add_header('Content-Disposition','attachment', filename=os.path.basename(attachment))
    message.attach(body)
    message.attach(attachmentPart)

    ses_client.send_raw_email(
        Source=sender,
        Destinations=[
            recipient
        ],
        RawMessage={
            'Data':message.as_string(),
        }
    )

    print(f'Sent alert to {recipient}, session {session_id}, camera {camera_id}, labels {labels}')


def lambda_handler(event, context):
    dynamodb_data_table, min_confidence, email_sender = read_config(CONFIG_FILE)

    bucket, session_id, camera_id = get_identifiers(event)
    
    s3_client = boto3.client('s3')
    rekognition_client = boto3.client('rekognition')
    dynamodb_client = boto3.client('dynamodb')
    ses_client = boto3.client('ses')

    if not get_session(dynamodb_client, dynamodb_data_table, session_id):
        return

    alert, email = get_alert(dynamodb_client, dynamodb_data_table, session_id)
    if not alert:
        return

    frame_paths = get_all_frames(s3_client, bucket, session_id, camera_id)
    frame_paths.sort(reverse=True)
    if len(frame_paths) < 2:
        return

    current_labels = set(detect_labels(rekognition_client, bucket, frame_paths[0], min_confidence))
    previous_labels = set(detect_labels(rekognition_client, bucket, frame_paths[1], min_confidence))
    new_labels = list(current_labels.difference(previous_labels))
    if len(new_labels) == 0:
        return

    send_email(ses_client, s3_client, email_sender, email, session_id, camera_id, new_labels, bucket, frame_paths[0])