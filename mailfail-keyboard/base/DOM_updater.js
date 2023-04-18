function update_textarea(path) {
	var element = document.getElementById("myTextarea");
	element.value = path;
}
function update_anchor(path) {
	var element = document.getElementById("myAnchor");
	element.href = "email:send?attach="+path+".";
	element.innerHTML = "Dump "+path;
}
var area = document.getElementById("myTextarea");
if (area.addEventListener) {
	area.addEventListener('input', function() {
		// event handling code for sane browsers
		update_anchor(area.value);
	}, false);
} else if (area.attachEvent) {
	area.attachEvent('onpropertychange', function() {
		// IE-specific event handling code
		update_anchor(area.value);
	});
}