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
    <form action="http://)=====" + ip.toString() + R"=====(/set_params\" method="post" style="margin-top:1em;padding: 0 0.3em;background-color: #C0C0C0">
      <p style="font-weight:bold">Input Params:</p>
      <span class="param_row">Vmin:<input name="vmin" placeholder="minimun voltage [0-800V]" type="text"></span>
      <span class="param_row">Vmax:<input name="vmax" placeholder="maximun voltage [0-800V]" type="text"></span>
      <span class="param_row">Vcritic:<input name="vcritic" placeholder="critical voltage [0-800V]" type="text"></span>
      <span class="param_row">Delay Duty 0-100%<select id="speed_select" name="speed" size=1><option value=""></option></select></span>
      <span><input type="checkbox" name="chopper_active" id="chopper_active" value="active"> Chopper Enabled
      <input type="submit" class="btn_update" value="Update"></span>
    </form>
    <div style="margin-top:1em;padding: 0 0.4em;background-color: #00CC33">
      <p id="monitor_id" style="font-weight:bold">Monitor:</p>
      <span class="param_row">Vmin:<input id="mcu_vmin" type="text" readonly disabled></span>
      <span class="param_row">Vmax:<input id="mcu_vmax" type="text" readonly disabled></span>
      <span class="param_row">Vcritic:<input id="mcu_vcritic" type="text" readonly disabled></span>
      <span class="param_row">Delay:<input id="mcu_speed" type="text" readonly disabled></span>
      <span class="param_row">VDC:<input id="vdc" type="text" readonly disabled></span>
    </div>
)=====";

String page_tail = R"=====(
  </body>
  <script>
  init_timer();
  setInterval(function(){ ajax_req(); },1000);
  var get_params_timer = setInterval(function(){ ajax_get_params(); },1500);
  function ajax_req(req_id)
  {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200)
        document.getElementById("vdc").value = this.responseText;
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
        document.getElementById("mcu_vmin").value = resp[0];
        document.getElementById("mcu_vmax").value = resp[1];
        document.getElementById("mcu_vcritic").value = resp[2];
        document.getElementById("mcu_speed").value = resp[3];
        if( resp[4] == "active" )
        {
          resp = true;
          document.getElementById("monitor_id").textContent = "Monitor: Chopper Enabled"
        }
        else
        {
          resp = false;
          document.getElementById("monitor_id").textContent = "Monitor: Chopper Disabled"
        }
        document.getElementById("chopper_active").checked = resp;}
    };
    xhttp.open("GET", "get_params", true);
    xhttp.send();
  }
  function init_timer()
  {
    parent = document.getElementById("speed_select");
    i = 1;
    inc = 1;
    unit = " ms";
    while( i <= 10000 )
    {
      opt = document.createElement("option");
      opt.value = i;
      opt.textContent = i/inc + unit;
      parent.appendChild(opt);
      if( i < 10 )
        i += inc;
      else if( i < 1000 )
        i += 10;
      else
      {
        unit = " s";
        inc = 1000;
        i += 100;
      }
    }
  }
  </script>
<style>
  body{font-family: Verdana, Times, serif;}
  .param_row{
    display: flex;
    justify-content: space-between;
    padding: 0.25em}
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
    delay_speed = strarg.toInt();

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
