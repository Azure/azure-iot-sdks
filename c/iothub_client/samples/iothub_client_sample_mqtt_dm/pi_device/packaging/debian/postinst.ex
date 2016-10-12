#!/bin/sh
set -e
ln -sf /usr/share/iothub_client_sample/iothub_client_sample_firmware_update.service /lib/systemd/system/iothub_client_sample_firmware_update.service
ln -sf /usr/share/iothub_client_sample/iothub_client_sample_firmware_update.service /etc/systemd/system/multi-user.target.wants/iothub_client_sample_firmware_update.service
systemctl daemon-reload
