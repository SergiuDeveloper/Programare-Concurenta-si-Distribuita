import boto3

from flask import Flask
from configparser import ConfigParser


CONFIG_FILE = '../configuration/config.ini'


app = Flask(__name__)

@app.get('/')
def serve_page():
    global dynamodb_client
    global sessions_table
    global cameras_table
    global timestamps_table

    with open('../resources/index.html') as index_file:
        return index_file.read()

@app.get('/plots.js')
def serve_plots_js():
    with open('../resources/plots.js') as plots_js_file:
        data = scan_bi_tables(dynamodb_client, sessions_table, cameras_table, timestamps_table)

        for category in ['sessions', 'cameras', 'timestamps']:
            date = [session['date'] for session in data[category]]
            traffic = [session['traffic'] for session in data[category]]
            traffic = [x for _, x in sorted(zip(date, traffic))]
            date = sorted(date)
            data[category] = {'date': date, 'traffic': traffic}

        file_content = plots_js_file.read()
        file_content = file_content.replace('* DATA * ', str(data))

        return file_content


def read_config(config_file_path):
    with open(config_file_path) as config_file:
        config_parser = ConfigParser()
        config_parser.read_file(config_file)

        access_key, secret_key, region, sessions_table, cameras_table, timestamps_table = config_parser['DEFAULT']['AccessKey'], config_parser['DEFAULT']['SecretKey'], config_parser['DEFAULT']['Region'], config_parser['DEFAULT']['SessionsTable'], config_parser['DEFAULT']['CamerasTable'], config_parser['DEFAULT']['TimestampsTable']

    return access_key, secret_key, region, sessions_table, cameras_table, timestamps_table

def scan_bi_tables(dynamodb_client, sessions_table, cameras_table, timestamps_table):
    data = {}

    for table_item in [(sessions_table, 'sessions'), (cameras_table, 'cameras'), (timestamps_table, 'timestamps')]:
        table_name = table_item[0]
        name = table_item[1]

        response = dynamodb_client.scan(
            TableName=table_name,
            Select='ALL_ATTRIBUTES'
        )
        
        items = response['Items']
        items = [{
            'date': item['date']['S'],
            'traffic': float(item['traffic']['N'])
        } for item in items]

        data[name] = items

    return data


access_key, secret_key, region, sessions_table, cameras_table, timestamps_table = read_config(CONFIG_FILE)
dynamodb_client = boto3.client('dynamodb', aws_access_key_id=access_key, aws_secret_access_key=secret_key, region_name=region)


if __name__ == '__main__':
    app.run()