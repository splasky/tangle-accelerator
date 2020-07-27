#!/usr/bin/env bash

set -uo pipefail

# Create endpoint app
make TESTS=true EP_TARGET=localhost EP_TA_HOST=node.deviceproof.org EP_TA_PORT=5566 legato

# Run endpoint app test here

export TARGET_TYPE="localhost"
export PATH="${LEGATO_ROOT}/build/localhost/bin:${LEGATO_ROOT}/bin:$PATH"

# startlegato


# stoplegato
