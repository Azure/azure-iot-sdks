// Copyright (c) 2013 Titanium I.T. LLC. All rights reserved. See LICENSE.TXT for details.
(function() {
	"use strict";

	var semver = require("semver");

	exports.check = function(options, success, fail) {
		if (semver.satisfies(options.actual, options.expected)) return success();
		return fail("Incorrect " + options.name + " version. Expected within range " +
			options.expected + ", but was " + options.actual + ".");
	};

}());