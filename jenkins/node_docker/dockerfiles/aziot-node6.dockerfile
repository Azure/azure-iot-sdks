FROM node:6
COPY . /tmp/azure-iot-sdks
ENV NPM_CONFIG_LOGLEVEL warn 
CMD /tmp/azure-iot-sdks/jenkins/linux_node.sh
