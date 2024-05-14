import json
import boto3
import time

client=boto3.client('dynamodb')

def lambda_handler(event, context):
    data = client.scan(
        TableName='black_db',
    )
    
    return {
        'statusCode': 200,
        'body': json.dumps(data["Items"])
    }