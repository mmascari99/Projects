//picks a random stock ticker and a random direction then prints to website "'ticker' will go 'direction'"
function generate(){
    var ticker = ["SPY", "MSFT", "NFLX", "AMD", "APPL", "SNAP", "TWTR", "FB", "DIS", "F", "BABA", "GE", "TSLA", "MCD", "CGC"]
    var tendies = ["up", "down"]
	  var random = Math.floor((Math.random( ) * 15)) % 15; 
	  var direction = ((Math.floor(Math.random() * 2)) %2);
    var newstring = "$" + ticker[random] + " will go " + tendies[direction];
    document.getElementById("content").innerHTML = newstring;
}	
