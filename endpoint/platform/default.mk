# Copyright (C) 2019-2020 BiiLabs Co., Ltd. and Contributors
# All Rights Reserved.
# This is free software; you can redistribute it and/or modify it under the
# terms of the MIT license. A copy of the license can be found in the file
# "LICENSE" at the root of this distribution.

export PA_DIR=${LEGATO_ROOT}/platform

platform-build-command = \
	source ./legato/bin/configlegatoenv && \	
	cd endpoint && \
	mkexe \ 
	-o endpoint_test \
	-w endpoint_build \
	-i endpoint/endpointComp \
	-t $(EP_TARGET) \
    -i ${LEGATO_ROOT}/framework/liblegato/ \
    -i ${LEGATO_ROOT}/interfaces/modemServices/ \
    -i ${LEGATO_ROOT}/interfaces/ \
	-l /home/splasky/Desktop/Biilabs/workspace/tangle-accelerator/endpoint/ \
	-C -static \
    ${CFLAGS} \
    ${LFLAGS} \
	$(LEGATO_FLAGS) \
    -L "-ljansson" \
	endpointComp

	# mkapp -t $(EP_TARGET) $(LEGATO_FLAGS) endpoint.adef
	#leaf shell -c "mkapp -t $(EP_TARGET) $(LEGATO_FLAGS) endpoint.adef"
