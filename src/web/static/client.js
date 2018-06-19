var GikoClient = function ()
{
    let socket = io();

    this.sendMessage = function (userName, messageContent)
    {
        socket.emit("client2serverMessage", userName, messageContent);
    };

    this.setMessageHandler = function (callback)
    {
        socket.on("server2clientMessage", callback);
    };

    this.setUpdateLogHandler = function(callback)
    {
        socket.on("updateLog", callback);
    };
}