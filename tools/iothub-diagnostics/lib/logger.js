/* LOGGER EXAMPLES
    app.logger.trace('testing');
    app.logger.debug('testing');
    app.logger.info('testing');
    app.logger.warn('testing');
    app.logger.crit('testing');
    app.logger.fatal('testing');
    */

const winston = require('winston');
const settings = require('../config');

var customColors = {
  trace: 'white',
  debug: 'blue',
  info: 'green',
  warn: 'yellow',
  crit: 'red',
  fatal: 'red'
};

function customFileFormatter (options) {
  return (options.timestamp ? new Date().toISOString() + ': ' : '') +
    (undefined !== options.message ? options.message : '') +
  	(options.meta && Object.keys(options.meta).length ? '\n\t'+ JSON.stringify(options.meta) : '' );
}

var logger = new(winston.Logger)({
  colors: customColors,
  levels: {
    fatal: 0,
    crit: 1,
    warn: 2,
    info: 3,
    debug: 4,
    trace: 5
  },
  transports: [
  new(winston.transports.Console)({
    level: settings.consoleLogLevel,
    colorize: true,
    timestamp: true
  }),
  new (winston.transports.File)({
    level: settings.fileLogLevel,
    filename: settings.logFileName,
    json: false,
    formatter: customFileFormatter
  })]
});

winston.addColors(customColors);

var origLog = logger.log;

logger.log = function (level, msg) {
  var objType = Object.prototype.toString.call(msg);
  if (objType === '[object Error]') {
    origLog.call(logger, level, msg.toString());
  } else {
    origLog.call(logger, level, msg);
  }
};

module.exports = logger;
