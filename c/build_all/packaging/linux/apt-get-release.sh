#!/bin/bash
#set -o pipefail
#

azureiot_folder="azure-iot-sdk"
azureiot_repo="https://github.com/Azure/azure-iot-sdks"
azureiot_c_folder="azure-iot-sdk-c-0.1.0"
azureiot_c_zip_folder="azure-iot-sdk-c-0.1.0.tar.gz"
azureiot_c_change_file="azure-iot-sdk-c_0.1.0-1_source.changes"

sync_azureiot()
{
	echo Sync Azure IoT SDK
	echo $azureiot_repo

	git clone $azureiot_repo $azureiot_folder
}

setup_debian_folder()
{
	cd ~/$azureiot_folder
	mv c $azureiot_c_folder
	tar czvf $azureiot_c_zip_folder $azureiot_c_folder
	cd $azureiot_c_folder
	dh_make -y -l -c mit -e aziotsdklinux@outlook.com -f ../$azureiot_c_zip_folder

	# Remove the debian folder
	dh_make -y -l -c mit -e aziotsdklinux@outlook.com -f ../$azureiot_c_zip_folder
	cp -r ~/$azureiot_folder/$azureiot_c_folder/build_all/packaging/linux/debian ./
}

build_package()
{
	cd ..
	echo build package
	dpkg-buildpackage -us -uc

	# Sign the packages
	dpkg-buildpackage -S
}

upload_to_Launchpad()
{
	dput ppa:aziotsdklinux/ppa-azure-iot-sdk-c $azureiot_c_file
}

sync_azureiot
build_package
