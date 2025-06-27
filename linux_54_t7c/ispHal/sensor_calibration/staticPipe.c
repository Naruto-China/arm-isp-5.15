/*
 * Copyright (c) 2021 Amlogic, Inc. All rights reserved.
 *
 * This source code is subject to the terms and conditions defined in the
 * file 'LICENSE' which is part of this source code package.
 *
 * Description:
 */

#include "staticPipe.h"
#include "logs.h"

int fetchPipeMaxResolution(media_stream_t *stream, uint32_t *width, uint32_t *height) {
    struct sensorConfig * cfg = matchSensorConfigByStream(stream);
    if (cfg) {
        *width = cfg->sensorWidth;
        *height = cfg->sensorHeight;
        INFO("find matched sensor configs %dx%d", *width, *height);
        return 0;
    }
    ERR("do not find matched sensor configs");
    return -1;
}

int fetchSensorFormat(media_stream_t *stream, int hdrEnable) {
    struct sensorConfig * cfg = matchSensorConfigByStream(stream);
    if (cfg) {
        return hdrEnable ? cfg->wdrFormat : cfg->sdrFormat;
    }
    ERR("do not find matched");
    return -1;
}

enum sensorType fetchSensorType(media_stream_t * stream) {
    struct sensorConfig * cfg = matchSensorConfigByStream(stream);
    if (cfg) {
        return cfg->type;
    }
    return sensor_NULL;
}

