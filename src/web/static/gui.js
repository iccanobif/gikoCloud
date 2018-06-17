$(function ()
{
	let gikoClient = new GikoClient();
	let btnSend = document.getElementById("btnSend");
	let btnClearBubble = document.getElementById("btnClearBubble");
	let txtUsername = document.getElementById("txtUsername");
	let txtMessage = document.getElementById("txtMessage");
	let divLog = document.getElementById("divLog");

	btnSend.addEventListener("click", function ()
	{
		gikoClient.sendMessage(txtUsername.value, txtMessage.value);
	});

	btnClearBubble.addEventListener("click", function ()
	{
		gikoClient.sendMessage("", "");
	});

	let messageList = [];

	gikoClient.setMessageHandler((message) =>
	{
		console.log("arrivato messaggio " + message);
		messageList.push(message);
		if (messageList.length > 10)
			messageList = messageList.slice(1); // Removes the first value
		divLog.innerHTML = messageList.join("<br />");
		// Scroll to bottom. Might be nice to avoid that if the log wasn't already scrolled to bottom.
		divLog.scrollTop = divLog.scrollHeight;
	});
}
);
