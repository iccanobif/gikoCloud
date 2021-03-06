$(function ()
{
	let gikoClient = new GikoClient();
	let btnSend = document.getElementById("btnSend");
	let btnClearBubble = document.getElementById("btnClearBubble");
	let txtUsername = document.getElementById("txtUsername");
	let txtMessage = document.getElementById("txtMessage");
	let divLog = document.getElementById("divLog");
	let divRoomList = document.getElementById("divRoomList");

	btnSend.addEventListener("click", function ()
	{
		gikoClient.sendMessage(txtUsername.value, txtMessage.value);
	});

	btnClearBubble.addEventListener("click", function ()
	{
		gikoClient.sendMessage("", "");
	});

	let messageList = [];

	function renderMessageList()
	{
		divLog.innerHTML = messageList
			.map(x => JSON.parse(x))
			.map(x => x["playerName"] + ": " + x["message"])
			.join("<br />");
		// Scroll to bottom. Might be nice to avoid that if the log wasn't already scrolled to bottom.
		divLog.scrollTop = divLog.scrollHeight;
	};

	gikoClient.setMessageHandler((message) =>
	{
		console.log("arrivato messaggio " + message);
		messageList.push(message);
		if (messageList.length > 100)
			messageList = messageList.slice(1); // Removes the first value
		renderMessageList();
	});

	gikoClient.setUpdateLogHandler((log) =>
	{
		console.log(log);
		messageList = log;
		renderMessageList();
	});

	gikoClient.setLoginListHandler((list) =>
	{
		divRoomList.innerHTML = JSON.parse(list)
			.filter(x => x["count"] > 0) // Only rooms with at least one user
			.map(x => "<b>" + x["stageName"] + "</b>:" + x["count"])
			.join("; ");
	});
});
