# Inspirationday Automation

Automation Setup for Arrow Inspiration Day 2023

## Wishlist

- [ ] AWX Install folder
- [ ] AWS Playbooks
- [ ] Azure Playbooks
- [ ] ESP32 AWX Deploy
- [ ] ESP32 Temp Sensor

## Guide

### Step 1 - Deploy AWX in AWS

Requires a Linux machine

#### Step 1.1 - Get AWS Credentials

In AWS Console create a new IAM user with programatic access

```bash
python3 -m venv ansible_insp

source ansible_insp/bin/activate

pip install ansible

pip install boto3

git clone https://github.com/arrowecsdk/inspirationday_automation.git

ansible-galaxy install jesperberth.deb_k3s --force

ansible-galaxy install jesperberth.awx_k8s_install --force

ansible-playbook 01_aws_tower_deploy.yml

```
