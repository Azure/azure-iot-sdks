// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

var exec = require('child_process').exec;
var fs = require('fs');

function getModifiedDocs(cb) {
	var cmd = 'git status -s ../../c/doc/api_reference';
	exec(cmd, function(err, stdout, stderr) {
		if(err) {
			console.err('Unable to retrieve list of modified doc files. ' +
				'Error: ' + err);
			process.exit(1);
		}
		
		// strip the leading ' M: ' string
		var lines = stdout.split('\n').map(function(line) {
			return line.substr(3);
		}).filter(function(path) {
			// ignore empty file paths
			return path.trim().length > 0;
		});
		cb(lines);
	});
}

function fixCRLF(filePath, cb) {
	fs.readFile(filePath, 'utf-8', function(err, data) {
		if(err) {
			return cb(err);
		}
		
		var re = /(?:\r\n|\r|\n)/g;
		data = data.replace(re, '\r\n');
		fs.writeFile(filePath, data, {
			encoding: 'utf-8'
		}, cb);
	})
}

function main() {
	getModifiedDocs(function(filePaths) {
		filePaths.forEach(function(path) {
			fixCRLF(path, function(err) {
				if(err) {
					console.error('Fixing CR/LF in file %s failed with error %s.', path, err);
				}
			});
		});
	});
}

main();