let express = require('express');
let app = express();
let http = require('http').Server(app);
let fs = require('fs');
let socketio = require("socket.io")(http);
let child_process = require("child_process");
let config = require("./config.js");

function log(msg)
{
    let d = new Date();
    console.log("[" + d.toISOString() + "] " + msg);
}

function printError(e)
{
    log(e.message + " " + e.stack);
};

let messageQueue = [];

socketio.on("connection", (socket) =>
{
    log("Connection attempt");
    socket.emit("updateLog", messagesFromServer);

    socket.on("client2serverMessage", (userName, messageContent) =>
    {
        try
        {
            if (messageQueue.length > 10)
            {
                log("got message from " + userName + ": " + messageContent + ".\nBut queue is full. Oops!");
                socket.emit("server2clientMessage", "can't send message '" + messageContent + "', man, there are too many in queue.");
            }
            else
            {
                log("got message from " + userName + ": " + messageContent + ".\nAdding to queue.");
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

// Loop for sending queued messaged to gikopoi
setInterval(() =>
{
    try
    {
        let messageToSend = messageQueue.shift();
        if (messageToSend == undefined)
            return;

        messageToSend = messageToSend.replace(/\n/g, "");

        log("Trying to send " + messageToSend + " to backend");
        backendProcess.stdin.write("msg " + messageToSend + "\n");
    }
    catch (e)
    {
        printError(e);
    }
}, 1000);

//Loop for updating #list
setInterval(() =>
{
    try
    {
        log("Sending to server list request");
        backendProcess.stdin.write("list\n");
    }
    catch (e)
    {
        printError(e);
    }
}, 10000);

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

let messagesFromServer = [];

function gotMessageFromGikopoi(msg)
{
    // TODO: put all messages in a log file, and in a buffer in memory, so that
    // new users can get at least a few lines of log as soon as they "log in"

    log("Message from backend: " + msg);
    messagesFromServer.push(msg);
    socketio.emit("server2clientMessage", msg);

    if (messagesFromServer.length > 100)
    {
        messagesFromServer.shift();
    }
}

// I bet there's a much better SICP-y way of doing this...
let parameters = [];
[["-u", "userName"], ["-s", "server"], ["-r", "room"], ["-p", "proxy"]].map((x) =>
{
    let val = config.getParameter(x[1]);
    if (val != undefined) 
    {
        parameters.push(x[0]);
        parameters.push(val);
    }
});

var backendProcess = child_process.spawn(config.getParameter("cli_path"), parameters);

(function ()
{
    let readBuffer = "";
    backendProcess.stdout.on("data", (data) =>
    {
        // I can't be sure that "data" has a complete line, so I have to fill a buffer and process a line only 
        // when I actually see a \n in it.
        readBuffer += data;
        while (readBuffer.match(/\n/))
        {
            let line = readBuffer.slice(0, readBuffer.search(/\n/));
            readBuffer = readBuffer.slice(readBuffer.search(/\n/) + 1);

            if (line.startsWith("MSG "))
            {
                // this message made gikocloud.js print some weird stuff
                // {"user": "1", "message": "7/31 8/1 8:00 宴会場 「第１１回夏祭りギコSUMMER!」 ついに１０周年！詳細は#ｲﾍﾞﾝﾄ！ "}
                gotMessageFromGikopoi(line.substring(4));
            }
            else if (line.startsWith("#LIST "))
            {
                log("Got #list from backend");
                socketio.emit("#list", line.substring("#LIST ".length));
            }
            else
            {
                log("Got weird message from cli: " + line);
            }
        }
    });
})();


backendProcess.stderr.on("data", (data) =>
{
    log("[cli] (stderr): " + String(data).trim());
});

backendProcess.on("exit", (code, signal) =>
{
    log("Backend process died...");
    //TODO Restart the process
});

http.listen(8080, "0.0.0.0");

log("Server running");