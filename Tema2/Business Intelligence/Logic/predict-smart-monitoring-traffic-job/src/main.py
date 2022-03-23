import boto3

from configparser import ConfigParser
from joblib import load
from datetime import datetime, timedelta
from boto3.dynamodb.conditions import Attr


CONFIG_FILE = '../configuration/config.ini'
SESSIONS_PREDICTOR = '../models/sessions_predictor.joblib'
CAMERAS_PREDICTOR = '../models/cameras_predictor.joblib'
TIMESTAMPS_PREDICTOR = '../models/frames_predictor.joblib'


def read_config(config_file_path):
    with open(config_file_path) as config_file:
        config_parser = ConfigParser()
        config_parser.read_file(config_file)

        access_key, secret_key, region, data_table, data_bucket, sessions_table, cameras_table, timestamps_table, window = config_parser['DEFAULT']['AccessKey'], config_parser['DEFAULT']['SecretKey'], config_parser['DEFAULT']['Region'], config_parser['DEFAULT']['DataTable'], config_parser['DEFAULT']['DataBucket'], config_parser['DEFAULT']['SessionsTable'], config_parser['DEFAULT']['CamerasTable'], config_parser['DEFAULT']['TimestampsTable'], int(config_parser['DEFAULT']['Window'])

    return access_key, secret_key, region, data_table, data_bucket, sessions_table, cameras_table, timestamps_table, window

def scan_data_table(dynamodb_client, table_name):
    response = dynamodb_client.scan(
        TableName=table_name,
        Select='ALL_ATTRIBUTES'
    )
    
    items = response['Items']
    normalized_items = []
    for item in items:
        normalized_item = {
            'sessionId': item['sessionId']['S'],
            'cameras': [{'cameraId': camera['S']} for camera in item['cameraIds']['L']],
            'email': item['email']['S'],
            'alert': item['alert']['BOOL']
        }
        normalized_items.append(normalized_item)
    return normalized_items

def scan_data_bucket(s3_client, bucket_name, session_id, camera_id):
    items = s3_client.list_objects(Bucket=bucket_name, Prefix=f'{session_id}/{camera_id}/', Delimiter='/')
    if 'Contents' not in items:
        return 0
    items = [item['Key'] for item in items['Contents'] if len([part for part in item['Key'].split('/') if len(part) > 0]) >= 3]

    return len(items)

def set_today_traffic_data(dynamodb_client, sessions_table, cameras_table, timestamps_table, sessions_count, cameras_count, timestamps_count):
    date = datetime.now().strftime('%Y/%m/%d')

    for table_item in [(sessions_table, sessions_count), (cameras_table, cameras_count), (timestamps_table, timestamps_count)]:
        table = table_item[0]
        traffic = table_item[1]
        dynamodb_client.update_item(
            Key={
                'date': {
                    'S': date
                }
            },
            UpdateExpression='SET traffic = :traffic',
            ExpressionAttributeValues={
                ':traffic': {'N': str(traffic)}
            },
            ReturnValues='UPDATED_NEW',
            TableName=table
        )

def get_previous_traffic_data(dynamodb_client, sessions_table, cameras_table, timestamps_table, window):
    data = {}

    for table_item in [(sessions_table, 'sessions'), (cameras_table, 'cameras'), (timestamps_table, 'timestamps')]:
        table = table_item[0]
        name = table_item[1]

        response = dynamodb_client.scan(
            TableName=table
        )

        items = [(item['date']['S'], float(item['traffic']['N'])) for item in response['Items']]
        items = sorted(items)[:window]
        items = [item[1] for item in items]

        data[name] = items

    return data

def predict_data(sessions_predictor_model, cameras_predictor_model, timestamps_predictor_model, session_data, cameras_data, timestamps_data):
    predictions = {}

    for model_item in [(sessions_predictor_model, session_data, 'sessions'), (cameras_predictor_model, cameras_data, 'cameras'), (timestamps_predictor_model, timestamps_data, 'timestamps')]:
        model = model_item[0]
        data = model_item[1]
        name = model_item[2]

        prediction = model.predict([data])[0]

        predictions[name] = prediction

    return predictions

def insert_predictions(dynamodb_client, sessions_table, cameras_table, timestamps_table, sessions_predictions, cameras_predictions, timestamps_predictions):
    for table_item in [(sessions_table, sessions_predictions), (cameras_table, cameras_predictions), (timestamps_table, timestamps_predictions)]:
        table = table_item[0]
        predictions = table_item[1]

        date = datetime.now()

        for prediction in predictions:
            date = date + timedelta(days=1)

            try:
                dynamodb_client.put_item(
                    TableName=table,
                    Item={
                        'date': {
                            'S': date.strftime('%Y/%m/%d')
                        },
                        'traffic': {
                            'N': str(prediction)
                        }
                    }
                )
            except:
                pass


if __name__ == '__main__':
    access_key, secret_key, region, data_table, data_bucket, sessions_table, cameras_table, timestamps_table, window = read_config(CONFIG_FILE)

    s3_client = boto3.client('s3', aws_access_key_id=access_key, aws_secret_access_key=secret_key, region_name=region)
    dynamodb_client = boto3.client('dynamodb', aws_access_key_id=access_key, aws_secret_access_key=secret_key, region_name=region)

    table_items = scan_data_table(dynamodb_client, data_table)
    sessions_count = 0
    cameras_count = 0
    timestamps_count = 0
    for item in table_items:
        sessions_count += 1
        for camera in item['cameras']:
            cameras_count += 1
            bucket_items_count = scan_data_bucket(s3_client, data_bucket, item['sessionId'], camera['cameraId'])
            camera['framesCount'] = bucket_items_count
            timestamps_count += camera['framesCount']

    set_today_traffic_data(dynamodb_client, sessions_table, cameras_table, timestamps_table, sessions_count, cameras_count, timestamps_count)

    traffic_data = get_previous_traffic_data(dynamodb_client, sessions_table, cameras_table, timestamps_table, window)

    sessions_predictor_model, cameras_predictor_model, timestamps_predictor_model = load(SESSIONS_PREDICTOR), load(CAMERAS_PREDICTOR), load(TIMESTAMPS_PREDICTOR)
    
    predictions = predict_data(sessions_predictor_model, cameras_predictor_model, timestamps_predictor_model, traffic_data['sessions'], traffic_data['cameras'], traffic_data['timestamps'])
    
    insert_predictions(dynamodb_client, sessions_table, cameras_table, timestamps_table, predictions['sessions'], predictions['cameras'], predictions['timestamps'])
