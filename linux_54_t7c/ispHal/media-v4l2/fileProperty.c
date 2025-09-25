/**
 *  @file aml_properties.c
 *
 *  @copyright Copyright (c) 2021 Amlogic, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  China. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Amlogic, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Amlogic, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Amlogic, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  Amlogic, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @author
 *
 *  @details
 *
 *  @history 15:45:14 2021-8-12 create
 *
 */

#include "fileProperty.h"
#include <stdbool.h>
#include "logs.h"

int system_property_set(const char* name, const char* value) {
    int fd = open(PROPERTY_FILE_NAME, O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        ERR("Error opening file");
        return 0;
    }

    if (flock(fd, LOCK_EX) < 0) {
        ERR("Error locking file");
        close(fd);
        return 0;
    }

    FILE *file = fdopen(fd, "r+");
    if (!file) {
        ERR("fdopen failed");
        flock(fd, LOCK_UN);
        close(fd);
        return 0;
    }

    long pos;
    int found = 0;
    char line[PROPERTY_KEY_MAX + PROPERTY_VALUE_MAX + 1];

    rewind(file);
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, name) == line && line[strlen(name)] == '=') {
            pos = ftell(file) - strlen(line);
            found = 1;
            break;
        }
    }

    if (found) {
        fseek(file, pos, SEEK_SET);
        fprintf(file, "%s=%s\n", name, value);
        fflush(file);
        ftruncate(fd, ftell(file));
    } else {
        fseek(file, 0, SEEK_END);
        fprintf(file, "%s=%s\n", name, value);
        fflush(file);
    }

    flock(fd, LOCK_UN);
    fclose(file);

    return strnlen(value, PROPERTY_VALUE_MAX - 1);
}


int system_property_get(const char* name, char* value) {
    int fd = open(PROPERTY_FILE_NAME, O_RDONLY);
    if (fd < 0) {
        return 0;
    }

    if (flock(fd, LOCK_SH) < 0) {
        close(fd);
        return 0;
    }
    FILE *file = fdopen(fd, "r");
    if (file == NULL) {
        flock(fd, LOCK_UN);
        close(fd);
        return 0;
    }
    char line[PROPERTY_KEY_MAX + PROPERTY_VALUE_MAX + 1];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, name) == line && line[strlen(name)] == '=') {
            const char *val_start = line + strlen(name) + 1;
            strncpy(value, val_start, PROPERTY_VALUE_MAX - 1);
            value[PROPERTY_VALUE_MAX - 1] = '\0';
            value[strcspn(value, "\n")] = '\0';
            flock(fd, LOCK_UN);
            fclose(file);
            return strnlen(value, PROPERTY_VALUE_MAX - 1);
        }
    }
    flock(fd, LOCK_UN);
    fclose(file);
    return 0;
}


int _property_get(const char *key, char *value, const char *default_value)
{
    int len = system_property_get(key, value);
    if (len > 0) {
        return len;
    }
    if (default_value) {
        len = strnlen(default_value, PROPERTY_VALUE_MAX - 1);
        memcpy(value, default_value, len);
        value[len] = '\0';
    }
    return len;
}

int property_set(const char *key, const char *value)
{
    return system_property_set(key, value);
}

int property_get_str(const char *key, char *value, const char *default_value)
{
    return _property_get(key, value, default_value);
}


