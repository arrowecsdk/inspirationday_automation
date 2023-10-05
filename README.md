# Inspirationday Automation

Automation Setup for Arrow Inspiration Day 2023

## ToDo List

- [x] AWX Install folder
- [x] AWS Playbooks
- [x] Azure Playbooks
- [ ] ESP32 AWX Deploy
- [ ] ESP32 Temp Sensor
- [ ] Website

## Guide

### Step 1 - Deploy AWX in AWS

Requires a Linux machine

#### Step 1.1 - Get AWS Credentials

In AWS Console create a new IAM user with programatic access

```bash
ssh-keygen

python3 -m venv ansible_insp

source ansible_insp/bin/activate

pip install ansible

pip install boto3

git clone https://github.com/arrowecsdk/inspirationday_automation.git

ansible-galaxy install jesperberth.deb_k3s --force

ansible-galaxy install jesperberth.awx_k8s_install --force

cd AWx-Setup

ansible-playbook 01_aws_tower_deploy.yml

ansible-playbook -i inventory.aws_ec2.yml install_awx_ec2.yml

```

Copy the hostname from the last line

![Alt text](images/001_hostname.png?raw=true "Hostname")

It takes ~ 5 miutes for AWX to run the deployment

Once ready login

user - admin

Create a OAuth Token

Click __Users / admin / Tokens__

Click __Add__ Select __Write__ in the __Scope__ dropdown

```bash
export CONTROLLER_HOST=http://<hostname>
export CONTROLLER_USERNAME=admin
export CONTROLLER_OAUTH_TOKEN=oKux6ADwqn127BK3Ov4WjfbPjtPG16

ansible-playbook AWX-Setup/configure_awx.yml

```
