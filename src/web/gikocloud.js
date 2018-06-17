let express = require('express');
let app = express();
let http = require('http').Server(app);
let fs = require('fs');
let socketio = require("socket.io")(http);
let child_process = require("child_process");
let config = require("./config.js");

function printError(e)
{
    console.log(e.message + " " + e.stack);
};

let messageQueue = [];

socketio.on("connection", (socket) =>
{
    console.log("Connection attempt");
    socket.on("client2serverMessage", (userName, messageContent) =>
    {
        try
        {
            if (messageQueue.length > 10)
            {
                console.log("got message from " + userName + ": " + messageContent + ".\nBut queue is full. Oops!");
                socket.emit("server2clientMessage", "can't send message '" + messageContent + "', man, there are too many in queue.");
            }
            else
            {
                console.log("got message from " + userName + ": " + messageContent + ".\nAdding to queue.");
                if (userName == "" && messageContent == "")
                    messageQueue.push("");    // Clears bubble
                else
                    messageQueue.push(userName + ": " + messageContent);
            }
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

        messageToSend = messageToSend.replace(/\n/g, "");

        console.log("Trying to send " + messageToSend + " to backend");
        backendProcess.stdin.write("msg " + messageToSend + "\n");
    }
    catch (e)
    {
        printError(e);
    }
}, 1000);

app.get("/", (req, res) =>
{
    try
    {
        res.writeHead(200, { 'Content-Type': 'text/html' });
        fs.readFile("static/home.htm", (err, data) =>
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
app.use(express.static('static', {
    "maxAge": 24 * 60 * 60 * 1000 // 1 day in milliseconds
}));

function gotMessageFromGikopoi(msg)
{
    // TODO: put all messages in a log file, and in a buffer in memory, so that
    // new users can get at least a few lines of log as soon as they "log in"

    
    console.log("Message from backend: " + msg);
    socketio.emit("server2clientMessage", msg);
}

var backendProcess = child_process.spawn(config.getParameter("cli_path"));
backendProcess.stdout.on("data", (data) =>
{
    String(data).trim().split("\n").forEach((line) =>
    {
        if (line.startsWith("MSG "))
        {
            // this message made gikocloud.js print some weird stuff
            // {"user": "1", "message": "7/31 8/1 8:00 宴会場 「第１１回夏祭りギコSUMMER!」 ついに１０周年！詳細は#ｲﾍﾞﾝﾄ！ "}
            gotMessageFromGikopoi(line.substring(4));
        }
        else
        {
            console.log("Got weird message from cli: " + line);
        }
    });
});
backendProcess.stderr.on("data", (data) =>
{
    console.log("[cli] (stderr): " + String(data).trim());
});

http.listen(8080, "0.0.0.0");

console.log("Server running");