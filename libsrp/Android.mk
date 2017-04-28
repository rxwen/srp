# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := eng
LOCAL_C_INCLUDES := external/openssl/include
LOCAL_CFLAGS += -DHAVA_MEMCPY -DUSE_SGTTY -DOPENSSL -DOPENSSL_ENGINE -DOPENSSL_SHA -DUSE_SHA512
LOCAL_SRC_FILES := \
	./t_truerand.c \
	./yp_tpasswd.c \
	./t_misc.c \
	./rfc2945_client.c \
	./srp6_server.c \
	./nsw_tconf.c \
	./t_conf.c \
	./t_pw.c \
	./srp.c \
	./yp_tconf.c \
	./t_math.c \
	./yp_misc.c \
	./nsswitch.c \
	./t_client.c \
	./t_read.c \
	./srp6_client.c \
	./cstr.c \
	./t_server.c \
	./rfc2945_server.c \
	./t_conv.c \
	./nsw_tpasswd.c

LOCAL_SHARED_LIBRARIES:=\
    libopenssl \

LOCAL_MODULE := libsrp
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := eng
LOCAL_C_INCLUDES := external/openssl/include
LOCAL_CFLAGS += -DHAVA_MEMCPY -DUSE_SGTTY -DOPENSSL -DOPENSSL_ENGINE -DOPENSSL_SHA -DUSE_SHA512
LOCAL_SRC_FILES := \
	./srp6bench.c

LOCAL_SHARED_LIBRARIES:=\
    libsrp \

LOCAL_MODULE := srp6bench
include $(BUILD_EXECUTABLE)
