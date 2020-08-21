#!/bin/bash

LIVE_DIR="/etc/letsencrypt/live/"

if [ ! -d $LIVE_DIR ]; then
  LIVE_DIR="/usr/local/etc/letsencrypt/live"
  if [ ! -d $LIVE_DIR ]; then
    echo -e "\033[31m[AutoSetup] You haven't installed certbot, or at least neither the folder \033[37m$LIVE_DIR\033[31m nor \033[37m/etc/letsencrypt/live\033[31m exists.\033[0m"
    exit 1
  fi
fi

if [ $(ls -lad $LIVE_DIR*/ | wc -l) -eq "1" ]; then
  export WS_TLS_CERT=$(echo $LIVE_DIR*/cert.pem)
  export WS_TLS_CHAIN=$(echo $LIVE_DIR*/chain.pem)
  export WS_TLS_PRIVATE_KEY=$(echo $LIVE_DIR*/privkey.pem)
  echo -e "\033[32m[AutoSetup] Success.\033[0m"
else
  echo -e "\033[31m[AutoSetup] This script cannot setup Let's Encrypt automatically because"
  echo "there are multiple directories (multiple seperate targets/domains) in"
  echo -e "\033[37m$LIVE_DIR\033[0m"
  cur_pwd=$(pwd)
  cd $LIVE_DIR
  ls -d */ | sed 's:/*$::' | awk '{print "\033[94m - \033[35m"$0"\033[0m"}'
  cd $cur_pwd
fi
