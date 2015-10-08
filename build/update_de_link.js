// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var fs = require('fs');
var path = require('path');
var util = require('util');

function main() {
	// the new version string should be passed in as a command line param
	if(process.argv.length < 3) {
		printUsage();
		process.exit(1);
	}
	
	var version = process.argv[2];
	var filePath = path.join(
		__dirname,
		'../tools/DeviceExplorer/doc/how_to_use_device_explorer.md');
	
	// load the md file in question
	fs.readFile(filePath, 'utf-8', function(err, data) {
		if(err) {
			console.err('Could not open how_to_use_device_explorer.md. Error: ', err);
			process.exit(1);
		}
		
		// replace the version string in the URL
		var regex = /(https\:\/\/github.com\/Azure\/azure-iot-sdks\/releases\/download\/).*(\/SetupDeviceExplorer.msi)/gm;
		data = data.replace(regex, '$1' + version + '$2');
		fs.writeFileSync(filePath, data);
	});
}

function printUsage() {
	console.log(util.format('Usage: node %s <version-string>',
		path.basename(process.argv[1])));
}

main();
