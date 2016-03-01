#!/usr/bin/env bash

INPROTOFOLDER="../src/"
INPROTONAMES=("VizaGlobal.proto" "VizaImpl.proto")
OUTFOLDER="../src/protobuf/"

for I in "${INPROTONAMES[@]}"; do
  OUTPROTO+="$INPROTOFOLDER$I "
done


echo $OUTPROTO
protoc --cpp_out=$OUTFOLDER -I$INPROTOFOLDER $OUTPROTO