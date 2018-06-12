var GikoClient = function ()
{
    let socket = io();

    this.sendMessage = function (userName, messageContent)
    {
        socket.emit("client2serverMessage", userName, messageContent);
    };

    let messageHandler = null;
    this.setMessageHandler = function (callback)
    {
        this.messageHandler = callback;
    };

    socket.on("server2clientMessage", (message) =>
    {
        if (this.messageHandler != null)
            this.messageHandler(message);
    });
}