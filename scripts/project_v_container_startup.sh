#!/bin/bash -e

export RELEX_CONTAINER_IP=$(cat /etc/hosts | grep relex | cut -f1 | sort -u)
guile -l ../opencog/nlp/chatbot/run-chatbot.scm
