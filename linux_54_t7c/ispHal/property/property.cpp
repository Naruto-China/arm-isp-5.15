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
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "logs.h"
#include "fileProperty.h"

void usage(char * prog){
    INFO("%s\n", prog);
    INFO("usage:\n");
    INFO(" example   : property -t 1 -e 33000\n");
    INFO("    t : set or get: 1 set 0 get\n");
    INFO("    e : exposuretime\n");
    INFO("    s : saturation\n");
}
const char* keyMap[] = {
    USER_SET_EXP_TIME,
    USER_SET_AWB,
    USER_SET_BRIGHTNESS,
    USER_SET_CONTRAST,
    USER_SET_SATURATION,
};
int main(int argc, char *argv[])
{
    int ops_type = 0;
    int usr_value = 33000;
    int keyMap_idx = 0;

    if (argc < 1) {
        usage(argv[0]);
        return -1;
    }

    int c;

    while (optind < argc) {
        if ((c = getopt (argc, argv, "t:e:w:b:c:s:")) != -1) {
            switch (c) {
            case 't':
                ops_type = atoi(optarg);
                break;
            case 'e':
                keyMap_idx = 0;
                usr_value = atoi(optarg);
                break;
            case 'w':
                keyMap_idx = 1;
                usr_value = atoi(optarg);
                break;
            case 'b':
                keyMap_idx = 2;
                usr_value = atoi(optarg);
                break;
            case 'c':
                keyMap_idx = 3;
                usr_value = atoi(optarg);
                break;
            case 's':
                keyMap_idx = 4;
                usr_value = atoi(optarg);
                break;
            case '?':
                usage(argv[0]);
                exit(1);
            }
        } else {
            MSG("Invalid argument %s\n", argv[optind]);
            usage(argv[0]);
            exit(1);
        }
    }
    char value[PROPERTY_VALUE_MAX];
    if (ops_type == 1) {
        sprintf(value, "%d", usr_value);
        property_set(keyMap[keyMap_idx], value);
    } else {
        property_get_str(keyMap[keyMap_idx], value, "-1");
        MSG("get %s %s\n", keyMap[keyMap_idx], value);
    }
    return 0;
}


