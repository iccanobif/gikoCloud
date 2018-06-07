var express = require('express');
var app = express();
var http = require('http').Server(app);
var fs = require('fs');
var io = require("socket.io")(http);

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

/*
Supported websocket messages:
*/

io.on("connection", function (socket)
{
    console.log("Connection attempt");

    var user = null;

    socket.on("user_connect", function (id)
    {
        try
        {
            socket.emit("server_usr_list", null); //only sends to this socket
            io.emit("server_new_user_login", null); // send to all connected sockets
        }
        catch (e)
        {
            console.log(e.message + " " + e.stack);
        }
    });
    socket.on("client2serverMessage", function(userName, messageContent){
        console.log("got message from " + userName + ": " + messageContent);
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

app.use(express.static('static', {
    "maxAge": 24 * 60 * 60 * 1000 // 1 day in milliseconds
}));

http.listen(8080, "0.0.0.0");

console.log("Server running");
