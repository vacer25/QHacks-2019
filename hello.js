var http = require("http");

http.createServer(function (request, response) {
    // Send the HTTP header
    // HTTP Status: 200 : OK
    // Content type: text/plain
    response.writeHead(200, {'Content-Type' : 'text/plain'});
    // (What happens if this header is a lie?

    // Hello QHacks 2019!
    response.end('Hello Qhacks 2019\n');
}).listen(8081);

console.log('Server unning at localhost:8081');

// WOOT!