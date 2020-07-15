#!/usr/bin/env bash

set -uo pipefail

COMMON_FILE="tests/endpoint/common.sh"

if [ ! -f "$COMMON_FILE" ]
then
    echo "$COMMON_FILE is not exists."
    exit 1
fi
source $COMMON_FILE

make TESTS=true EP_TARGET=wp76xx EP_TA_HOST=node1.puyuma.org EP_TA_PORT=6699 legato && \
tar zcf endpoint-wp76xx.tgz endpoint/_build_endpoint/wp76xx/app/endpoint/staging/read-only/
