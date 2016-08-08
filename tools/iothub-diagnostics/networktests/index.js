'use strict';

const logger = require('../lib').logger;
const config = require('../config');
const dns = require('dns');
const ping = require('net-ping');

function traceRouteStepCb (err, target, ttl, sent, rcvd) {
  var ms = rcvd - sent;
  if (err) {
      if (err instanceof ping.TimeExceededError) {
          logger.debug(target + ': ' + err.source + ' (ttl=' + ttl + ' ms=' + ms +')');
      } else {
          logger.debug(target + ': ' + err.toString () + ' (ttl=' + ttl + ' ms=' + ms +')');
      }
  } else {
      logger.debug(target + ': ' + target + ' (ttl=' + ttl + ' ms=' + ms +')');
  }
}

function pingIpv4Address(ipv4Address, done) {
  logger.info('');
  logger.info("Pinging IPV4 address '" + ipv4Address + "'...");

  const options = {
      networkProtocol: ping.NetworkProtocol.IPv4,
      packetSize: 16,
      retries: 1,
      sessionId: (process.pid % 65535),
      timeout: 2000,
      ttl: 128
  };

 var session = ping.createSession (options);
  session.on ('error', function (err) {
      logger.crit(err.toString ());
      session.close ();
      return done(err)
  });

  session.pingHost (ipv4Address, function (error, target) {
    if (error) {
      logger.crit ('--> Failed to ping ' + target + ': ' + error.toString ());
    } else { 
      logger.info('--> Successfully pinged ' + target);
    }

    logger.info('');
    logger.info('Starting TraceRoute to ' + target + '...');

    session.traceRoute (ipv4Address, 100, traceRouteStepCb, function(err) {
      if(err) {
        logger.crit('--> Error running traceRoute: ' + err);
      } else {
        logger.info('--> Traceroute completed.');
      } 

      session.close();
      return done(null);    
    });
  });
};

function run(done) { 
  var domain = config.pingUrl;

  logger.info('');
  logger.info("Starting DNS resolution for host '" + domain + "'..." );
  dns.resolve4(domain, function(err, addresses) {
    if (err) {
       logger.crit("--> Failed to resolve host, error: " + err.code);
       return done(err);
    } else {
       logger.info('--> Successfully resolved DNS.');
       logger.debug(JSON.stringify(domain) + ' using address: ' + addresses[0]);
       pingIpv4Address(addresses[0], done);
    }
  });
}

module.exports = {
  run: run
};