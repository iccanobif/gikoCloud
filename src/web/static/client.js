var GikoClient = function()
{
    let socket = io();

    this.sendMessage = function(userName, messageContent)
    {
        socket.emit("client2serverMessage", userName, messageContent);
    };
}