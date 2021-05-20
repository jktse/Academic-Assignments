"use strict";
// Client-side interactions with the browser.

// Make connection to server when web page is fully loaded.
var socket = io.connect();

$(document).ready(function() {
	document.querySelector('#modeid').innerHTML = "None";
	sendPrimeCommand("update volume");
	sendPrimeCommand("update tempo");
	sendPrimeCommand("update uptime");
	
	window.setInterval(function(){
		updateUpTime();
	}, 1000);


	window.setInterval(function(){
			sendPrimeCommand("update volume");
			sendPrimeCommand("update tempo");
			sendPrimeCommand("update mode");
		}, 500);


	$('#modeNone').click(function(){
		sendPrimeCommand("mode none");
		document.querySelector('#modeid').innerHTML = "None";
	});
	$('#modeRock1').click(function(){
		sendPrimeCommand("mode rock 1");
		document.querySelector('#modeid').innerHTML = "Rock 1";
	});
	$('#modeRock2').click(function(){
		sendPrimeCommand("mode rock 2");
		document.querySelector('#modeid').innerHTML = "Rock 2";
	});
	$('#modeJackyBeat').click(function(){
		sendPrimeCommand("mode Jacky beat");
		document.querySelector('#modeid').innerHTML = "Jacky's Beat";
	});
	$('#modeKennethBeat').click(function(){
		sendPrimeCommand("mode Kenenth beat");
		document.querySelector('#modeid').innerHTML = "Kenneth's Beat";
	});

	$('#volumeDown').click(function(){
		sendPrimeCommand("volume down");
	});
	$('#volumeUp').click(function(){
		sendPrimeCommand("volume up");
	});
	$('#tempoDown').click(function(){
		sendPrimeCommand("tempo down");
	});
	$('#tempoUp').click(function(){
		sendPrimeCommand("tempo up");
	});
	$('#hitHat').click(function(){
		sendPrimeCommand("play hitHat");
	});
	$('#snare').click(function(){
		sendPrimeCommand("play snare");
	});
	$('#base').click(function(){
		sendPrimeCommand("play base");
	});

	var errorTimer = setTimeout(function() {
		$('#error-box').show();
		document.querySelector('#error-text').innerHTML = "Having trouble connecting to server...";
	}, 3000);

	socket.on('commandReply', function(result) {
		handleReplies(result);
		clearTimeout(errorTimer);
	});
	
});

function sendPrimeCommand(message) {
	socket.emit('prime', message);
};

function handleReplies(reply) {
	if(reply.toString().includes("volume")){
		var volume = reply.toString().replace( /^\D+/g, '');
		document.querySelector('#volumeid').value = volume;
		$('#error-box').hide();
	}
	else if(reply.toString().includes("tempo")){
		var tempo = reply.toString().replace( /^\D+/g, '');
		document.querySelector('#tempoid').value = tempo;
		$('#error-box').hide();

	}
	else if(reply.toString().includes("H:M:S")){
		document.querySelector('#upTimeStatus').innerHTML = reply;
		$('#error-box').hide();

	}
	else if(reply.toString().includes("mode")){
		var mode = reply.replace("mode ", '');
		document.querySelector('#modeid').innerHTML = mode;
		
	}
	else if(reply.toString().includes("Server Timeout")){
		$('#error-box').show();
		document.querySelector('#error-text').innerHTML = "Client disconnected...";
	}
}

function updateUpTime(){
	sendPrimeCommand("update uptime");
}
