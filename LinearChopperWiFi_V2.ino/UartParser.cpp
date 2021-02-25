#include "UartParser.hpp"
#include "Arduino.h"

void UartParser::begin(ESP8266WebServer *server)
{
  this->server = server;
  st = INIT;
  busy = true;
}

void UartParser::operator()()
{
  if( st == SHUTDOWN )
    return;
  if( st == INIT )
  {
    resetTimer(local_timer);
    st = WAIT_IDLE;
    return;
  }
  if( st == WAIT_IDLE )
  {
    if( Serial.available() > 0 )
    {
      resetTimer(local_timer);
      Serial.read();
      return;
    }
    if( !timerElapse(local_timer,500) )
      return;
    busy = false;
    st = TRANSMIT_REQUEST;
    return;
  }
  if( st == TRANSMIT_REQUEST )
  {
    if( Serial.available() != 0 )
    {
      //error, mcu should not be sending any data, go to INIT to wait for it to free the channel
      busy = true;
      st = INIT;
      return;
    }
    if( busy == false )
      return;
    Serial.write(com);
    if( com == SET_PARAMS )
    {
      sendHL(vmin);
      sendHL(vmax);
      sendHL(vcritic);
      sendHL(delay_speed);
      sendHL(relay_reset_voltage);
      sendHL(reset_duration);
      Serial.write(chopper_active);
    }
    if( com == GET_PARAMS ) { Xor = com; }
    if( com == GET_VDC )    { Xor = com; }
//    if( com == GET_RELAY_COUNTER ) { Xor = com; }
    Serial.write(Xor);
    if( com == SET_PARAMS )
    {
//      digitalWrite(2,!digitalRead(2));
      server->sendHeader("Location", "/", true);
      server->send(302, "text/plane", "");
    }
    resetTimer(local_timer);
    st = GENERATE_IDLE;
    return;
  }
  if( st == GENERATE_IDLE )
  {
    if( !timerElapse(local_timer,300) )
      return;
    if( com == SET_PARAMS )
    {
      busy = false;
      st = TRANSMIT_REQUEST;
      return;
    }
    buff_idx = 0;
    resetTimer(local_timer);
    st = WAIT_RESPONSE; //com must be GET_PARAMS or GET_VDC
    return;  
  }
  if( st == WAIT_RESPONSE )
  {
    if( timerElapse(local_timer,1000) ) //if no response in 1 second, communication error is assumed
    {
      /*
      //microcontroller is not responding
      if( com == GET_PARAMS )
      {
        mcu_vmin = 0xFFFF;
        mcu_vmax = 0xFFFF;
        mcu_vcritic = 0xFFFF;
        mcu_speed = 0xFFFF;
        mcu_chopper_active = 3;
      }
      if( com == GET_VDC )
        mcu_vdc = 0xFFFF;
      */

      busy = false;
      st = TRANSMIT_REQUEST;
      return;
    }
    if( Serial.available() != 0 )
    {
      resetTimer(local_timer);
      st = READ_RESPONSE;
    }
    return;
  }
  if( st == READ_RESPONSE )
  {
    if( Serial.available() > 0 )
    {
      resetTimer(local_timer);
      if( buff_idx >= buffer_size )
      {
        //error, input stream too long. Go to INIT to wait for the stream to end
        busy = true;
        st = INIT;
        return;
      }
      buffer[buff_idx++] = Serial.read();
      return;
    }
    if( timerElapse(local_timer,300) )
    {
      //input stream ended ok, now is time to parse it
      st = PROCESS_RESPONSE;
    }
    return;
  }
  if( st == PROCESS_RESPONSE )
  {
    if( buffer[0] == com && buff_idx == 15 && com == GET_PARAMS )
    {                     //buff_idx == 15
      //ajax response:
      String ajax_resp;
      mcu_vmin = buffer[1];
      mcu_vmin = (mcu_vmin<<8) | buffer[2];
      mcu_vmax = buffer[3];
      mcu_vmax = (mcu_vmax<<8) | buffer[4];
      mcu_vcritic = buffer[5];
      mcu_vcritic = (mcu_vcritic<<8) | buffer[6];
      mcu_speed = buffer[7];
      mcu_speed = (mcu_speed<<8) | buffer[8];
      mcu_relay_reset_voltage = buffer[9];
      mcu_relay_reset_voltage = (mcu_relay_reset_voltage<<8) | buffer[10];
      mcu_reset_duration = buffer[11];
      mcu_reset_duration = (mcu_reset_duration<<8) | buffer[12];
      mcu_chopper_active = buffer[13]; //mcu_chopper_active = buffer[13];
//      Serial.println(mcu_speed);
      String chopper_state;
      if( mcu_chopper_active != 0 )
        chopper_state = "active";
      else
        chopper_state = "disabled";
      //chopper_state = mcu_chopper_active;
      ajax_resp = mcu_vmin    + String(",") +
                  mcu_vmax    + String(",") +
                  mcu_vcritic + String(",") +
                  mcu_speed   + String(",") +
                  mcu_relay_reset_voltage + String(",") +
                  mcu_reset_duration      + String(",") +
                  chopper_state;

      server->send(200, "text/plane",ajax_resp);
    }
    if( buffer[0] == com && buff_idx == 4 && com == GET_VDC )
    {
      //ajax response:
      String ajax_resp;
      mcu_vdc = buffer[1];
      mcu_vdc = (mcu_vdc<<8) | buffer[2];

      ajax_resp = mcu_vdc + String(" V");
      server->send(200, "text/plane",ajax_resp);
    }
    /*if( buffer[0] == com && buff_idx == 6 && com == GET_RELAY_COUNTER )
    {
      //ajax response:
      String ajax_resp;
      uint32_t val = buffer[1];
      mcu_relay_counter = val << 24;
      val = buffer[2];
      mcu_relay_counter |= val << 16;
      val = buffer[3];
      mcu_relay_counter |= val << 8;
      val = buffer[4];
      mcu_relay_counter |= val;

      ajax_resp = mcu_relay_counter;
      server->send(200, "text/plane",ajax_resp);
    }*/
    //either error or success, go to TRANSMIT_REQUEST
    busy = false;
    st = TRANSMIT_REQUEST;
    return;
  }
}

void UartParser::setParams(uint16_t vmin,
                           uint16_t vmax,
                           uint16_t vcritic,
                           uint16_t delay_speed,
                           uint16_t relay_reset_voltage,
                           uint16_t reset_duration,
                           uint8_t chopper_active)
{
  if( busy == false )
  {
    com = SET_PARAMS;
    this->vmin = vmin;
    this->vmax = vmax;
    this->vcritic = vcritic;
    this->delay_speed = delay_speed;
    this->relay_reset_voltage = relay_reset_voltage;
    this->reset_duration = reset_duration;
    this->chopper_active = chopper_active;
    Xor = com;
    Xor ^= (vmin>>8) & 0xFF;
    Xor ^= vmin      & 0xFF;
    Xor ^= (vmax>>8) & 0xFF;
    Xor ^= vmax      & 0xFF;
    Xor ^= (vcritic>>8) & 0xFF;
    Xor ^= vcritic      & 0xFF;
    Xor ^= (delay_speed>>8) & 0xFF;
    Xor ^= delay_speed      & 0xFF;
    Xor ^= (relay_reset_voltage>>8) & 0xFF;
    Xor ^= relay_reset_voltage      & 0xFF;
    Xor ^= (reset_duration>>8) & 0xFF;
    Xor ^= reset_duration      & 0xFF;
    Xor ^= chopper_active;
    busy = true;
  }
}

void UartParser::getParams()
{
  if( busy == false )
  {
    com = GET_PARAMS;
    busy = true;
  }
}

void UartParser::getVdc()
{
  if( busy == false )
  {
    com = GET_VDC;
    busy = true;
  }
}

/*void UartParser::getRelayCounter()
{
  if( busy == false )
  {
    com = GET_RELAY_COUNTER;
    busy = true;  
  }
}*/

void UartParser::resetTimer(uint32_t &local_timer)
{
  local_timer = (uint32_t)~millis() + (uint32_t)1;
}

bool UartParser::timerElapse(uint32_t local_timer,uint32_t ms)
{
  return millis()+local_timer >= ms;
}

void UartParser::sendHL(uint16_t val)
{
  uint8_t H = val >> 8;
  uint8_t L = val & 0xFF;
  Serial.write(H);
  Serial.write(L);  
}
