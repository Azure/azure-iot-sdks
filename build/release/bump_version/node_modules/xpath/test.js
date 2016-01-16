var xpath = require('./xpath.js')
, dom = require('xmldom').DOMParser
, assert = require('assert');

module.exports = {
	'api': function(test) {
		assert.ok(xpath.evaluate, 'evaluate api ok.');
		assert.ok(xpath.select, 'select api ok.');
		test.done();
	},

	'evaluate': function(test) {
		var xml = '<book><title>Harry Potter</title></book>';
		var doc = new dom().parseFromString(xml);
		var nodes = xpath.evaluate('//title', doc, null, xpath.XPathResult.ANY_TYPE, null).nodes;
		assert.equal('title', nodes[0].localName);
		assert.equal('Harry Potter', nodes[0].firstChild.data);
		assert.equal('<title>Harry Potter</title>', nodes[0].toString());
		test.done();
	},

	'select': function(test) {
		var xml = '<book><title>Harry Potter</title></book>';
		var doc = new dom().parseFromString(xml);
		var nodes = xpath.select('//title', doc);
		assert.equal('title', nodes[0].localName);
		assert.equal('Harry Potter', nodes[0].firstChild.data);
		assert.equal('<title>Harry Potter</title>', nodes[0].toString());
		test.done();
	},

	'select single node': function(test) {
		var xml = '<book><title>Harry Potter</title></book>';
		var doc = new dom().parseFromString(xml);

		assert.equal('title', xpath.select('//title[1]', doc)[0].localName);

		test.done();
	},

	'select text node': function (test) {
		var xml = '<book><title>Harry</title><title>Potter</title></book>';
		var doc = new dom().parseFromString(xml);

		assert.deepEqual('book', xpath.select('local-name(/book)', doc));
		assert.deepEqual('Harry,Potter', xpath.select('//title/text()', doc).toString());

		test.done();
	},

	'select number node': function(test) {
		var xml = '<book><title>Harry</title><title>Potter</title></book>';
		var doc = new dom().parseFromString(xml);

		assert.deepEqual(2, xpath.select('count(//title)', doc));

		test.done();
	},

	'select xpath with namespaces': function (test) {
		var xml = '<book><title xmlns="myns">Harry Potter</title></book>';
		var doc = new dom().parseFromString(xml);

		var nodes = xpath.select('//*[local-name(.)="title" and namespace-uri(.)="myns"]', doc);
		assert.equal('title', nodes[0].localName);
		assert.equal('myns', nodes[0].namespaceURI) ;

		test.done();
	},

	'select xpath with namespaces, using a resolver': function (test) {
		var xml = '<book xmlns:testns="http://example.com/test"><testns:title>Harry Potter</testns:title><testns:field testns:type="author">JKR</testns:field></book>';
		var doc = new dom().parseFromString(xml);

		var resolver = {
			mappings: {
				'testns': 'http://example.com/test'
			},
			lookupNamespaceURI: function(prefix) {
				return this.mappings[prefix];
			}
		}

		var nodes = xpath.selectWithResolver('//testns:title/text()', doc, resolver);
		assert.equal('Harry Potter', xpath.selectWithResolver('//testns:title/text()', doc, resolver)[0].nodeValue);
		assert.equal('JKR', xpath.selectWithResolver('//testns:field[@testns:type="author"]/text()', doc, resolver)[0].nodeValue);

		test.done();
	},

	'select xpath with default namespace, using a resolver': function (test) {
		var xml = '<book xmlns="http://example.com/test"><title>Harry Potter</title><field type="author">JKR</field></book>';
		var doc = new dom().parseFromString(xml);

		var resolver = {
			mappings: {
				'testns': 'http://example.com/test'
			},
			lookupNamespaceURI: function(prefix) {
				return this.mappings[prefix];
			}
		}

		var nodes = xpath.selectWithResolver('//testns:title/text()', doc, resolver);
		assert.equal('Harry Potter', xpath.selectWithResolver('//testns:title/text()', doc, resolver)[0].nodeValue);
		assert.equal('JKR', xpath.selectWithResolver('//testns:field[@type="author"]/text()', doc, resolver)[0].nodeValue);

		test.done();
	},

	'select xpath with namespaces, prefixes different in xml and xpath, using a resolver': function (test) {
		var xml = '<book xmlns:testns="http://example.com/test"><testns:title>Harry Potter</testns:title><testns:field testns:type="author">JKR</testns:field></book>';
		var doc = new dom().parseFromString(xml);

		var resolver = {
			mappings: {
				'ns': 'http://example.com/test'
			},
			lookupNamespaceURI: function(prefix) {
				return this.mappings[prefix];
			}
		}

		var nodes = xpath.selectWithResolver('//ns:title/text()', doc, resolver);
		assert.equal('Harry Potter', xpath.selectWithResolver('//ns:title/text()', doc, resolver)[0].nodeValue);
		assert.equal('JKR', xpath.selectWithResolver('//ns:field[@ns:type="author"]/text()', doc, resolver)[0].nodeValue);

		test.done();
	},

	'select xpath with namespaces, using namespace mappings': function (test) {
		var xml = '<book xmlns:testns="http://example.com/test"><testns:title>Harry Potter</testns:title><testns:field testns:type="author">JKR</testns:field></book>';
		var doc = new dom().parseFromString(xml);
		var select = xpath.useNamespaces({'testns': 'http://example.com/test'});

		assert.equal('Harry Potter', select('//testns:title/text()', doc)[0].nodeValue);
		assert.equal('JKR', select('//testns:field[@testns:type="author"]/text()', doc)[0].nodeValue);

		test.done();
	},


	'select attribute': function (test) {
		var xml = '<author name="J. K. Rowling"></author>';
		var doc = new dom().parseFromString(xml);

		var author = xpath.select1('/author/@name', doc).value;
		assert.equal('J. K. Rowling', author);

		test.done();
	},
	
	'evaluate substring-after': function (test) {
	    var xml = '<classmate>Hermione</classmate>';
		var doc = new dom().parseFromString(xml);
		
		var part = xpath.select('substring-after(/classmate, "Her")', doc);
		assert.deepEqual('mione', part);
		
		test.done();
    }
}
