/*
The MIT License (MIT)

Copyright (c) 2013 Michael Cordingley

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
(function(){'use strict';var o=Math.PI,floor=Math.floor,sin=Math.sin,asin=Math.asin,cos=Math.cos,acos=Math.acos,tan=Math.tan,atan=Math.atan;Date.prototype.sunrise=function(a,b){return p(true,a,b,this);};Date.prototype.sunset=function(a,b){return p(false,a,b,this);};var p,mod,mod24,mod360,degToRad,radToDeg,degSin,degASin,degCos,degACos,degTan,degATan;p=function(a,b,c,d,e){b=b||0;c=c||0;d=d||new Date();e=e||90.5;var f=d.getFullYear(),month=(d.getMonth()+1),day=(d.getDate()+1);var g=floor(275*month/9),N2=floor((month+9)/12),N3=(1+floor((f-4*floor(f/4)+2)/3)),N=g-(N2*N3)+day-30;var h=c/15;var t;if(a){t=N+((6-h)/24);}else{t=N+((18-h)/24);}var M=(0.9856*t)-3.289;var L=M+(1.916*degSin(M))+(0.020*degSin(2*M))+282.634;L=mod360(L);var i=degATan(0.91764*degTan(L));i=mod360(i);var j=(floor(L/90))*90,RAquadrant=(floor(i/90))*90;i=i+(j-RAquadrant);i=i/15;var k=0.39782*degSin(L),cosDec=degCos(degASin(k));var l=(degCos(e)-(k*degSin(b)))/(cosDec*degCos(b));if(l>1){return Number.NEGATIVE_INFINITY;}else if(l<-1){return Number.POSITIVE_INFINITY;}var H;if(a){H=360-degACos(l);}else{H=degACos(l);}H=H/15;var T=H+i-(0.06571*t)-6.622;var m=T-h;m=mod24(m);var n=new Date(d),hours=floor(m),minutes=floor((m-hours)*60),seconds=floor((m-hours-(minutes/60))*3600);n.setUTCHours(hours,minutes,seconds);return n;};mod=function(b){return function(a){while(a<0){a+=b;}return a%b;};};mod24=mod(24);mod360=mod(360);degSin=function(a){return sin(degToRad(a));};degASin=function(a){return radToDeg(asin(a));};degCos=function(a){return cos(degToRad(a));};degACos=function(a){return radToDeg(acos(a));};degTan=function(a){return tan(degToRad(a));};degATan=function(a){return radToDeg(atan(a));};degToRad=function(a){return a*o/180;};radToDeg=function(a){return a*180/o;};})();

//LOAD STORED DATA-------------------------------------------------
var s_Temp = window.localStorage.getItem("temp") || "\0";
var s_Wind = window.localStorage.getItem("wind") || "\0";
var s_Icon = window.localStorage.getItem("icon") || "\0";
var s_Place = window.localStorage.getItem("place") || "\0";
var s_Humidity = window.localStorage.getItem("humidity") || "\0";
var setTemp = window.localStorage.getItem("t_set") || "1";
var setWind = window.localStorage.getItem("w_set") || "1";



//TAKE CHANGES FROM CONFIGURATION PANEL
function configClosed(e) {
	console.log("Configuration window returned: " + e.response);
    var configuration = e.response.split(",");
    
	if(configuration[0] !== "") {
		if(configuration[0] != setTemp || configuration[1] != setWind) {
            console.log("temp: " + configuration[0]);
            console.log("wind: " + configuration[1]);
			setTemp = configuration[0];
            setWind = configuration[1];
			window.localStorage.setItem("t_set", setTemp);
            window.localStorage.setItem("w_set", setWind);
            
			Pebble.sendAppMessage({ "status":   "configUpdated" });
		}
	}
    
	else console.log("No options returned\n");
}

//RUN CONFIGURATION PANEL
function showConfigWindow(e) {
	var url	=	"http://students.uniparthenope.it/~0108000755/index.html?" + "temp=" + setTemp + "&wind=" + setWind ;
	console.log("Showing config page: " + url);
	Pebble.openURL(url);
}

//RETURN AN ICON CHOSEN ACCORDING TO RAIN, CLOUDS AND LOCATION
function findIcon(location, clf, crh)
{
	var icon;
	var date=new Date();
	var time=date.getTime();
	var sunset=date.sunset(location.coords.latitude, location.coords.longitude).getTime();
	var sunrise=date.sunrise(location.coords.latitude, location.coords.longitude).getTime();
	if (crh < 0.11) {
      if (clf < 12.5) {
        icon='01';
      } 
      else if (clf < 37.5) {
        icon='02';
      } 
      else if (clf < 62.5) {
        icon='03';
      } 
      else if (clf < 87.5) {
        icon='04';
      } 
      else {
        icon='04';
      }
    } 
    else if (crh < 2) {
      icon='09';
    } 
    else if (crh < 10) {
      icon='10';
    } 
    else {
      icon='11';
    }
    
    if(sunrise<time && time<sunset){
        icon=""+icon+"d";
        console.log("giorno");
    }
    else {
        icon=""+icon+"n";
        console.log("notte");
    }
    
	console.log("ora : " + time);
	console.log("sunset : " + sunset );
	console.log("sunrise : " + sunrise );
	return icon;
}

//TEMP CONVERTER
function confTemp(celsius)
{
    if(setTemp==1)
        return ""+ parseInt(celsius) + "\u00B0" + "C";
    else
        return ""+ parseInt((celsius*9/5)+32) + "\u00B0" + "F";
}

//SPEED CONVERTER
function setSpeed(speed)
{
    console.log("--------------------------->" + setWind);
    switch(setWind)
    {
        case "1":
            return "" + parseInt(speed * 1.852) + " km/h ";
        case "2":
            
            return "" + parseInt(speed * 0.5144) + " m/s ";
        case "3":
            return "" + parseInt(speed * 1.1508) + " mph ";
        case "4":
            return "" + parseInt(speed) + " kn ";
    }
}

//WIND DIRECTION FINDER
function findDirection(p_Wd10)
{
    console.log("gradi:"+ p_Wd10);
    if (p_Wd10 >= 0 && p_Wd10 < 33.75) return "N";
    else if (p_Wd10 >= 33.75 && p_Wd10 < 78.75) return "NE";
    else if (p_Wd10 >= 78.75 && p_Wd10 < 123.75) return "E";
    else if (p_Wd10 >= 123.75 && p_Wd10 < 168.75) return "SE";
    else if (p_Wd10 >= 168.75 && p_Wd10 < 213.75) return "S";
    else if (p_Wd10 >= 213.75 && p_Wd10 < 258.75) return "SW";
    else if (p_Wd10 >= 258.75 && p_Wd10 < 303.75) return "W";
    else if (p_Wd10 >= 303.75 && p_Wd10 < 348.75) return "NW";
    else if (p_Wd10 >= 348.75 && p_Wd10 < 360) return "N";
    return -1;
}

function makeRequest( url, callback, location)
{
	var req = new XMLHttpRequest();
	req.open('GET', url, true);
	req.onreadystatechange = function()
	{
		if (req.readyState == 4)
		{
            console.log("Che succede?");
            if( req.status == 200  )
            {
                console.log("Risposta ricevuta dal SERVER");
                var response=JSON.parse(req.responseText);
				
                callback(response, location);

            }
            else
            {
                if(callback==dati_openweather)
                {
                    console.log("Errore di comunicazione con openweather");
                    Pebble.sendAppMessage({"status" : "failed"});
                }
                else
                {
                    console.log("Errore di comunicazione con meteo.uniparthenope.it");
                    url= 'http://api.openweathermap.org/data/2.5/weather?lat='+location.coords.latitude+'&lon='+location.coords.longitude+'&units=metric&appid=1233eaeae2f2a527804d80980ab54fa8';
                    makeRequest(url, dati_openweather, location);
                }
            }
    	}
    };
    req.timeout=5000;
    req.ontimeout = function()
    {
    	console.log("lento lento lento");
    	if(callback==dati_openweather)
        {
            console.log("Errore di comunicazione con openweather");
            Pebble.sendAppMessage({"status" : "failed"});
        }
        else
        {
          	console.log("Errore di comunicazione con meteo.uniparthenope.it");
            url= 'http://api.openweathermap.org/data/2.5/weather?lat='+location.coords.latitude+'&lon='+location.coords.longitude+'&units=metric&appid=1233eaeae2f2a527804d80980ab54fa8';
            makeRequest(url, dati_openweather, location);
        }
    }
    
    req.send(null);
    console.log("URL: "+ url);
}

function station_nearest(response, location)
{

    if(response.places.length)
    {
        url='http://meteo.uniparthenope.it/openapi/v2/products/wrf3/forecast/' + response.places[0].id;
        makeRequest(url, dati_parthenope, location);
    }
    else
    {
        url= "http://api.openweathermap.org/data/2.5/weather?lat="+location.coords.latitude+"&lon="+location.coords.longitude+"&units=metric&appid=1233eaeae2f2a527804d80980ab54fa8";
        makeRequest(url, dati_openweather, location);
    }

}

function dati_parthenope(response, location)
{
	console.log("Caricamento terminato...");
    var humidity = "" + parseInt(response.forecast.data.places.place.rh2['#text']) + " \u0025 " ; // Make sure these are strings
    var temp = confTemp(response.forecast.data.places.place.t2c['#text']);
	var icon = findIcon(location, parseFloat(response.forecast.data.places.place.clf['#text']), parseFloat(response.forecast.data.places.place.crh['#text']));
    var wind = "" + findDirection(parseFloat(response.forecast.data.places.place.wd10['#text'])) + " " + setSpeed(response.forecast.data.places.place.ws10['#text']);
    var place = "" + response.forecast.data.places.place['@label'];
    
    //LOGS -----------------------------------------------------------------------------------------
    console.log("Place="+place);
    console.log("Clouds="+response.forecast.data.places.place.clf['#text']);
    console.log("pioggia="+response.forecast.data.places.place.crh['#text']);
    console.log("Wind="+wind);
    console.log("Temp="+temp);
    console.log("Icon="+icon);
    console.log("Humidity="+humidity);
    console.log("Invio informazioni");
    //STORAGE ---------------------------------------------------------------------------------------
    window.localStorage.setItem("place", place);
    window.localStorage.setItem("humidity", humidity);
    window.localStorage.setItem("temp", temp);
    window.localStorage.setItem("wind", wind);
    window.localStorage.setItem("icon", icon);
    
    //SEND -----------------------------------------------------------------------------------------
    Pebble.sendAppMessage
    ({
        "status": "reporting",
        "humidity": humidity,
        "temp": temp,
        "icon": icon,
        "wind": wind,
        "source": "Meteo Uniparthenope",
        "place" : place
	});
}

function dati_openweather(response, location)
{
	console.log("Caricamento terminato...");
    var humidity = "" + parseInt(response.main.humidity) + " \u0025 " ; // Make sure these are strings
    var temp =confTemp(response.main.temp);
	var icon = "" + response.weather[0].icon;
    var wind = "" + findDirection(parseFloat(response.wind.deg))+ " " + setSpeed(response.wind.speed * 1.9438);
    var place = "" + response.name;
    //LOGS -----------------------------------------------------------------------------------------
    console.log("Wind="+wind);
    console.log("Temp="+temp);
    console.log("Icon="+icon);
    console.log("Humidity="+humidity);
    console.log("Place="+place);
    console.log("Invio informazioni");
	//STORAGE ---------------------------------------------------------------------------------------
    window.localStorage.setItem("place", place);
    window.localStorage.setItem("humidity", humidity);
    window.localStorage.setItem("temp", temp);
    window.localStorage.setItem("wind", wind);
    window.localStorage.setItem("icon", icon);
    Pebble.sendAppMessage
    ({
        "status": "reporting",
        "humidity": humidity,
        "temp": temp,
        "icon": icon,
        "wind": wind,
        "source": "OpenWeather\nMap",
        "place" : place
	});
}
//Funzione per la chiamata al server e l'invio del risultato al Pebble
// Funzione che richiama fetchWether nel momento in cui la richiesta di localizzazione 
// è andata a buon fine
function fetchWeather(location)
{
    var url= 'http://meteo.uniparthenope.it/openapi/v2/places/search/bycoords/'+location.coords.latitude+'/'+location.coords.longitude+'?filter=[%22com%22,%22euro%22,%22world%22]&range=50';
    console.log(url);
    makeRequest( url, station_nearest, location);
}



// Funzione locationError
                      
function locationError(error) 
{
	console.log("Errore caricamento coordinate: " + error.message + "\n");
    Pebble.sendAppMessage({"status": "failed"});
}

var locationOptions = { "timeout": 15000, "maximumAge": 60000 }; 


function receivedHandler(message) {
        if(message.payload.status == "retrieve") 
        {
                console.log("Ricevuto status \"retrieve\"");
                console.log("Acquisizione coordinate");
                navigator.geolocation.getCurrentPosition(fetchWeather, locationError, locationOptions);
        }
}

function readyHandler(e) {
		console.log("Invio status: ready");
		    console.log("Icon="+s_Icon+"9");
        Pebble.sendAppMessage({
                              "status"  : "ready",
                              "temp"    : s_Temp,
                              "humidity": s_Humidity,
                              "icon"    : s_Icon,
                              "wind"    : s_Wind,
                              "place"   : s_Place
                              });
                              
}


Pebble.addEventListener("ready", readyHandler);
Pebble.addEventListener("appmessage", receivedHandler);
Pebble.addEventListener("showConfiguration", showConfigWindow);
Pebble.addEventListener("webviewclosed", configClosed);

