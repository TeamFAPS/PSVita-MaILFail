// JDR: let's roll!
window.onload = function() {
	loadUp();
};
// JDR: the usual suspects
var canvas;
var canvasReady = false;
//var audioElement;
var ctx;
var textarea;
var w = 0;
var h = 0;
const framerate = 30;

var timer;
var touches = [];

// JDR: let's setup some default and what not
var keys = [];
var keyInUse = [];
var keyboardReady = false;
var keyWidth = 48;
var keyHeight = 44;
var keyOffset = 10;
var rowOffset = 10;
var keyColorFill = "#cccccc";
var keyStrokeFill = "#ff0000";
var shiftToggle = false;
var capsToggle = false;
var caretWhere = 0;
// JDR: move the keyboard down; note this is really very specific to our use case with the PQ Labs device on the big screen
var startDrawX = 0;
var startDrawY = 50;
// JDR: for the sake of this example, we just have an object that defined the rows and keys inline
// ideally, you add the key setup + defs in an JSON file, and then load it up
// allowing multiple keyboards of any type. Not clever, just common sense really.
var myKeyboard = {"rows": [
	{"keys": [
		{"startKey": "`", "shiftKey": "~"},
		{"startKey": "1", "shiftKey": "!"},
		{"startKey": "2", "shiftKey": "@"},
		{"startKey": "3", "shiftKey": "#"},
		{"startKey": "4", "shiftKey": "$"},
		{"startKey": "5", "shiftKey": "%"},
		{"startKey": "6", "shiftKey": "^"},
		{"startKey": "7", "shiftKey": "&"},
		{"startKey": "8", "shiftKey": "*"},
		{"startKey": "9", "shiftKey": "("},
		{"startKey": "0", "shiftKey": ")"},
		{"startKey": "-", "shiftKey": "_"},
		{"startKey": "=", "shiftKey": "+"},
		{"startKey": "", "shiftKey": "DELETE", "type": 1, "w": 170}
	]},
	{"keys": [
		{"startKey": "", "shiftKey": "TAB", "type": 2, "w": 110},
		{"startKey": "q", "shiftKey": "Q"},
		{"startKey": "w", "shiftKey": "W"},
		{"startKey": "e", "shiftKey": "E"},
		{"startKey": "r", "shiftKey": "R"},
		{"startKey": "t", "shiftKey": "T"},
		{"startKey": "y", "shiftKey": "Y"},
		{"startKey": "u", "shiftKey": "U"},
		{"startKey": "i", "shiftKey": "I"},
		{"startKey": "o", "shiftKey": "O"},
		{"startKey": "p", "shiftKey": "P"},
		{"startKey": "[", "shiftKey": "{"},
		{"startKey": "]", "shiftKey": "}"},
		{"startKey": "\\", "shiftKey": "|", "w": 110}
	]},
	{"keys": [
		{"startKey": "", "shiftKey": "CAPS", "type": 3, "w": 90},
		{"startKey": "a", "shiftKey": "A"},
		{"startKey": "s", "shiftKey": "S"},
		{"startKey": "d", "shiftKey": "D"},
		{"startKey": "f", "shiftKey": "F"},
		{"startKey": "g", "shiftKey": "G"},
		{"startKey": "h", "shiftKey": "H"},
		{"startKey": "j", "shiftKey": "J"},
		{"startKey": "k", "shiftKey": "K"},
		{"startKey": "l", "shiftKey": "L"},
		{"startKey": ";", "shiftKey": ":"},
		{"startKey": "'", "shiftKey": "\""},
		{"startKey": "", "shiftKey": "ENTER", "type": 4, "w": 190}
	]},
	{"keys": [
		{"startKey": "", "shiftKey": "SHIFT", "type": 5, "w": 170},
		{"startKey": "z", "shiftKey": "Z"},
		{"startKey": "x", "shiftKey": "X"},
		{"startKey": "c", "shiftKey": "C"},
		{"startKey": "v", "shiftKey": "V"},
		{"startKey": "b", "shiftKey": "B"},
		{"startKey": "n", "shiftKey": "N"},
		{"startKey": "m", "shiftKey": "M"},
		{"startKey": ",", "shiftKey": "<"},
		{"startKey": ".", "shiftKey": ">"},
		{"startKey": "/", "shiftKey": "?"},
		{"startKey": "", "shiftKey": "SHIFT", "type": 5, "w": 170}
	]},
	{"keys": [
		{"startKey": "", "shiftKey": "SPACE", "type": 6, "w": 950}
	]}
]};

// JDR: this is a key
function Key() {
	this.x;
	this.y;
	this.w;
	this.h;
	this.key;
	this.shift;
	this.type;
	this.colorfill = null;
}

// JDR: let us add a key to the board
function addKey(x, y, w, h, key) {
	var keyMe = new Key;
	keyMe.x = x;
	keyMe.y = y;
	keyMe.w = w;
	keyMe.h = h;
	keyMe.key = key.startKey;
	keyMe.shift = key.shiftKey;
	keyMe.type = key.type;
	keys.push(keyMe);
}

// JDR: this draws a particular key on the canvas
function drawKey(context, key) {
	if (key.colorfill != null)
		context.fillStyle = key.colorfill;
	else
		context.fillStyle = keyColorFill;
	context.fillRectR(key.x, key.y, key.w, key.h);
	keyThisYo = key.key;
	if (key.key.toLowerCase() != key.shift.toLowerCase()) {
		context.font = "12px sans serif"
		context.fillStyle = "#000000";
		context.fillText(key.shift, key.x+10, key.y+20);
	} else {
		if (shiftToggle == true || capsToggle == true)
			keyThisYo = key.shift;
	}
	context.font = "16px sans serif"
	context.fillStyle = "#000000";
	context.fillText(keyThisYo, key.x+30, key.y+35);
}

// JDR: this deals with firing / removing key actions
function triggerKey(context, key, flag) {
	context.strokeStyle = keyStrokeFill;
	context.strokeRectR(key.x, key.y, key.w, key.h);
	// JDR: we are adding to the field, but let us limit the char repeat
	if (flag == 0 && !(key.key in keyInUse)) {
		keyInUse[key.key] = null;
		tmpString = textarea.value;
		if (key.type == 1) {
			// JDR: delete/backspace key
			preText = tmpString.substring(0, caretWhere-1);
			postText = tmpString.substring(caretWhere, tmpString.length);
			textarea.value = preText + "" + postText;
			textarea.focus();
			setCaretToPos(textarea, caretWhere-1);
			//audioElement.play();
		} else if (key.type == 3) { // CAPS
			if (capsToggle == true) {
				capsToggle = false;
				context.fillStyle = keyColorFill;
				context.fillRectR(key.x, key.y, key.w, key.h);
				key.colorfill = keyColorFill;
			} else {
				capsToggle = true;
				context.fillStyle = "#B1F2AB";
				key.colorfill = "#B1F2AB";
				context.fillRectR(key.x, key.y, key.w, key.h);
				context.font = "16px sans serif"
				context.fillStyle = "#000000";
				context.fillText(key.key, key.x+30, key.y+35);
			}
			//audioElement.play();
		} else if (key.type == 5) {
			key.colorfill = "#B1F2AB";
			context.fillStyle = key.colorfill;
			context.fillRectR(key.x, key.y, key.w, key.h);
			context.font = "16px sans serif"
			context.fillStyle = "#000000";
			context.fillText(key.key, key.x+30, key.y+35);
			shiftToggle = true;
			keyboardReady = false; // JDR: redraw so the keys get capitalized
			//audioElement.play();
		} else {
			if (shiftToggle == true || capsToggle == true)
				keyPlacement = key.shift;
			else
				keyPlacement = key.key;
			if (key.type == 6)
				keyPlacement = " ";
			// JDR: add a character at the specific caret position
			preText = tmpString.substring(0, caretWhere);
			postText = tmpString.substring(caretWhere, tmpString.length);
			textarea.value = preText + keyPlacement + postText;
			textarea.focus();
			setCaretToPos(textarea, caretWhere+1);
			//audioElement.play();
		}
	}
	// JDR: apparently the touch is now gone, so let us ditch that timer
	if (flag == 1 && (key.key in keyInUse)) {
		if (key.type == 5) {
			shiftToggle = false;
			key.colorfill =  "#cccccc";
			context.fillStyle = key.colorfill;
			context.fillRectR(key.x, key.y, key.w, key.h);
			context.font = "16px sans serif"
			context.fillStyle = "#000000";
			context.fillText(key.key, key.x+30, key.y+35);
		}
		delete keyInUse[key.key];
	}
	preText = textarea.value;
	postText = key.key;
	if (key.type == 6)
		postText = " ";
	else if (key.type == 2)
		postText = "\t";
	else if (key.type == 3) {
		capsToggle = !capsToggle;
		return;
	} else if (key.type == 5) {
		shiftToggle = !shiftToggle;
		return;
	} else if (key.type == 1)
		preText = preText.substring(0, preText.length-1);
	else if (shiftToggle == true || capsToggle == true)
		postText = key.shift;
	//alert(preText + postText);
	update_textarea(preText + postText);
	update_anchor(preText + postText);
	textarea.focus();
}

// JDR: draw the entire keyboard
function drawKeyboard () {
	var l = keys.length;
	for (var i = 0; i < l; i++)
		drawKey(ctx, keys[i]);
}

// JDR: determine if a key is touched
function iskeyTouched(pX, pY, flag) {
	var l = keys.length;
	for (var i = 0; i < l; i++) {
		// JDR: is the touch point we are on hit one of the existing keys?
		if ( (pY >= keys[i].y) && (pY <= keys[i].y+keys[i].h) && (pX >= keys[i].x) && (pX <= keys[i].x+keys[i].w) )
			triggerKey(ctx, keys[i], flag);
	}
}

// JDR: where the action and drawing happens
function draw() {
	// JDR: this sets the canvas full viewport
	if (canvasReady == false) {
		var nw = window.innerWidth;
		var nh = window.innerHeight;
		if ((w != nw) || (h != nh)) {
			w = nw;
			h = nh;
			canvas.style.width = w+'px';
			canvas.style.height = h+'px';
			canvas.width = w;
			canvas.height = h;
		}
		canvasReady = true;
	}
	// JDR: what, where is the keyboard!?!?!
	if (keyboardReady == false) {
		//alert("Drawing the keyboard");
		ctx.clearRect(0, 0, w, h);
		drawKeyboard();
		keyboardReady = true;
	}
	// JDR: where be it the touches
	var i, len = touches.length;
	for (i = 0; i < len; i++) {
		var touch = touches[i];
		var pX = touch.pageX;
		var pY = touch.pageY;
		// JDR: this is here because of an odd bug
		caretWhere = getCaret(textarea);
		iskeyTouched(pX, pY, 0);
	}
}

function loadUp() {
	textarea_id = 'myTextarea';
	canvas = document.getElementById('canvas');
	ctx = canvas.getContext('2d');
	textarea = document.getElementById(textarea_id);
	canvas.onselectstart = function () { return false; }
	// JDR: every keyboard needs a click clack key press sound
	//audioElement = document.createElement('audio');
	//audioElement.setAttribute('src', 'click.wav');
	//audioElement.load();
	// JDR: loop over the keyboard object and get some rows of keys
	var j, rl = myKeyboard.rows.length;
	for (j = 0; j < rl; j++) {
		// JDR: loop over the keys in a row
		var h, kl = myKeyboard.rows[j].keys.length;
		var prevkeyWidth = 0;
		for (h = 0; h < kl; h++) {
			// JDR: override the default key width if needed
			if (typeof myKeyboard.rows[j].keys[h].w === "undefined")
				tmpkeyWidth = keyWidth;
			else
				tmpkeyWidth = myKeyboard.rows[j].keys[h].w;
			myY = ((rowOffset + keyHeight) * j) + startDrawY;
			// JDR: if we are at the start, we add on the initial draw offset
			if (h == 0)
				myX = keyOffset + prevkeyWidth + startDrawX;
			else
				myX = keyOffset + prevkeyWidth;
			prevkeyWidth = myX + tmpkeyWidth;
			addKey(myX, myY, tmpkeyWidth, keyHeight, myKeyboard.rows[j].keys[h]);
		}
	}
	(function animloop(){
		requestAnimFrame(animloop);
		draw();
	})();
	canvas.addEventListener('touchend', function(event) {
		// JDR: event.changedTouches[] tells will tell us where the touch that left was last seen
		var k, lt = event.changedTouches.length;
		for (k = 0; k < lt; k++) {
			var pX = event.changedTouches[k].pageX;
			var pY = event.changedTouches[k].pageY;
			iskeyTouched(pX, pY, 1);
		}
		// JDR: redraw me
		keyboardReady = false;
	});

	canvas.addEventListener('touchmove', function(event) {
		event.preventDefault();
		touches = event.touches;
	});

	canvas.addEventListener('touchstart', function(event) {

	});
};

/**
* I heart requestAnimationFrame
*
* http://paulirish.com/2011/requestanimationframe-for-smart-animating/
*/
window.requestAnimFrame = (function(){
  return  window.requestAnimationFrame   ||
  window.webkitRequestAnimationFrame ||
  window.mozRequestAnimationFrame||
  window.oRequestAnimationFrame  ||
  window.msRequestAnimationFrame ||
  function(/* function */ callback, /* DOMElement */ element){
window.setTimeout(callback, 1000 / framerate);
  };
})();

/**
* Implement new methods on the canvas 2d context
*	>> fillRectR() draw a solid rounded rectangle
*	>> strokeRectR() draw an outline of a rounded rectangle
*
* via
* http://www.supercalifrigginawesome.com/Extending-Canvas-to-Draw-Rounded-Rectangles
*/
Object.getPrototypeOf(document.createElement('canvas').getContext('2d')).fillRectR = function(x,y,w,h,r) {
	if (typeof r === "undefined")
		r = 5;
	this.beginPath();
	this.moveTo(x + r, y);
	this.lineTo(x + w - r, y);
	this.quadraticCurveTo(x + w, y, x + w, y + r);
	this.lineTo(x + w, y + h - r);
	this.quadraticCurveTo(x + w, y + h, x + w - r, y + h);
	this.lineTo(x + r, y + h);
	this.quadraticCurveTo(x, y + h, x, y + h - r);
	this.lineTo(x, y + r);
	this.quadraticCurveTo(x, y, x + r, y);
	this.closePath();
	this.fill();
};

Object.getPrototypeOf(document.createElement('canvas').getContext('2d')).strokeRectR = function(x,y,w,h,r) {
	if (typeof r === "undefined")
		r = 5;
	this.beginPath();
	this.moveTo(x + r, y);
	this.lineTo(x + w - r, y);
	this.quadraticCurveTo(x + w, y, x + w, y + r);
	this.lineTo(x + w, y + h - r);
	this.quadraticCurveTo(x + w, y + h, x + w - r, y + h);
	this.lineTo(x + r, y + h);
	this.quadraticCurveTo(x, y + h, x, y + h - r);
	this.lineTo(x, y + r);
	this.quadraticCurveTo(x, y, x + r, y);
	this.closePath();
	this.stroke();
};

/**
* Get's the current location of the caret in a textarea or input
*
* See discussion:
* http://stackoverflow.com/questions/263743/how-to-get-cursor-position-in-textarea/
*/
function getCaret(el) {
	if (el.selectionStart)
		return el.selectionStart;
	else if (document.selection) {
		el.focus();
		var r = document.selection.createRange();
		if (r == null)
			return 0;
		var re = el.createTextRange(),
		rc = re.duplicate();
		re.moveToBookmark(r.getBookmark());
		rc.setEndPoint('EndToStart', re);
		return rc.text.length;
	}
	return 0;
}

/**
* This simply sets where we want the caret position to be in a given textarea or input
*
* See link for jQuery versions
* http://stackoverflow.com/questions/499126/jquery-set-cursor-position-in-text-area/
*/
function setSelectionRange(input, selectionStart, selectionEnd) {
	if (input.setSelectionRange) {
		input.focus();
		input.setSelectionRange(selectionStart, selectionEnd);
	} else if (input.createTextRange) {
		var range = input.createTextRange();
		range.collapse(true);
		range.moveEnd('character', selectionEnd);
		range.moveStart('character', selectionStart);
		range.select();
	}
}

function setCaretToPos (input, pos) {
	setSelectionRange(input, pos, pos);
}