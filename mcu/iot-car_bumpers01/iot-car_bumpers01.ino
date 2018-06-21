#include <Arduino.h>

extern "C" {
#include "user_interface.h"
}

   

#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
//#include <ESP8266mDNS.h>
//#include <DNSServer.h>
#include <Hash.h>

char* HOSTNAME = "laheyBot";     
char* WifiPASS = "thereIsNoSpoon";

#define LEFT_PWM  D1
#define LEFT_DIR  D3

#define RIGHT_PWM D2
#define RIGHT_DIR D4

int rightFrontBumper = D6;



ESP8266WebServer server = ESP8266WebServer(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void Set_Speed(String MOTOR, int SPEED) {
    int dir = 0;
    if(SPEED < 0) {
        dir = 1;
        SPEED = SPEED * -1;
    }

    if(MOTOR == "left") {
        Serial.printf("Left - Speed: %u Direction %u \n", SPEED, dir);
        digitalWrite(LEFT_DIR, dir);
        analogWrite(LEFT_PWM, SPEED);
    } else if(MOTOR == "right") {  
        Serial.printf("Right - Speed: %u Direction %u \n", SPEED, dir);
        digitalWrite(RIGHT_DIR, dir);
        analogWrite(RIGHT_PWM, SPEED);
    }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

            // send message to client
            webSocket.sendTXT(num, "Connected");
        }
            break;
        case WStype_TEXT:
            Serial.printf("[%u] get Text: %s\n", num, payload);
            String strpayload = (char*)payload;

            if(int location = strpayload.indexOf(':')) {
                String MOTOR = strpayload.substring(0, location);
                String SPEED = strpayload.substring(location+1);

                int intSPEED = SPEED.toInt();
                
                Set_Speed(MOTOR, intSPEED);
            }
            break;
    }

}


void setup() {
    Serial.begin(115200);
    pinMode(LEFT_PWM, OUTPUT);
    pinMode(LEFT_DIR, OUTPUT);
    pinMode(RIGHT_PWM, OUTPUT);
    pinMode(RIGHT_DIR, OUTPUT);

    analogWrite(LEFT_PWM, 0);
    analogWrite(RIGHT_PWM, 0);
    digitalWrite(LEFT_DIR, 0);
    digitalWrite(RIGHT_DIR, 0);

    pinMode(rightFrontBumper, INPUT_PULLUP);



    

    // Start Wifi AP
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(HOSTNAME, WifiPASS);


    
    // handle index -- HTTP Server
    server.on("/", []() {
        // send index.html
        server.send(200, "text/html", "<html><head><title>Joystick</title><meta content=\"width=device-width, initial-scale=1, max-width=device-width, max-scale=1\" name=\"viewport\"><script type=\"text/javascript\">function send(n){var o=parseInt(document.getElementById(n).value);console.log(n+\": \"+o.toString()),connection.send(n+\":\"+o.toString())}var connection=new WebSocket(\"ws://\"+location.hostname+\":81/\",[\"arduino\"]);connection.onopen=function(){connection.send(\"#Connect \"+new Date)},connection.onerror=function(n){console.log(\"WebSocket Error \",n)},connection.onmessage=function(n){console.log(\"WebSocket Message \",n.data)},document.addEventListener(\"DOMContentLoaded\",function(n){var o=document.getElementById(\"left\"),e=document.getElementById(\"right\");o.onmousedown=function(n){o.onmousemove=function(){send(\"left\")}},o.onmouseup=function(n){o.value=0,send(\"left\"),o.onmousemove=null},e.onmousedown=function(n){e.onmousemove=function(){send(\"right\")}},e.onmouseup=function(n){e.value=0,send(\"right\"),e.onmousemove=null},o.ontouchstart=function(n){o.ontouchmove=function(){send(\"left\")}},o.ontouchend=function(n){o.value=0,send(\"left\"),o.ontouchmove=null},e.ontouchstart=function(n){e.ontouchmove=function(){send(\"right\")}},e.ontouchend=function(n){e.value=0,send(\"right\"),e.ontouchmove=null}});var VirtualJoystick=function(t){t=t||{},this._container=t.container||document.body,this._strokeStyle=t.strokeStyle||\"cyan\",this._stickEl=t.stickElement||this._buildJoystickStick(),this._baseEl=t.baseElement||this._buildJoystickBase(),this._mouseSupport=void 0!==t.mouseSupport&&t.mouseSupport,this._stationaryBase=t.stationaryBase||!1,this._baseX=this._stickX=t.baseX||0,this._baseY=this._stickY=t.baseY||0,this._limitStickTravel=t.limitStickTravel||!1,this._stickRadius=void 0!==t.stickRadius?t.stickRadius:100,this._useCssTransform=void 0!==t.useCssTransform&&t.useCssTransform,this._container.style.position=\"relative\",this._container.appendChild(this._baseEl),this._baseEl.style.position=\"absolute\",this._baseEl.style.display=\"none\",this._container.appendChild(this._stickEl),this._stickEl.style.position=\"absolute\",this._stickEl.style.display=\"none\",this._pressed=!1,this._touchIdx=null,!0===this._stationaryBase&&(this._baseEl.style.display=\"\",this._baseEl.style.left=this._baseX-this._baseEl.width/2+\"px\",this._baseEl.style.top=this._baseY-this._baseEl.height/2+\"px\"),this._transform=!!this._useCssTransform&&this._getTransformProperty(),this._has3d=this._check3D();var s=function(t,s){return function(){return t.apply(s,arguments)}};this._$onTouchStart=s(this._onTouchStart,this),this._$onTouchEnd=s(this._onTouchEnd,this),this._$onTouchMove=s(this._onTouchMove,this),this._container.addEventListener(\"touchstart\",this._$onTouchStart,!1),this._container.addEventListener(\"touchend\",this._$onTouchEnd,!1),this._container.addEventListener(\"touchmove\",this._$onTouchMove,!1),this._mouseSupport&&(this._$onMouseDown=s(this._onMouseDown,this),this._$onMouseUp=s(this._onMouseUp,this),this._$onMouseMove=s(this._onMouseMove,this),this._container.addEventListener(\"mousedown\",this._$onMouseDown,!1),this._container.addEventListener(\"mouseup\",this._$onMouseUp,!1),this._container.addEventListener(\"mousemove\",this._$onMouseMove,!1))};VirtualJoystick.prototype.destroy=function(){this._container.removeChild(this._baseEl),this._container.removeChild(this._stickEl),this._container.removeEventListener(\"touchstart\",this._$onTouchStart,!1),this._container.removeEventListener(\"touchend\",this._$onTouchEnd,!1),this._container.removeEventListener(\"touchmove\",this._$onTouchMove,!1),this._mouseSupport&&(this._container.removeEventListener(\"mouseup\",this._$onMouseUp,!1),this._container.removeEventListener(\"mousedown\",this._$onMouseDown,!1),this._container.removeEventListener(\"mousemove\",this._$onMouseMove,!1))},VirtualJoystick.touchScreenAvailable=function(){return\"createTouch\"in document},function(t){t.addEventListener=function(t,s){return void 0===this._events&&(this._events={}),this._events[t]=this._events[t]||[],this._events[t].push(s),s},t.removeEventListener=function(t,s){void 0===this._events&&(this._events={}),t in this._events!=!1&&this._events[t].splice(this._events[t].indexOf(s),1)},t.dispatchEvent=function(t){if(void 0===this._events&&(this._events={}),void 0!==this._events[t])for(var s=this._events[t].slice(),i=0;i<s.length;i++){var e=s[i].apply(this,Array.prototype.slice.call(arguments,1));if(void 0!==e)return e}}}(VirtualJoystick.prototype),VirtualJoystick.prototype.deltaX=function(){return this._stickX-this._baseX},VirtualJoystick.prototype.deltaY=function(){return this._stickY-this._baseY},VirtualJoystick.prototype.up=function(){if(!1===this._pressed)return!1;var t=this.deltaX(),s=this.deltaY();return!(s>=0)&&!(Math.abs(t)>2*Math.abs(s))},VirtualJoystick.prototype.down=function(){if(!1===this._pressed)return!1;var t=this.deltaX(),s=this.deltaY();return!(s<=0)&&!(Math.abs(t)>2*Math.abs(s))},VirtualJoystick.prototype.right=function(){if(!1===this._pressed)return!1;var t=this.deltaX(),s=this.deltaY();return!(t<=0)&&!(Math.abs(s)>2*Math.abs(t))},VirtualJoystick.prototype.left=function(){if(!1===this._pressed)return!1;var t=this.deltaX(),s=this.deltaY();return!(t>=0)&&!(Math.abs(s)>2*Math.abs(t))},VirtualJoystick.prototype._onUp=function(){this._pressed=!1,this._stickEl.style.display=\"none\",0==this._stationaryBase&&(this._baseEl.style.display=\"none\",this._baseX=this._baseY=0,this._stickX=this._stickY=0)},VirtualJoystick.prototype._onDown=function(t,s){if(this._pressed=!0,0==this._stationaryBase&&(this._baseX=t,this._baseY=s,this._baseEl.style.display=\"\",this._move(this._baseEl.style,this._baseX-this._baseEl.width/2,this._baseY-this._baseEl.height/2)),this._stickX=t,this._stickY=s,!0===this._limitStickTravel){var i=this.deltaX(),e=this.deltaY(),o=Math.sqrt(i*i+e*e);if(o>this._stickRadius){var n=i/o,r=e/o;this._stickX=n*this._stickRadius+this._baseX,this._stickY=r*this._stickRadius+this._baseY}}this._stickEl.style.display=\"\",this._move(this._stickEl.style,this._stickX-this._stickEl.width/2,this._stickY-this._stickEl.height/2)},VirtualJoystick.prototype._onMove=function(t,s){if(!0===this._pressed){if(this._stickX=t,this._stickY=s,!0===this._limitStickTravel){var i=this.deltaX(),e=this.deltaY(),o=Math.sqrt(i*i+e*e);if(o>this._stickRadius){var n=i/o,r=e/o;this._stickX=n*this._stickRadius+this._baseX,this._stickY=r*this._stickRadius+this._baseY}}this._move(this._stickEl.style,this._stickX-this._stickEl.width/2,this._stickY-this._stickEl.height/2)}},VirtualJoystick.prototype._onMouseUp=function(t){return this._onUp()},VirtualJoystick.prototype._onMouseDown=function(t){t.preventDefault();var s=t.clientX,i=t.clientY;return this._onDown(s,i)},VirtualJoystick.prototype._onMouseMove=function(t){var s=t.clientX,i=t.clientY;return this._onMove(s,i)},VirtualJoystick.prototype._onTouchStart=function(t){if(null===this._touchIdx&&!1!==this.dispatchEvent(\"touchStartValidation\",t)){this.dispatchEvent(\"touchStart\",t),t.preventDefault();var s=t.changedTouches[0];this._touchIdx=s.identifier;var i=s.pageX,e=s.pageY;return this._onDown(i,e)}},VirtualJoystick.prototype._onTouchEnd=function(t){if(null!==this._touchIdx){this.dispatchEvent(\"touchEnd\",t);for(var s=t.changedTouches,i=0;i<s.length&&s[i].identifier!==this._touchIdx;i++);if(i!==s.length)return this._touchIdx=null,t.preventDefault(),this._onUp()}},VirtualJoystick.prototype._onTouchMove=function(t){if(null!==this._touchIdx){for(var s=t.changedTouches,i=0;i<s.length&&s[i].identifier!==this._touchIdx;i++);if(i!==s.length){var e=s[i];t.preventDefault();var o=e.pageX,n=e.pageY;return this._onMove(o,n)}}},VirtualJoystick.prototype._buildJoystickBase=function(){var t=document.createElement(\"canvas\");t.width=126,t.height=126;var s=t.getContext(\"2d\");return s.beginPath(),s.strokeStyle=this._strokeStyle,s.lineWidth=6,s.arc(t.width/2,t.width/2,40,0,2*Math.PI,!0),s.stroke(),s.beginPath(),s.strokeStyle=this._strokeStyle,s.lineWidth=2,s.arc(t.width/2,t.width/2,60,0,2*Math.PI,!0),s.stroke(),t},VirtualJoystick.prototype._buildJoystickStick=function(){var t=document.createElement(\"canvas\");t.width=86,t.height=86;var s=t.getContext(\"2d\");return s.beginPath(),s.strokeStyle=this._strokeStyle,s.lineWidth=6,s.arc(t.width/2,t.width/2,40,0,2*Math.PI,!0),s.stroke(),t},VirtualJoystick.prototype._move=function(t,s,i){this._transform?this._has3d?t[this._transform]=\"translate3d(\"+s+\"px,\"+i+\"px, 0)\":t[this._transform]=\"translate(\"+s+\"px,\"+i+\"px)\":(t.left=s+\"px\",t.top=i+\"px\")},VirtualJoystick.prototype._getTransformProperty=function(){for(var t,s=[\"webkitTransform\",\"MozTransform\",\"msTransform\",\"OTransform\",\"transform\"],i=document.createElement(\"p\"),e=0;e<s.length;e++)if(t=s[e],null!=i.style[t])return t},VirtualJoystick.prototype._check3D=function(){var t=this._getTransformProperty();if(!t||!window.getComputedStyle)return module.exports=!1;var s=document.createElement(\"div\");s.style[t]=\"translate3d(1px,1px,1px)\",document.body.insertBefore(s,null);var i=getComputedStyle(s).getPropertyValue({webkitTransform:\"-webkit-transform\",OTransform:\"-o-transform\",msTransform:\"-ms-transform\",MozTransform:\"-moz-transform\",transform:\"transform\"}[t]);return document.body.removeChild(s),null!=i&&i.length&&\"none\"!=i};</script></head><body style=\"overscroll-behavior:contain;position:fixed;max-width:100%;max-height:100%;margin:0px\"></body><script>function valBetween(t,n,e){return Math.min(e,Math.max(n,t))}var joystick=new VirtualJoystick({container:document.body,mouseSupport:!0});setInterval(function(){var t=joystick.deltaX(),n=joystick.deltaY();joystick.deltaX(),joystick.deltaY(),joystick.right(),joystick.up(),joystick.left(),joystick.down();x=valBetween(t/window.innerWidth*2e3,-1023,1023),y=valBetween(n/window.innerHeight*2e3,-1023,1023),m=\"left: \"+Math.min(-y,1023).toString(),console.log(m),connection.send(m),m=\"right: \"+Math.min(-y-x,1023).toString(),console.log(m),connection.send(m)},1/30*1e3);</script></html>");
    });

/*
    server.on("/act", []() {
        // send index.html
    }
  */  
    server.begin();



    // start webSocket server
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    // Add service to MDNS
   // MDNS.addService("http", "tcp", 80);
   // MDNS.addService("ws", "tcp", 81);
    
}

void loop() {
  if(digitalRead(rightFrontBumper) == HIGH) {
    digitalWrite(LEFT_DIR, HIGH);
    digitalWrite(RIGHT_DIR, HIGH);
    analogWrite(LEFT_PWM, 1023);
    analogWrite(RIGHT_PWM, 1023);
    delay(1000);
  }
    webSocket.loop();
    server.handleClient();
}
