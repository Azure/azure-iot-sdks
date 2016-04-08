FROM node:5
COPY . /tmp/azure-iot-sdks
ENV NPM_CONFIG_LOGLEVEL warn 
CMD /tmp/azure-iot-sdks/build/linux_node.sh
