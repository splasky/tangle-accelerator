#!/usr/bin/env bash


COMMON_FILE="tests/endpoint/common.sh"

if [ ! -f "$COMMON_FILE" ]
then
    echo "$COMMON_FILE is not exists."
    exit 1
fi
source $COMMON_FILE

# setup FX30(WP7607-1) leaf shell
setup_leaf "fx30-stable" "swi-fx30-cat1_1.0.0"

set -euo pipefail

make TESTS=true EP_TARGET=wp76xx EP_TA_HOST=node.deviceproof.org EP_TA_PORT=5566 legato && \
tar zcf endpoint-wp76xx.tgz endpoint/_build_endpoint/wp76xx/app/endpoint/staging/read-only/
