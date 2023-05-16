#ifndef _STM32OTA_H_
#define _STM32OTA_H_

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
#include <FS.h>

#define STERR         "ERROR"
#define STM32INIT     0x7F          // Send Init command
#define STM32ACK      0x79          // return ACK answer
#define STM32NACK     0x1F          // return NACK answer
#define STM32GET      0             // get version command
#define STM32GVR      0x01          // get read protection status           never used in here
#define STM32ID       0x02          // get chip ID command
#define STM32RUN      0x21          // Restart and Run programm
#define STM32RD       0x11          // Read flash command                   never used in here
#define STM32WR       0x31          // Write flash command
#define STM32ERASE    0x43          // Erase flash command
#define STM32ERASEN   0x44          // Erase extended flash command
#define STM32WP       0x63          // Write protect command                never used in here
#define STM32WP_NS    0x64          // Write protect no-stretch command     never used in here
#define STM32UNPCTWR  0x73          // Unprotect WR command                 never used in here
#define STM32UW_NS    0x74          // Unprotect write no-stretch command   never used in here
#define STM32RP       0x82          // Read protect command                 never used in here
#define STM32RP_NS    0x83          // Read protect no-stretch command      never used in here
#define STM32UR       0x92          // Unprotect read command               never used in here
#define STM32UR_NS    0x93          // Unprotect read no-stretch command    never used in here
#define STM32XOR      0xFF          // Unprotect read no-stretch command    

#define STM32STADDR   0x8000000     // STM32 codes start address, you can change to other address if use custom bootloader like 0x8002000
#define STM32ERR      0

class stm32ota
{
  private:
    int pin_NRST;
    int pin_BOOT0;
    int pin_LED;

    void SendCommand(unsigned char commd);
    unsigned char GetId();
    unsigned char Erase();
    unsigned char Erasen();
    unsigned char Address(unsigned long addr);
    unsigned char SendData(unsigned char * data, unsigned char wrlen);
    unsigned char getChecksum( unsigned char * data, unsigned char datalen);

  public:
    stm32ota(int _NRST, int _BOOT0 , int _LED);
    String otaUpdate(String File_Url);
    void FlashMode();
    void RunMode();
    String conect();
    char chipVersion();
    String binfilename();
    boolean  downloadFile(String File_Url , String File_Name);
    void deletfiles(String bin_file);
    boolean Flash(String bin_file_name);
    boolean EraseChip();
    String stm32Read(unsigned long rdaddress, unsigned char rdlen);
};

#endif
