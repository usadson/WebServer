#!/bin/bash

COLOR_RED='\033[0;31m'
COLOR_RESET='\033[0;0m'

function die() {
  printf "${COLOR_RED}E: $1${COLOR_RESET}\nUsage: $0 <file> <line count>\n" >&2
  exit 1
}

if [ -z "$1" ] || [ -z "$2" ] ; then
  die "E: Not enough arguments!"
fi

cat /dev/null > $1 || die "Failed to create file: '$1'"
len=$2

re='^[0-9]+$'
if ! [[ $len =~ $re ]] ; then
   die "LineCount is not a number!"
fi

for i in $(seq 1 $len);
do
   echo "$i" >> $1
done
