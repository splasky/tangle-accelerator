# Copyright (C) 2019-2020 BiiLabs Co., Ltd. and Contributors
# All Rights Reserved.
# This is free software; you can redistribute it and/or modify it under the
# terms of the MIT license. A copy of the license can be found in the file
# "LICENSE" at the root of this distribution.

# need to configelgato first
export PA_DIR=${LEGATO_ROOT}/platformAdaptor
export LEGATO_MODEM_SERVICES=${LEGATO_ROOT}/components/modemServices
export LEGATO_SECSTORE=${LEGATO_ROOT}/components/secStore
export ENDPOINT_DIR=$(PWD)/endpoint

platform-build-command = \
	cd endpoint && \
	mkexe -o endpoint_test \
	-w endpoint_test_build \
	-t $(EP_TARGET) \
	-i ${LEGATO_ROOT}/interfaces \
	-i ${LEGATO_ROOT}/interfaces/secureStorage \
	-i ${LEGATO_ROOT}/interfaces/modemServices \
	-i ${LEGATO_SECSTORE}/platformAdaptor/inc \
	-i ${LEGATO_MODEM_SERVICES}/modemDaemon \
	-i ${LEGATO_ROOT}/framework/liblegato \
	-i ${LEGATO_ROOT}/framework/liblegato/linux \
	-i ${LEGATO_ROOT}/components/appCfg \
    	-i ${LEGATO_ROOT}/components/cfgEntries \
	-i ${PA_DIR}/simu/components/le_pa_secStore \
    	-i ${PA_DIR}/simu/components/le_pa \
	-i ${PA_DIR}/simu/components/simuConfig \
	$(LEGATO_FLAGS) \
	endpointComp \
	${PA_DIR}/simu/components/le_pa_secStore \
	${PA_DIR}/simu/components/simuConfig

	# mkapp -t $(EP_TARGET) $(LEGATO_FLAGS) endpoint.adef
	#leaf shell -c "mkapp -t $(EP_TARGET) $(LEGATO_FLAGS) endpoint.adef"
