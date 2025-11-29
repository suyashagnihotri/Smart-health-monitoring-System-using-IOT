#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>

#define N1 WiFi
#define N2 ESP8266WebServer
#define N3 SoftwareSerial
#define N4(...) String(_VA_ARGS_)
#define N5(x,y) server.on(x,y)
#define N6(x,y,z) server.send(x,y,z)

const char* a1="HealthMonitor";
const char* a2="12345678";

N3 a3(D5,D6);
N2 server(80);

volatile int v1=0;
volatile int v2=0;
volatile float v3=0.0;
volatile unsigned long v4=0;

String L="";

const char H[] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta charset="UTF-8"><title>HM</title><meta name="viewport" content="width=device-width,initial-scale=1"><style>
body{font-family:Arial,sans-serif;background:#f5f5f5;margin:0;padding:10px;box-sizing:border-box}
h1{text-align:center;margin-bottom:10px}
.c{display:flex;flex-wrap:wrap;justify-content:center;gap:10px;margin-bottom:15px}
.k{background:#fff;border-radius:8px;padding:10px 15px;flex:1 1 100px;max-width:200px;box-shadow:0 2px 4px rgba(0,0,0,.1);text-align:center}
.t1{font-size:14px;color:#555;margin-bottom:4px}
.t2{font-size:24px;font-weight:bold}
.t3{font-size:12px;color:#777}
#q{background:#fff;border-radius:8px;padding:10px;box-shadow:0 2px 4px rgba(0,0,0,.1)}
#z{width:100%;height:250px}
.l{font-size:12px;margin-top:5px;text-align:center}
.l span{display:inline-block;margin:0 8px}
.d{display:inline-block;width:10px;height:10px;border-radius:50%;vertical-align:middle;margin-right:4px}
.dp{background:#e74c3c}.ds{background:#3498db}.dt{background:#27ae60}
</style></head><body>
<h1>Health Monitor</h1>
<div class="c">
<div class="k"><div class="t1">Pulse</div><div class="t2" id="p">--</div><div class="t3">bpm</div></div>
<div class="k"><div class="t1">SpO₂</div><div class="t2" id="s">--</div><div class="t3">%</div></div>
<div class="k"><div class="t1">Body Temp</div><div class="t2" id="t">--</div><div class="t3">°C</div></div>
</div>
<div id="q"><canvas id="z"></canvas>
<div class="l"><span><span class="d dp"></span>Pulse</span><span><span class="d ds"></span>SpO₂</span><span><span class="d dt"></span>Temp</span></div>
</div>
<script>
var c=document.getElementById('z'),x=c.getContext('2d');function r(){c.width=c.clientWidth;c.height=c.clientHeight}window.addEventListener('resize',r);r();
var m=60,a=[],b=[],d=[],e=[];
function A(j,k,l,u){a.push(j);b.push(k);d.push(l);e.push(u);if(a.length>m){a.shift();b.shift();d.shift();e.shift()}B()}
function B(){var w=c.width,h=c.height;x.clearRect(0,0,w,h);if(e.length<2)return;var mn=9999,mx=-9999,f=a.concat(b,d);for(var i=0;i<f.length;i++){if(f[i]<mn)mn=f[i];if(f[i]>mx)mx=f[i]}if(mx-mn<5){mx+=2.5;mn-=2.5}
function Y(v){return h-((v-mn)/(mx-mn))*(h-10)-5}
function X(i){return e.length<=1?0:(i/(e.length-1))*(w-10)+5}
x.beginPath();x.strokeStyle='#ddd';x.moveTo(0,h/2);x.lineTo(w,h/2);x.stroke();
function L(D,col){x.beginPath();x.strokeStyle=col;for(var i=0;i<D.length;i++){var xx=X(i),yy=Y(D[i]);if(i==0)x.moveTo(xx,yy);else x.lineTo(xx,yy)}x.stroke()}
L(a,'#e74c3c');L(b,'#3498db');L(d,'#27ae60')}
function C(){fetch('/data').then(function(z){return z.json()}).then(function(o){
document.getElementById('p').textContent=o.pulse;
document.getElementById('s').textContent=o.spo2;
document.getElementById('t').textContent=o.temp.toFixed(1);
A(o.pulse,o.spo2,o.temp,o.t);
}).catch(function(e){console.log(e)})}
setInterval(C,1000);C();
</script></body></html>
)=====";

void f0(){server.send_P(200,"text/html",H);}

void f1(){
  String j="{\"pulse\":"+N4(v1)+",\"spo2\":"+N4(v2)+",\"temp\":"+N4(v3,1)+",\"t\":"+N4(v4)+"}";
  N6(200,"application/json",j);
}

void f2(){N6(404,"text/plain","X");}

void f3(const String& s){
  int c1=s.indexOf(','),c2=s.indexOf(',',c1+1);
  if(c1<0||c2<0)return;
  String a=s.substring(0,c1);
  String b=s.substring(c1+1,c2);
  String c=s.substring(c2+1);
  int p=a.toInt();
  int o=b.toInt();
  float t=c.toFloat();
  v1=p;v2=o;v3=t;v4++;
}

void setup(){
  Serial.begin(115200);
  delay(500);
  a3.begin(9600);
  N1.mode(WIFI_AP);
  N1.softAP(a1,a2);
  IPAddress ip=N1.softAPIP();
  Serial.print("AP: ");Serial.println(a1);
  Serial.print("IP: ");Serial.println(ip);
  N5("/",f0);
  N5("/data",f1);
  server.onNotFound(f2);
  server.begin();
}

void loop(){
  server.handleClient();
  while(a3.available()){
    char ch=a3.read();
    if(ch=='\n'){
      if(L.length()>0){f3(L);L="";}
    }else if(ch!='\r'){
      L+=ch;
      if(L.length()>60)L="";
    }
  }
}
