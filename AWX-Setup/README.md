# AWX-Setup

## Deploy AWX in AWS

Requires a Linux machine with Python 3.9+

### Create Azure Service Principal

In the Azure Cloud Shell

```bash
SubID=$(az account list --query "[].{id:id}" -o tsv)

az ad sp create-for-rbac --name awx-demo --role Contributor --scopes /subscriptions/$SubID

echo $SubID

Save the credentials in ~/.azure/credentials on your linux machine

In this format

[default]
subscription_id=xxxx-xxxx-xxxx-xxxx-xxxx
client_id=xxxx-xxxx-xxxx-xxxx-xxxx
secret=xxxx-xxxx-xxxx-xxxx-xxxx
tenant=xxxx-xxxx-xxxx-xxxx-xxxx
```

### Create AWS IAM User

In the AWS Cloud Shell

```bash
aws iam create-user --user-name awx-demo

aws iam add-user-to-group --user-name awx-demo --group-name AWSAdmins

aws iam create-access-key --user-name awx-demo

Save the credentials in ~/.aws/credentials on you linux machine

In this format

[default]
aws_access_key_id=XXXXXXXXXXXXXXXXXX
aws_secret_access_key=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

```

### Install and run playbooks on your linux machine

```bash

ssh-keygen

python3 -m venv ansible_insp

source ansible_insp/bin/activate

pip install ansible

pip install boto3

git clone https://github.com/arrowecsdk/inspirationday_automation.git

ansible-galaxy install jesperberth.deb_k3s --force

ansible-galaxy install jesperberth.awx_k8s_install --force

ansible-playbook AWX-Setup/01_aws_tower_deploy.yml

ansible-playbook -i AWX-Setup/inventory.aws_ec2.yml AWX-Setup/02_install_awx_ec2.yml

ansible-playbook AWX-Setup/03_credentials.yml

ansible-playbook AWX-Setup/04_configure_awx.yml

```
