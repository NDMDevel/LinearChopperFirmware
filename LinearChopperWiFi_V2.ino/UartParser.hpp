#ifndef __UART_PARSER_HPP
#define __UART_PARSER_HPP

#include <cstdint>
#include <ESP8266WebServer.h>

class UartParser
{
  private:
    uint8_t x;
    enum
    {
      SHUTDOWN,
      INIT,
      WAIT_IDLE,
      TRANSMIT_REQUEST,
      GENERATE_IDLE,
      WAIT_RESPONSE,
      READ_RESPONSE,
      PROCESS_RESPONSE
    }st;
    static constexpr uint8_t buffer_size = 32;
    uint8_t buffer[buffer_size];
    uint8_t buff_idx;
    uint8_t Xor;
    uint32_t local_timer;
    bool busy;
    enum Command
    {
      SET_PARAMS = 0xAA,
      GET_PARAMS = 0x55,
      GET_VDC    = 0xA5
//      GET_RELAY_COUNTER = 0x5A
    };
    Command com;
    uint16_t vmin;
    uint16_t vmax;
    uint16_t vcritic;
    uint16_t delay_speed;
    uint16_t relay_reset_voltage;
    uint16_t reset_duration;
    uint8_t  chopper_active;
    uint16_t mcu_vdc;
    uint16_t mcu_vmin;
    uint16_t mcu_vmax;
    uint16_t mcu_vcritic;
    uint16_t mcu_speed;
    uint16_t mcu_relay_reset_voltage;
    uint16_t mcu_reset_duration;
    uint8_t  mcu_chopper_active;
//    uint32_t mcu_relay_counter;
    ESP8266WebServer *server;
  public:
    void begin(ESP8266WebServer *server);
    void operator()();    //loop task
    bool isBusy(){ return busy; }
    void setParams(uint16_t vmin,
                   uint16_t vmax,
                   uint16_t vcritic,
                   uint16_t delay_speed,
                   uint16_t relay_reset_voltage,
                   uint16_t reset_duration,
                   uint8_t chopper_active);
    void getParams();
    void getVdc();
//    void getRelayCounter();
  protected:
    void resetTimer(uint32_t &local_timer);
    bool timerElapse(uint32_t local_timer,uint32_t ms);
    void sendHL(uint16_t data);
};

#endif //__UART_PARSER_HPP
