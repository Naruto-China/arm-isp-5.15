
LOCAL_PATH := $(shell pwd)
define module_clean
    make -C $(LOCAL_PATH)/$(1) clean
endef

.PHONY: clean media-v4l2_clean sensor_calibration_clean v4l2_test_media_clean ispService_clean lens_clean

clean:
	$(call module_clean,media-v4l2)
	$(call module_clean,sensor_calibration)
	$(call module_clean,v4l2_test_media)
	$(call module_clean,ispService)
	$(call module_clean,lens)
	@rm -rf $(LOCAL_PATH)/v4l2_test_media/v4l2_test_raw.o
	@rm -rf $(LOCAL_PATH)/ispService/ispMgr.o
	@rm -rf $(LOCAL_PATH)/ispService/ispService.o

media-v4l2_clean:
	$(call module_clean,media-v4l2)

sensor_calibration_clean:
	$(call module_clean,sensor_calibration)

v4l2_test_media_clean:
	$(call module_clean,v4l2_test_media)
	@rm -rf $(LOCAL_PATH)/v4l2_test_media/v4l2_test_raw.o
ispService_clean:
	$(call module_clean,ispService)
	@rm -rf $(LOCAL_PATH)/ispService/ispMgr.o
	@rm -rf $(LOCAL_PATH)/ispService/ispService.o
lens_clean:
	$(call module_clean,lens)
