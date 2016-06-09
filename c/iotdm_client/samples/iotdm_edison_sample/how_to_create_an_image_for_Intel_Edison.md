## How to Create an image for Intel Edison

This document describes how to create test images for Intel Edison containing Microsoft Azure IoT Hub device management components.

Note: the images created using these instructions are designed for performing tests with Azure IoT Hub device management Preview. Do not use these images in production systems.

## Prerequisites

You should have the following items ready before beginning the process:

-   A Linux Ubuntu 14.04 desktop to create the Intel Edison images

## Creating a base Intel Edison image:

On a Linux 64-bit Ubuntu 14.04 follow these steps to build the latest Yocto build for Linux: <http://www.hackgnar.com/2016/01/manually-building-yocto-images-for.html>

Below are the commands we used to create the image:

```
sudo apt-get -y install build-essential git diffstat gawk chrpath texinfo libtool gcc-multilib libsdl1.2-dev u-boot-tools
mkdir -p ~/src/edison
cd ~/src/edison
curl -O http://downloadmirror.intel.com/25028/eng/edison-src-ww25.5-15.tgz
tar xfvz edison-src-ww25.5-15.tgz
cd edison-src
./meta-intel-edison/setup.sh --parallel_make=4 --bb_number_thread=4
cd out/linux64
source poky/oe-init-build-env
bitbake -c fetchall edison-image
```

>Note: We've recently seen the following errors when running `bitbake -c fetchall edison-image`:
>
>```
>ERROR: Fetcher failure: Fetch command failed with exit code 128, output:
>Cloning into bare repository '~/src/edison/edison-src/bbcache/downloads/git2/git.eclipse.org.gitroot.paho.org.eclipse.paho.mqtt.c.git'...
>fatal: repository 'http://git.eclipse.org/gitroot/paho/org.eclipse.paho.mqtt.c.git/' not found
>
>ERROR: Function failed: Fetcher failure for URL: 'git://git.eclipse.org/gitroot/paho/org.eclipse.paho.mqtt.c.git;protocol=http'. Unable to fetch URL from any source.
>ERROR: Logfile of failure stored in: ~/src/edison/edison-src/out/linux64/build/tmp/work/core2-32-poky-linux/paho-mqtt/3.1-r1/temp/log.do_fetch.45221
>ERROR: Task 105 (~/src/edison/edison-src/out/linux64/poky/meta-intel-iot-middleware/recipes-connectivity/paho-mqtt/paho-mqtt_3.1.bb, do_fetch) failed with exit code '1'
>```
>
>You can work around these errors by opening ~/src/edison/edison-src/meta-intel-edison/meta-intel-edison-distro/recipes-core/images/edison-image.bb and commenting out lines to match the following:
>
>```
># Edison Middleware stuff
>IMAGE_INSTALL += "packagegroup-core-buildessential"
># IMAGE_INSTALL += "iotkit-opkg"
>IMAGE_INSTALL += "zeromq-dev"
>IMAGE_INSTALL += "cppzmq-dev"
># IMAGE_INSTALL += "paho-mqtt-dev"
>IMAGE_INSTALL += "mdns-dev"
># IMAGE_INSTALL += "iotkit-comm-js"
># IMAGE_INSTALL += "iotkit-comm-c-dev"
># IMAGE_INSTALL += "iotkit-agent"
># IMAGE_INSTALL += "iotkit-lib-c-dev"
>IMAGE_INSTALL += "xdk-daemon"
>IMAGE_INSTALL += "oobe"
>```
>
>Now re-run the previous bitbake command, and continue with the proceeding instructions.

```
bitbake edison-image
mkdir ~/src/edison/edison-src/build
cd ~/src/edison/edison-src/
ln -s ~/src/edison/edison-src/out/linux64/build/tmp ~/src/edison/edison-src/build/tmp
./meta-intel-edison/utils/flash/postBuild.sh
cd build/toFlash/
rm ~/edison.zip
zip -r ~/edison.zip .
```

Note: the bitbake image build process is particularly sensitive to the differences between dashes and underscores. Dashers are a character that you can use in identifiers. Underscores can sometimes be used in identifiers, but they are also sometimes used as a separator. You should pay particular attention to the difference between dashes and underscores in these instructions

## Configure Wifi to connect automatically

-   Edit the file **~/src/edison/edison-src/meta-intel-edison/meta-intel-edison-distro/recipes-connectivity/wpa\_supplicant/wpa-supplicant/wpa\_supplicant.conf-sane**

-   Add the following lines:

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

Any deviations on the formatting of the **wpa\_supplicant.conf-sane** file can cause the wifi services to fail to boot. It is recommended to obtain the wifi settings file directly from a running Edison device connected to the wifi network you want to use in your image. To do so do the following:

1.  On a running device the current wifi information is stored in **~/etc/wpa_supplicant/wpa_supplicant.conf**

2.  Create a copy of this file called **wpa\_supplicant.conf-sane**

3.  Then copy this file into your Ubuntu desktop machine at location **~/src/edison/edison-src/meta-intel-edison/meta-intel-edison-distro/recipes-connectivity/wpa_supplicant/wpa-supplicant/**

## Adding the device management client to the image 

-   Clone the azure-iot-sdks branch into your home folder
    ```
    cd ~
    git clone https://github.com/Azure/azure-iot-sdks -b dmpreview --recursive
    ```

-   If you clone into a different folder, you will have to edit the recipe file (in **azure-iot-sdks/c/iotdm_client/samples/iotdm_edison_sample/bitbake/iotdm-edison-sample.bb**) to point to your local clone

-   Copy the recipe into your bitbake build directory:
    ```
    cd ~/azure-iot-sdks/c/iotdm_client/samples/iotdm_edison_sample/bitbake/
    ./do_populate.sh
    ```

-   Edit **~/src/edison/edison-src/meta-intel-edison/meta-intel-edison-distro/recipes-core/images/edison-image.bb** and add this line at the bottom:

    ```
    IMAGE_INSTALL += "iotdm-edison-sample"
    ```

-   After you do this, you will need to update your Edison Image. Follow the steps below:

    ```
    cd ~/src/edison/edison-src/out/linux64
    source poky/oe-init-build-env
    bitbake edison-image
    cd ~/src/edison/edison-src/
    ./meta-intel-edison/utils/flash/postBuild.sh
    cd build/toFlash/
    rm ~/edison.zip
    zip -r ~/edison.zip .
    ```

The newly created edison.zip file contains an image for the Intel Edison that includes the iotdm_edison_sample agent. You can use this image to experiment with firmware update and factory reset scenarios.
