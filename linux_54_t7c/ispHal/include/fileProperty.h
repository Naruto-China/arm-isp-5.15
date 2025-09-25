/*
 * Copyright (c) 2025 Amlogic, Inc. All rights reserved.
 *
 * This source code is subject to the terms and conditions defined in the
 * file 'LICENSE' which is part of this source code package.
 *
 * Description:
 */

#ifndef __ISP_PROPERTY_H__
#define __ISP_PROPERTY_H__

#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define PROPERTY_KEY_MAX   256

// max 3k for coef property.
#define PROPERTY_VALUE_MAX  (3*1024)
#define PROPERTY_FILE_NAME  "/tmp/ispservice_config.txt"
#define USER_SET_EXP_TIME         "camera.exp.time"
#ifdef __cplusplus
extern "C" {
#endif

int system_property_set(const char* name, const char* value);
int system_property_get(const char* name, char* value);
int property_get(const char *key, char *value, const char *default_value);
int property_set(const char *key, const char *value);
int property_get_str(const char *key, char *value, const char *default_value);

#ifdef __cplusplus
}
#endif

#endif
