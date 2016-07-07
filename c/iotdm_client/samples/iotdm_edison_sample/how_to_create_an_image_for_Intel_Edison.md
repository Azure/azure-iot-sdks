# Creating image for Intel Edison from sources

This document describes how to create test images for the Intel Edison containing Microsoft Azure IoT Hub device management components.

> *Note: the images created using these instructions are designed for performing tests with Azure IoT Hub Device Management Preview. Do not use these images in production systems.*

#### System check and preparation for Ubuntu 14.04

```
sudo apt-get -y install build-essential git diffstat gawk chrpath texinfo libtool gcc-multilib libsdl1.2-dev u-boot-tools
mkdir -p ~/src/edison
cd ~/src/edison
curl -O http://iotdk.intel.com/src/3.5/edison/iot-devkit-yp-poky-edison-20160606.zip
unzip iot-devkit-yp-poky-edison-20160606.zip
```

## Modify the sources to include `iotdm_edison_sample`
### Configure Wi-Fi via `wpa_supplicant.conf`

Any deviations on the formatting of the `wpa_supplicant.conf-sane` file can cause the wifi services to fail to boot. It is recommended to obtain the wifi settings file directly from a running Edison device connected to the wifi network you want to use in your image.

#### Option 1: Copy/paste the `wpa_supplicant.conf` file from a running Edison

```
scp root@<edison-ip-address>:/etc/wpa_supplicant/wpa_supplicant.conf ~/src/edison/iot-devkit-yp-poky-edison-20160606/poky/meta-intel-edison/meta-intel-edison-distro/recipes-connectivity/wpa_supplicant/wpa-supplicant/wpa_supplicant.conf-sane
```

#### Option 2: Manual configuration
- Edit the `wpa_supplicant.conf-sane` file:

    ```
    nano ~/src/edison/iot-devkit-yp-poky-edison-20160606/poky/meta-intel-edison/meta-intel-edison-distro/recipes-connectivity/wpa_supplicant/wpa-supplicant/wpa_supplicant.conf-sane
    ```

- Add the following lines:

    ```
    network={
        ssid="<your wifi ssid>"
        key_mgmt=WPA-PSK
        pairwise=CCMP TKIP
        group=CCMP TKIP WEP104 WEP40
        eap=TTLS PEAP TLS
        psk="<your wifi password>"
    }
    ```

### Adding the device management client to the image 

- Clone the azure-iot-sdks branch into your home folder

  ```
  git clone https://github.com/Azure/azure-iot-sdks ~/azure-iot-sdks/ --branch dmpreview --recursive
  ```

  > *NOTE: If you clone the repository into a different folder, you will have to edit the recipe file (`azure-iot-sdks/c/iotdm_client/samples/iotdm_edison_sample/bitbake/iotdm-edison-sample.bb`) to point to your local clone.*

- Copy the recipe into your bitbake build directory:

  ```
  pushd ~/azure-iot-sdks/c/iotdm_client/samples/iotdm_edison_sample/bitbake/
  ./do_populate.sh ~/src/edison/iot-devkit-yp-poky-edison-20160606/poky/meta-intel-edison/meta-intel-edison-distro/recipes-support
  popd
  ```

- Append a line to the `edison-image.bb` file:

  ```
  echo 'IMAGE_INSTALL += "iotdm-edison-sample"' >> ~/src/edison/iot-devkit-yp-poky-edison-20160606/poky/meta-intel-edison/meta-intel-edison-distro/recipes-core/images/edison-image.bb
  ```

## Creating the custom image

#### Follow the steps found in README.edison

  ```
  cat README.edison
  ```

  Skipping the `unzip` step, follow the remaining instructions found in `README.edison` *(contents as of 06 JUL 2016)*:

  > ```
  > # Building the edison image
  > unzip iot-devkit-yp-poky-edison-20160606.zip
  > cd iot-devkit-yp-poky-edison-20160606/poky/
  > source oe-init-build-env ../build_edison/
  > bitbake edison-image u-boot
  > ../poky/meta-intel-edison/utils/flash/postBuild.sh .
  > zip -r toFlash.zip toFlash
  > ```

The newly created edison.zip file contains an image for the Intel Edison that includes the iotdm_edison_sample agent. You can use this image to experiment with firmware update and factory reset scenarios.

#### Send the new image to your Edison

```
scp toFlash.zip root@<edison_ip_address>:~/edison.zip
```

**RESOURCES:**
- [hackgnar: Manually Building Yocto Images for the Intel Edison Board from Source][hackgnar]

[hackgnar]: http://www.hackgnar.com/2016/01/manually-building-yocto-images-for.html
