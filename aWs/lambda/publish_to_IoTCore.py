import json
import boto3

client = boto3.client('iot-data', region_name='eu-west-1')

def lambda_handler(event, context):
    
    response = client.publish(
        topic='topic_board',
        qos=1,
        payload=event['body']
    )
    
    return {
        'statusCode': 200,
        'body': json.dumps('Published to topic')
    }