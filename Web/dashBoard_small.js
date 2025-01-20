//change filname to index.js
//change urlPort to http://IP.address:8080
var urlPort = "http://localhost:8080";


//MAIN PAGE setup
var leftPanels = ["scrollText", "stopWatch", "countdownP", "optionsP"];
var leftPanelButtons = ["scrollTextB", "stopWatchB", "countdownB", "optionsB"];

function reloadPage(){
	window.location.reload();
    clearRightPanel();
};

function clearRightPanel(){
	for(i = 0; i < leftPanels.length; i++){
		document.getElementById(leftPanels[i]).style.visibility = "hidden";
	};
};
clearRightPanel();

function resetButtonColor(){
	for(i = 0; i < leftPanelButtons.length; i++){
		document.getElementById(leftPanelButtons[i]).style.backgroundColor = "#ed766d";  //right buttons default color
	};
};
resetButtonColor();

function chooseRightPanel(divID, btnID){
	clearRightPanel();
	resetButtonColor();
	document.getElementById(divID).style.visibility = "visible";
	document.getElementById(btnID).style.backgroundColor = "#7a7d23";   //right buttons colour when pressed
};

//AUTO ARRAY setup
var currentArrayPosition = 0;
var displayAssistOnOff = true; //called on page load and switched to false
var intervalDelay = 5000;
var autoScrollArray;
var currentAutoArray;
var currentInterval;
var countDownTimer = "2:00";
const possibleCountdown = ["countDown1", "countDown2", "countDown3", "countDown4", "countDown5"];

var satSchedule = [
  '[ 2:00pm MenShot Final]}',
  '[ 9:00am Men LJ Final]}',
  '[ 10:00 Wom  LJ Final]}'
];

var sunSchedule = [
  '[a2:00pm Mens Shotput Final]{',
  '[a9:00am Mens Long Jump Final]{',
  '[a10:00am Womens Long Jump Final]{'
];

var countDownDate = new Date();
function setAutoArray(array){//kevinkevinkevin
    //intervalDelay = document.getElementById("autoDelay").value;
	if(array == 'DisplayTime'){
		intervalDelay = 2000;
		return array;
	}
	else if(array == 'satSchedule'){
		intervalDelay = 5000;
		return satSchedule;
	}
	else if(array == 'sunSchedule'){
		intervalDelay = 8000;
		return sunSchedule;
	}
	else if(array == 'countDown1'){
		intervalDelay = 1000;
		countDownDate = new Date().getTime() + (60000 + 2000);
		return array;
	}
	else if(array == 'countDown2'){
		intervalDelay = 1000;
		countDownDate = new Date().getTime() + (120000 + 2000);
		return array;
	}
	else if(array == 'countDown3'){
		intervalDelay = 1000;
		countDownDate = new Date().getTime() + (180000 + 2000);
		return array;
	}
	else if(array == 'countDown4'){
		intervalDelay = 1000;
		countDownDate = new Date().getTime() + (240000 + 2000);
		return array;
	}
	else if(array == 'countDown5'){
		intervalDelay = 1000;
		countDownDate = new Date().getTime() + (300000 + 2000);
		return array;
	}
	else{
		array = 'DisplayTime';
		intervalDelay = 10000;
		return array;
	};
};

function toggleDisplayAssist(){
	displayAssistOnOff = !displayAssistOnOff;
	currentAutoArray = document.getElementById("autoDropdown").value;
	autoScrollArray = setAutoArray(currentAutoArray);
	
	if(displayAssistOnOff){
		postColor('*');
		document.getElementById("displayAssist").style.backgroundColor = "#26ab90";  //when time is on
		document.getElementById("on").style.visibility = "visible";
		document.getElementById("off").style.visibility = "hidden";
		startDisplayAssist(intervalDelay);
	}
	else{
		document.getElementById("displayAssist").style.backgroundColor = "grey";    //when time is off
		document.getElementById("on").style.visibility = "hidden";
		document.getElementById("off").style.visibility = "visible";
		stopDisplayAssist();
	}
};
toggleDisplayAssist();

var rightNow = new Date();
function sendDisplayAssist(){	//kevinkevinkevin
    var displaySelect = 0;
	var xhr = new XMLHttpRequest();
	xhr.open('POST', urlPort + '/display/assist', true)	
	if(Object.prototype.toString.call(autoScrollArray) === '[object Array]') {
		xhr.send(displaySelect + autoScrollArray[currentArrayPosition]);
	}
	else if(autoScrollArray  == 'DisplayTime'){
		var today = new Date();
		var hours = today.getHours();
		var amPM = 't';
		if(hours > 11){
			amPM = 'T';
		}
		else{
			amPM = 't';
		}
		hours = hours % 12;
		if(hours == 0){
			hours = 12;
		}
		var mins = today.getMinutes();
		
		if (hours < 10){
			hours = " " + hours;
		}
		if (today.getMinutes() < 10){
			mins = "0" + today.getMinutes();
		}
		var time = hours + ":" + mins;
		xhr.send(displaySelect + "[" + amPM + time + "]U");
	}
	else if(possibleCountdown.includes(autoScrollArray)){
		rightNow = new Date().getTime();
		var difference = countDownDate - rightNow;
		
		var cdMin = Math.floor((difference % (1000 * 60 * 60)) / (1000 * 60));
		var cdSec = Math.floor((difference % (1000 * 60)) / 1000);
		
		if(difference > 0){
			if(cdSec < 10){
				xhr.send(displaySelect + "[ " + cdMin + ":0" + cdSec + "]|");
			}
			else{
				xhr.send(displaySelect + "[ " + cdMin + ":" + cdSec + "]|");
			}
			
		}
		else{
			xhr.send(displaySelect + "[ " + "0:00" + "]|");
		}
	}
	else{
		alert('error');
	}
	
	currentArrayPosition++;
	
	if(currentArrayPosition >= autoScrollArray.length){
		currentArrayPosition = 0;
	};
};

function startDisplayAssist(interval) {
  currentInterval = setInterval(sendDisplayAssist, interval);
};

function stopDisplayAssist() {
  clearInterval(currentInterval);
};

//kevinkevinkevin
function loadCountdown() {
  currentAutoArray = document.getElementById("autoDropdown").value;
  
  if(currentAutoArray == "countDown1"){
	  countDownTimer = "1:00";
  }
  else if(currentAutoArray == "countDown2"){
	  countDownTimer = "2:00";
  }
  else if(currentAutoArray == "countDown3"){
	  countDownTimer = "3:00";
  }
  else if(currentAutoArray == "countDown4"){
	  countDownTimer = "4:00";
  }
  else if(currentAutoArray == "countDown5"){
	  countDownTimer = "5:00";
  }
  else{
	  countDownTimer = "N/A";
  }
  
  var dataToSend = JSON.stringify({"command": "staticBig", "data": countDownTimer, "displaySelect": 0});
	
	var xhr = new XMLHttpRequest();	
	xhr.open('POST', urlPort + '/dashboard/post', true);	
	xhr.send(dataToSend);
  
  //console.log(countDownTimer)
};

function selectTickerDisplay() {
	var value = parseFloat(document.getElementById("tickerDisplay").value);
		
	var xhr = new XMLHttpRequest();
    var dataToSend = JSON.stringify({"command": "tickerDisplay", "data": value});
	xhr.open('POST', urlPort + '/server/command', true)	
	xhr.send(dataToSend);
};

function selectResultsDisplay() {
	var value = parseFloat(document.getElementById("resultsDisplay").value);
		
	var xhr = new XMLHttpRequest();
    var dataToSend = JSON.stringify({"command": "resultsDisplay", "data": value});
	xhr.open('POST', urlPort + '/server/command', true)	
	xhr.send(dataToSend);
};

var nameMode = ' ';
function nextNameMode() {
	if(nameMode == ' '){
		nameMode = 1;
	}
	else{
		nameMode = ' ';
	}
	var xhr = new XMLHttpRequest();
    var dataToSend = JSON.stringify({"command": "nameMode", "data": nameMode});
	xhr.open('POST', urlPort + '/server/command', true)	
	xhr.send(dataToSend);
	console.log(dataToSend);
};

function testSerial() {
	var xhr = new XMLHttpRequest();
	var dataToSend = "nameMode[ F.LastName1st  Rnd.1]<=[1First NameLast Name ]<";
	xhr.open('POST', urlPort + '/serial/post', true)	
	xhr.send(dataToSend);
	console.log(dataToSend);
};

function postColor(charValue) {
	//var displaySelect = parseFloat(document.getElementById("displaySelect").value);
	var displaySelect = 0;
	var xhr = new XMLHttpRequest();
    var dataToSend = JSON.stringify({"command": "colorChange", "data": charValue, "displaySelect": displaySelect});
	xhr.open('POST', urlPort + '/dashboard/post', true)	
	xhr.send(dataToSend);
};

var tickerStatus = 0;
function changeTickerStatusDisplay() {//kevinkevin	
	if(tickerStatus){
		document.getElementById("tickerStatus").style.backgroundColor = "green";
		document.getElementById("t_on").style.visibility = "visible";
		document.getElementById("t_off").style.visibility = "hidden";
		tickerStatus = 1;
	}
	else{
		document.getElementById("tickerStatus").style.backgroundColor = "grey";
		document.getElementById("t_on").style.visibility = "hidden";
		document.getElementById("t_off").style.visibility = "visible";
		stopDisplayAssist();
		tickerStatus = 0;
	}
};
//changeTickerStatusDisplay();

function changeTickerStatus() {//kevinkevin	
	//changeTickerStatusDisplay();
	var xhr = new XMLHttpRequest();
    var dataToSend = JSON.stringify({"command": "changeTicker", "data": 1});
	xhr.open('POST', urlPort + '/dashboard/post', true)	
	console.log(dataToSend);
	xhr.send(dataToSend);
};


//TEXT Functions
//flash to display
function postBigText() {
	var displaySelect = 0;
	var valueEntered = document.getElementById("bigTextInput").value;
	var dataToSend = JSON.stringify({"command": "staticBig", "data": valueEntered, "displaySelect": displaySelect});
	
	var xhr = new XMLHttpRequest();	
	xhr.open('POST', urlPort + '/dashboard/post', true);	
	xhr.send(dataToSend);
};

function postStaticText() {
	var displaySelect = 0;
	var topRow = document.getElementById("staticTextInput1").value;
	var bottomRow = document.getElementById("staticTextInput2").value;
    var dataToSend = JSON.stringify({"command": "staticText", "data": topRow + bottomRow, "displaySelect": displaySelect});
	
	var xhr = new XMLHttpRequest();	
	xhr.open('POST', urlPort + '/dashboard/post', true);	
	xhr.send(dataToSend);
};

//scroll and pause
function postPauseText(flag) {
	var displaySelect = 0;
	var textEntered;
	var xhr = new XMLHttpRequest();	
	if (flag == 'scroll1' || flag == 'pause1' || flag == 'static1'){
		textEntered = document.getElementById("staticTextInput1").value;
	}
	else if (flag == 'scroll1W' || flag == 'pause1W' || flag == 'static1W'){
		textEntered = document.getElementById("staticTextInput1w").value;
	}
	else if (flag == 'scroll2' || flag == 'pause2' || flag == 'static2'){
		textEntered = document.getElementById("staticTextInput2").value;
	}
	else if (flag == 'scroll2W' || flag == 'pause2W' || flag == 'static2W'){
		textEntered = document.getElementById("staticTextInput2w").value;
	}
	else if (flag == 'scroll3' || flag == 'pause3' || flag == 'static3'){
		textEntered = document.getElementById("staticTextInput3").value;
	}
	else if (flag == 'scroll3W' || flag == 'pause3W' || flag == 'static3W'){
		textEntered = document.getElementById("staticTextInput3w").value;
	}
	else if (flag == 'scroll4' || flag == 'pause4' || flag == 'static4'){
		textEntered = document.getElementById("staticTextInput4").value;
	}
	else if (flag == 'scroll4W' || flag == 'pause4W' || flag == 'static4W'){
		textEntered = document.getElementById("staticTextInput4w").value;
	}
	else if (flag == 'scrollT' || flag == 'pauseT' || flag == 'staticT'){
		textEntered = document.getElementById("staticTextInputT").value;
	}
	else if (flag == 'scrollTw' || flag == 'pauseTw' || flag == 'staticTw'){
		textEntered = document.getElementById("staticTextInputTw").value;
	}
	else if (flag == 'scrollB' || flag == 'pauseB' || flag == 'staticB'){
		textEntered = document.getElementById("staticTextInputB").value;
	}
	else if (flag == 'scrollBw' || flag == 'pauseBw' || flag == 'staticBw'){
		textEntered = document.getElementById("staticTextInputBw").value;
	}
	else if (flag == 'scrollBig' || flag == 'pauseBig' || flag == 'staticBig'){
		textEntered = document.getElementById("staticTextInputBig").value;
	}
	else if (flag == 'scrollBigW' || flag == 'pauseBigW' || flag == 'staticBigW'){
		textEntered = document.getElementById("staticTextInputBigW").value;
	}
	else{
		textEntered = document.getElementById("staticTextInput1").value;
	}
	
    var dataToSend = JSON.stringify({"command": flag, "data": textEntered, "displaySelect": displaySelect});
	xhr.open('POST', urlPort + '/dashboard/post', true);	
	xhr.send(dataToSend);
};

function postScrollText(flag) {
	var displaySelect = 0;
	var textEntered = document.getElementById("textInput").value;
	var xhr = new XMLHttpRequest();
	console.log(displaySelect);
    var dataToSend = JSON.stringify({"command": flag, "data": textEntered, "displaySelect": displaySelect});
	xhr.open('POST', urlPort + '/dashboard/post', true);	
	xhr.send(dataToSend);
};


//COUNTDown setup  //kevinkevin
function displayCountdown(){	
    var countdownVal = document.getElementById("countdown").value;
	var displaySelect = 0;
	var xhr = new XMLHttpRequest();	
	var data = 'X' + countdownVal;
	var dataToSend = JSON.stringify({"command": "postRaw", "data": data, "displaySelect": displaySelect});
	xhr.open('POST', urlPort + '/dashboard/post', true);	
	xhr.send(dataToSend);
	console.log(dataToSend);
};

function showCountdown(){	
	var displaySelect = 0;
	var xhr = new XMLHttpRequest();	
	var dataToSend = JSON.stringify({"command": "postRaw", "data": 'W', "displaySelect": displaySelect});
	xhr.open('POST', urlPort + '/dashboard/post', true);	
	xhr.send(dataToSend);
};

function startCountdown(){	
	var displaySelect = 0;
	var xhr = new XMLHttpRequest();	
	var dataToSend = JSON.stringify({"command": "postRaw", "data": 'Y', "displaySelect": displaySelect});
	xhr.open('POST', urlPort + '/dashboard/post', true);	
	xhr.send(dataToSend);
};

function pauseCountdown(){	
	var displaySelect = 0;
	var xhr = new XMLHttpRequest();	
	var dataToSend = JSON.stringify({"command": "postRaw", "data": 'Z', "displaySelect": displaySelect});
	xhr.open('POST', urlPort + '/dashboard/post', true);	
	xhr.send(dataToSend);
};

//STOP WATCH setup
var manualTimeAdjust = 0.00;
var defaultTimeOffset = 0.50;
var handTimeAdjust = manualTimeAdjust + defaultTimeOffset;

function displayStopWatch(){	
	var displaySelect = 0;
	var xhr = new XMLHttpRequest();	
	xhr.open('POST', urlPort + '/timer/post', true);	
	xhr.send(displaySelect + '~');
};

function startStopWatch(){	
	var displaySelect = 0;
	var xhr = new XMLHttpRequest();	
	xhr.open('POST', urlPort + '/timer/post', true);	
	xhr.send(displaySelect + '#');
};

//kevinkevin
function stopStopWatch(time){	
	var displaySelect = 0;
	var xhr = new XMLHttpRequest();
	xhr.open('POST', urlPort + '/timer/post', true);		
	xhr.send(displaySelect + '&');  //STOP
};	
	

//CLIENT SIDE STOPWATCH	
function getStopWatchTime(){	
	if(minutes < 10){
		minutesAsString = "0" + minutes.toString();
	}
	else{
		minutesAsString = minutes.toString();
	};
	
	if(seconds < 10){
		secondsAsString = "0" + seconds.toString();
	}
	else{
		secondsAsString = seconds.toString();
	};
	
	if(milliseconds < 10){
		millisecondsAsString = "0" + milliseconds.toString();
	}
	else{
		millisecondsAsString = milliseconds.toString();
	};
	
	return minutesAsString + secondsAsString + millisecondsAsString;
};

function setTime() {
    h1.textContent = currentMins + ":" + currentSeconds + "." + currentMilliseconds
};

//timing variables
var currentMins = "00";
var currentSeconds = "00";
var currentMilliseconds = "00";

var h1 = document.getElementsByTagName('h1')[0],
    start = document.getElementById('start'),
    stop = document.getElementById('stop'),
    clear = document.getElementById('clear'),
    seconds = 0, minutes = 0, milliseconds = 0,
    t;

var startTime;
var currentTime;
var previousTime;
var timeControl = 0;
var msAsString = '0';
var timeValues;
var timeArray;

//convert system ms to a readable time
function msToTime(duration) {
  var ms = parseInt((duration % 1000) / 100),
    s = Math.floor((duration / 1000) % 60),
    m = Math.floor((duration / (1000 * 60)) % 60),
	timeArray = [m,s,ms];
	
  return timeArray;
};

//add a millisecond for display purposes
function add() {
    milliseconds++;
	//correct display time every second
    if (milliseconds >= 100) {
		currentTime = new Date().getTime();
		timeValues = msToTime(currentTime - startTime);
		
		minutes = timeValues[0];
		seconds = timeValues[1];
		milliseconds = timeValues[2];
    };
    
	//format and display for the webpage
    h1.textContent = (minutes ? (minutes > 9 ? minutes : "0" + minutes) : "00") + ":" + (seconds > 9 ? seconds : "0" + seconds) + "." + (milliseconds ? (milliseconds > 9 ? milliseconds : "0" + milliseconds) : "00");

	//recall timer function
    timer();
};

//
function timer() {
	//if time should be cleared
	if(timeControl == 0){		
		startTime= new Date().getTime();
		startTime=startTime - handTimeAdjust*1000;
		previousTime = startTime;
		timeControl ++;
	};	
	//timeout
    t = setTimeout(add, 10);
};


/* Start button */
start.onclick = function(){
	timer();
	startStopWatch();
};

/* Stop button */
stop.onclick = function() {
    clearTimeout(t);
	stopStopWatch(getStopWatchTime());
};

/* Clear button */
clear.onclick = function() {
    h1.textContent = currentMins + ":" + currentSeconds + "." + currentMilliseconds
    seconds = 0; minutes = 0; hours = 0;
	timeControl =0;
	milliseconds = 0;
	displayStopWatch();
};

 



