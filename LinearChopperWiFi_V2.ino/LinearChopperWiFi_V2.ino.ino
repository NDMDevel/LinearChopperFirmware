#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "UartParser.hpp"

//Global Vars:
#define LED 2 //pinLed
const char *ssid = "ESP-AP";
const char *password = "labelectro2019";

UartParser uart_parser;
ESP8266WebServer server(80);

//Static AP IP:
IPAddress ip(192,168,1,10);

//WebPage:
String page_head = R"=====(
<!DOCTYPE html>
  <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <meta http-equiv="X-UA-Compatible" content="ie=edge">
        <title>Linear Chopper</title>
    </head>
  <body>
    <p style="font-weight:bold">Linear PWM Chopper</p>
    <form onsubmit="return submit_check()" action="http://)=====" + ip.toString() + R"=====(/set_params\" method="post" style="margin-top:1em;padding: 0 0.3em;background-color: #C0C0C0">
      <p style="font-weight:bold">Input Params:</p>
      <span class="param_row">Vmin:<input name="vmin" placeholder="minimun voltage [0-800V]" type="text"></span>
      <span class="param_row">Vmax:<input name="vmax" placeholder="maximun voltage [0-800V]" type="text"></span>
      <span class="param_row">Vcritic:<input name="vcritic" placeholder="critical voltage [0-800V]" type="text"></span>
      <span class="param_row">Delay Duty 0-100%<select id="speed_select" name="speed" size=1><option value=""></option></select></span>
      <div style="background-color: #A0A0A0">
        <span class="param_row">Relay reset voltage:<input name="relay_reset_voltage" placeholder="voltage [0-800V]" type="text" title="when VDC < RESET VOLTAGE => Relay opens"></span>
        <span class="param_row">Reset duration:<select id="relay_duration_select" name="reset_duration" size=1 title="time interval that the relay remains opened before it closes again"><option value=""></option></select></span>
      </div>
      <span><input type="checkbox" name="chopper_active" id="chopper_active" value="active"> Chopper Enabled
      <input type="submit" class="btn_update" value="Update"></span>
    </form>
    <div style="margin-top:1em;padding: 0 0.4em;background-color: #00CC33">
      <p id="monitor_id" style="font-weight:bold">Monitor:</p>
      <span class="param_row">Vmin:<input id="mcu_vmin" type="text" readonly disabled></span>
      <span class="param_row">Vmax:<input id="mcu_vmax" type="text" readonly disabled></span>
      <span class="param_row">Vcritic:<input id="mcu_vcritic" type="text" readonly disabled></span>
      <span class="param_row">Delay:<input id="mcu_speed" type="text" readonly disabled></span>
      <div style="background-color: #00AC13;">
        <span class="param_row">Relay reset voltage:<input id="mcu_relay_reset_voltage" type="text" readonly disabled></span>
        <span class="param_row">Reset duration:<input id="mcu_relay_duration" type="text" readonly disabled></span>
        <span class="param_row">Relay activations:<input id="relay_counter" type="text" readonly disabled></span>
      </div>
      <span class="param_row">VDC:<input id="vdc" type="text" readonly disabled></span>
    </div>
)=====";

String page_tail = R"=====(
  </body>
<script>
//insert the options inside the delay combo box (select html tag)
init_speed();
init_relay_reset_duration();

setInterval(function(){ ajax_req(); },1000);
var get_params_timer = setInterval(function(){ ajax_get_params(); },1500);

function submit_check()
{
  console.log("verifying input params: [not implemented, does nothing]");
  return true;
}
function ajax_req(req_id)
{
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200)
    {
      var resp = this.responseText.split(",");
      document.getElementById("vdc").value = resp[0];//this.responseText;
      if( resp.length > 1 )
        document.getElementById("relay_counter").value = resp[1];
    }
  };
  xhttp.open("GET", "get_vdc", true);
  xhttp.send();
}
function ajax_get_params()
{
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      clearInterval(get_params_timer);
      resp = this.responseText.split(",");
      console.log(this.responseText);
      document.getElementById("mcu_vmin").value = resp[0] + String(" V");
      document.getElementById("mcu_vmax").value = resp[1] + String(" V");
      document.getElementById("mcu_vcritic").value = resp[2] + String(" V");
      if( resp[3] >= 1000 )
        document.getElementById("mcu_speed").value = resp[3]/1000 + String(" s");
      else
        document.getElementById("mcu_speed").value = resp[3] + String(" ms");
      document.getElementById("mcu_relay_reset_voltage").value = resp[4] + String(" V");
      if( resp[5] >= 1000 )
        document.getElementById("mcu_relay_duration").value = resp[5]/1000 + String(" s");
      else
        document.getElementById("mcu_relay_duration").value = resp[5] + String(" ms");
      if( resp[6] == "active" )
      {
        resp = true;
        document.getElementById("monitor_id").textContent = "Monitor: Chopper Enabled";
      }
      else
      {
        resp = false;
        document.getElementById("monitor_id").textContent = "Monitor: Chopper Disabled";
      }
      document.getElementById("chopper_active").checked = resp;
    }
  };
  xhttp.open("GET", "get_params", true);
  xhttp.send();
}
function init_speed()
{
  parent = document.getElementById("speed_select");
  var ms = 100;
  var integer = 100;
  var decimal = 0;
  var unit = " ms";
  while( ms <= 10000 )
  {
    opt = document.createElement("option");
    opt.value = ms;
    if( ms < 1000 )
    {
      opt.textContent = integer + unit;
      integer += 100;
    }
    else
    {
      if( integer > 10 )
      {
        integer = 1;
        unit = " s";
      }
      if( decimal == 0 )
        opt.textContent = integer + unit;
      else
        opt.textContent = integer + "." + decimal + unit;
      decimal++;
      if( decimal == 10 )
      {
        decimal = 0;
        integer++;
      }
    }
    parent.appendChild(opt);
    ms += 100;
  }
}
function init_relay_reset_duration()
{
  parent = document.getElementById("relay_duration_select");
  var duration = [ 1000 , 2000 , 5000 , 10000 ]; //time in ms
  for( i=0 ; i<duration.length ; i++ )
  {
    opt = document.createElement("option");
    opt.value = duration[i];
    opt.textContent = duration[i]/1000 + " s";
    parent.appendChild(opt);
  }
}
</script>
<style>
  body{font-family: Verdana, Times, serif;}
  .param_row{
    display: flex;
    justify-content: space-between;
    padding: 0.25em;}
  .btn_update{
    background-color:  #f44336; /* Green */
    border: none;
    color: white;
    padding: 7px 16px;
    text-align: center;
    text-decoration: none;
    display: inline-block;
    font-size: 16px;
    font-weight: bold;
    margin: 4px 2px;}
  </style>  
</html>
)=====";

void setup()
{
  Serial.begin(115200);
  Serial.swap();
  uart_parser.begin(&server);
  
  pinMode(LED,OUTPUT);

  WiFi.mode(WIFI_AP);
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255,255,255,0);
  WiFi.softAPConfig(ip,gateway,subnet);
  WiFi.softAP(ssid,password);

  server.on("/",home_handler);
  server.on("/get_vdc",vdc_handler);
//  server.on("/get_relay_counter",relay_counter_handler);
  server.on("/get_params",get_params_handler);
  server.on("/set_params/",set_params_handler);
  server.onNotFound(notfound);
  server.begin();  
}

void loop()
{
  server.handleClient();
  uart_parser();
}

void vdc_handler()
{
  uart_parser.getVdc();
}

/*void relay_counter_handler()
{
  uart_parser.getRelayCounter();
}*/

void get_params_handler()
{
  uart_parser.getParams();
}

void set_params_handler()
{
  String strarg; 

  uint16_t vmin;
  strarg = server.arg("vmin");
  strarg.trim();
  if( strarg == "" )
    vmin = 0xFFFF;
  else
    vmin = strarg.toInt();

  uint16_t vmax;
  strarg = server.arg("vmax");
  strarg.trim();
  if( strarg == "" )
    vmax = 0xFFFF;
  else
    vmax = strarg.toInt();

  uint16_t vcritic;
  strarg = server.arg("vcritic");
  strarg.trim();
  if( strarg == "" )
    vcritic = 0xFFFF;
  else
    vcritic = strarg.toInt();

  uint16_t delay_speed;
  strarg = server.arg("speed");
  strarg.trim();
  if( strarg == "" )
    delay_speed = 0xFFFF;
  else
    delay_speed = strarg.toInt(); //send the time in ms

  uint16_t relay_reset_voltage;
  strarg = server.arg("relay_reset_voltage");
  strarg.trim();
  if( strarg == "" )
    relay_reset_voltage = 0xFFFF;
  else
    relay_reset_voltage = strarg.toInt();
  
  uint16_t reset_duration;
  strarg = server.arg("reset_duration");
  strarg.trim();
  if( strarg == "" )
    reset_duration = 0xFFFF;
  else
    reset_duration = strarg.toInt(); //sends the time in ms

  uint8_t chopper_active;
  strarg = server.arg("chopper_active");
  strarg.trim();
  if( strarg == "" )
    chopper_active = 0;
  else
    chopper_active = 1;

  uart_parser.setParams(vmin,
                        vmax,
                        vcritic,
                        delay_speed,
                        relay_reset_voltage,
                        reset_duration,
                        chopper_active);
}

uint8_t serial_send_HL(uint16_t val)
{
    uint8_t H = val >> 8;
    uint8_t L = val & 0xFF;
    uint8_t Xor = H ^ L;
    Serial.write(H);
    Serial.write(L);
    return Xor;
}

void home_handler()
{
  server.send(200, "text/html",page_head + page_tail);
}

void notfound()
{
  server.send(404, "text/html",R"=====(
    <!DOCTYPE html>
    <html lang="en">
      <h1>Page Not Found</h1>
      <a href="http://)=====" + ip.toString() + R"=====(\">Go back home.</a>
    </html>
    )=====");
}
