var express = require('express');
var app = express();
var http = require('http').Server(app);
var fs = require('fs');
var socketio = require("socket.io")(http);
var child_process = require("child_process");

function printError(e)
{
    console.log(e.message + " " + e.stack);
};

let messageQueue = [];

socketio.on("connection", function (socket)
{
    console.log("Connection attempt");

    var user = null;

    socket.on("client2serverMessage", function (userName, messageContent)
    {
        try
        {
            console.log("got message from " + userName + ": " + messageContent + ". Adding to queue.");

            if (messageQueue.length > 10)
                socket.emit("server2clientMessage", "can't send message '" + messageContent + "', man, there are too many in queue.");
            else
                messageQueue.push(userName + ": " + messageContent);
        }
        catch (e)
        {
            printError(e);
        }
    });
});

setInterval(() =>
{
    try
    {
        let messageToSend = messageQueue.shift();
        if (messageToSend == undefined)
            return;

            // backendProcess.stdin.write("msg " + messageToSend);
        socketio.emit("server2clientMessage", messageToSend); // send to all connected sockets
    }
    catch (e)
    {
        printError(e);
    }

}, 1000);

app.get("/", function (req, res)
{
    try
    {
        res.writeHead(200, { 'Content-Type': 'text/html' });
        fs.readFile("static/home.htm", function (err, data)
        {
            if (err) res.end(err);
            else res.end(data);
        });
    }
    catch (e)
    {
        printError(e);
    }
});

// let i = 0;
// setInterval(() => {
//     console.log("trying to send message to clients");
//     socketio.emit("server2clientMessage", "suck my dicko " + i++); // send to all connected sockets
// }, 1000);



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