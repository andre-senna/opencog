# Tutorial - How to write an Opencog service

-------------------------------

_Before following this tutorial, make sure you've installed_

* _Docker (https://www.docker.com/)_
* _Metamask (https://metamask.io)_

_You will need a private-public key pair to register your service in SNET. Generate them in Metamask before you start this turorial._

_You will also need an Opencog container running with Relex. Follow the
instructions to setup docker-compose from here:
https://github.com/opencog/docker/blob/master/opencog/README.md but use the
SNET forked version of opencog (https://github.com/singnet/opencog)._

_See https://github.com/singnet/wiki/tree/master/tutorials/howToWriteCPPService
for detailed information regarding the aspects of C++ gRPC implementation_

-------------------------------

In this tutorial we'll create an Opencog-based service and publish it in SingularityNET.

## Step 1 

Start an Opencog container using docker-compose. Follow this tutorial in
container's prompt.

## Step 2

Install prerequisites in container.

```Sh
$ cd /opencog/opencog/snet-services
$ sudo ./install_prerequisites.sh
```

## Step 3


