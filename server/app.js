'use strict';

var	compression = require('compression'),
	path = require('path'),
	express = require('express');

var app = express(),
	server = require('http').createServer(app);

var appDir = '/../app/';

 app.set('views', appDir);
 app.set('view engine', 'html');
 app.use(compression());

 app.use(express.static(path.resolve(__dirname + appDir)));

app.use('/', function(req, res){
	res.sendFile(path.resolve(__dirname + appDir +'index.html'));
});

server.listen(18000, function(){
	console.log('监听18000端口');
})
