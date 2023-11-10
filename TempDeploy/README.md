# TempDeploy

## AWS TimeStream

### Database

Create standard Database "tempStream"

### Table

Create table "tempHumi"

Default Partition

Data Retention: Memory 12 hours, Magnetic 1 Day

## AWS IoT Core

### Thing

Create thing "esp32_temp"

Copy Certifcates to "src/secret.h"

### Policies

Create "IoTTempPolicy"

iot:Connect
iot:Publish
iot:Receive
iot:RetainPublish
iot:Subscribe

### Rules

SQL Statement: SELECT * FROM 'esp32/temp'

TimeStream Table / tempStream / tempHumi

Dimentions: host = ${hostname}

TimeValue: ${time} - MILLISECONDS

IAM Role: tempThing_to_TimeStream

Error Action: Republish to IoT Topic

## EC2 Grafana

### Instance

### IAM Role
