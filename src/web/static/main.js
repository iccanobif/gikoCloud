$(function ()
{
	let gikoClient = new GikoClient();
	let btnSend = document.getElementById("btnSend");
	let txtUsername = document.getElementById("txtUsername");
	let txtMessage = document.getElementById("txtMessage");
	btnSend.addEventListener("click", function ()
	{
		gikoClient.sendMessage(txtUsername.value, txtMessage.value);
	});
}
);
