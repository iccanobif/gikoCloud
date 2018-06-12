var express = require('express');
var app = express();
var http = require('http').Server(app);
var fs = require('fs');
var socketio = require("socket.io")(http);
var child_process = require("child_process");

function readCookie(cookies, cookieName)
{
    c = cookies.split(";");
    for (var i = 0; i < c.length; i++)
    {
        s = c[i].trim().split("=")
        if (s[0] == cookieName)
            return s[1];
    }
    return null;
}

socketio.on("connection", function (socket)
{
    console.log("Connection attempt");

    var user = null;

    socket.on("client2serverMessage", function (userName, messageContent)
    {
        try
        {
            console.log("got message from " + userName + ": " + messageContent);
            // backendProcess.stdin.write("msg " + userName + ": " + messageContent);
            socketio.emit("server2clientMessage", userName + ": " + messageContent); // send to all connected sockets
        }
        catch (e)
        {
            console.log(e.message + " " + e.stack);
        }
    });
});

app.get("/", function (req, res)
{
    res.writeHead(200, { 'Content-Type': 'text/html' });
    fs.readFile("static/home.htm", function (err, data)
    {
        if (err) res.end(err);
        else res.end(data);
    });
});

setInterval(() => {
    console.log("trying to send message to clients");
    socketio.emit("server2clientMessage", "suck my dicko"); // send to all connected sockets
}, 1000);

// var backendProcess = child_process.spawn("/src/src/cli/cli");

// //backendProcess.stdout.on("data", data => console.log("Message from the backend (stdout): " + data));
// backendProcess.stdout.on("data", function (data)
// {
//     console.log("Message from the backend (stdout): " + data);
// });

// backendProcess.stderr.on("data", function (data)
// {
//     console.log("Message from the backend (stderr): " + data);
// });

app.use(express.static('static', {
    "maxAge": 24 * 60 * 60 * 1000 // 1 day in milliseconds
}));
http.listen(8080, "0.0.0.0");

console.log("Server running");

// backendProcess.stdin.write("msg lol");