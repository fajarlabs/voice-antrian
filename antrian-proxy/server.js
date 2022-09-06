var http = require('http'),
    httpProxy = require('http-proxy'),
    express = require('express');

// create a server
var app = express();
var proxy = httpProxy.createProxyServer({ target: 'http://127.0.0.1:18081', ws: true });
var server = require('http').createServer(app);

// proxy HTTP GET / POST
app.get('/*', function(req, res) {
  console.log("proxying GET request", req.url);
  proxy.web(req, res, {});
});

// Proxy websockets
server.on('upgrade', function (req, socket, head) {
  console.log("proxying upgrade request", req.url);
  proxy.ws(req, socket, head);
});

server.listen(18080);