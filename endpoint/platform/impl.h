/*
 * Copyright (C) 2020 BiiLabs Co., Ltd. and Contributors
 * All Rights Reserved.
 * This is free software; you can redistribute it and/or modify it under the
 * terms of the MIT license. A copy of the license can be found in the file
 * "LICENSE" at the root of this distribution.
 */

#ifndef ENDPOINT_IMPL_H
#define ENDPOINT_IMPL_H

#include <stdint.h>
#include <stdio.h>
#include "common/ta_errors.h"

status_t get_device_key(uint8_t *key);
status_t get_device_id(char *device_id);
status_t sec_write(const char *name, const uint8_t *buf, size_t buf_size);
status_t sec_read(const char *name, uint8_t *buf, size_t *buf_size);
status_t sec_delete(const char *name);

#endif
