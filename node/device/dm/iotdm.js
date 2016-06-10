var wrapper = require('./emwrapper.js')

var CreateSession = wrapper.cwrap('CreateSession', 'number', [ 'string' ]);
var Connect = wrapper.cwrap('Connect', null, [ 'number' ]);
var DoWork = wrapper.cwrap('DoWork', null, [ 'number' ]);

var s = CreateSession("<connection_string>");
Connect(s);
setInterval(function() { DoWork(s); }, 1000);


 