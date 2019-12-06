/* 
 * File:   Machine.cpp
 * Author: guilherme
 * 
 * Created on 5 de Dezembro de 2019, 21:06
 */

#include "Machine.h"

Machine::Machine(){
    display.LCD_Init();
    display.LCD_String("  Reading.....");
    display.LCD_Command(0xC0);		/* Go to 2nd line*/
    display.LCD_String("     ......");
    _state = 3;
}
Machine::~Machine() {}

void Machine::event_Command(void *p){   
    ((Machine *)p) -> handle_fsm(Command);    
}

void Machine::event_Read(void *p){
    ((Machine *)p) -> handle_fsm(Read);
}

void Machine::convert(uint16_t v) {
    char value[sizeof (uint16_t)];
    ltoa(v, value, 10);
    display.LCD_String(value);
    uart.puts(value);
}

void Machine::convert8(uint8_t v) {
    char value[sizeof (uint8_t)];
    ltoa(v, value, 10);
    uart.puts(value);
    display.LCD_String(value);
}

void Machine::tx_serial(uint16_t v) {
    char value[sizeof (uint16_t)];
    ltoa(v, value, 10);
    uart.puts(value);
}
void Machine::tx_serial8(uint8_t v) {
    char value[sizeof (uint8_t)];
    ltoa(v, value, 10);
    uart.puts(value);
}

void Machine::handle_fsm(int event){    
    if(event == Command){
        if(!uart.has_data()){
            return;
        }
        else{
            info = uart.get();
            uart.put(info);
        }
    }
    
    switch(_state){        
        case Idle:
            display.LCD_Clear();
            if(event == Read){
                uart.puts("\nCase Idle");
                single = adc.single_read(ADConverter::A0);
                display.LCD_String(" L: ");
                single = (single*100/1021);
                single = 100 - single;                
                convert(single);
                display.LCD_String("%");
                display.LCD_String("  H: ");
                if(dht_gethumidity(&humidity) != -1){
                    convert8(humidity);
                }
                display.LCD_String("%");
                display.LCD_Command(0xC0);		/* Go to 2nd line*/
                display.LCD_String(" T: ");	
                convert8(bmp.readTemp());
                display.LCD_String("  P: ");	
                convert8(bmp.readPress());
                //_delay_ms(2000); // retirar, para deixar pelo timer                
                _state = Idle;
            }
            else if(event == Command){
                if (info == '1'){
                    display.LCD_String("Valor digitado:");
                    display.LCD_Char(info);
                    _state = Conf;
                }
                else if (info == '2'){
                    display.LCD_String("Valor digitado:");
                    display.LCD_Char(info);   
                    _state = Get;
                }
                else if (info == '3'){
                    display.LCD_String("Valor digitado:");
                    display.LCD_Char(info);   
                    _state = Synck;
                }
                else{
                    uart.put(info);
                    display.LCD_String("     Valor  ");
                    display.LCD_Command(0xC0);		/* Go to 2nd line*/
                    display.LCD_String("   Inv�lido!  ");
                    _delay_ms(2000);  // s� para teste
                    display.LCD_Clear();
                }
            }
            break;            


        case Conf:
            display.LCD_Clear();
            uart.puts("\nCase Conf");
                display.LCD_String(" Digite o valor");
                display.LCD_Command(0xC0);		/* Go to 2nd line*/
                display.LCD_String("  do intervalo");
                _delay_ms(1000);
                display.LCD_Clear();
                display.LCD_String(" Digite o valor");
                display.LCD_Command(0xC0);		/* Go to 2nd line*/
                display.LCD_String("   em minutos");
                _delay_ms(1000);
                while(true){
                    display.LCD_Clear();
                    display.LCD_String("Aguardando...");
                    _delay_ms(1000);
                    if(!uart.has_data()){
                        continue;
                    }
                    intervalo = uart.get();
                    uart.put(intervalo);
                    display.LCD_Clear();
                    display.LCD_String("   Intervalo");
                    display.LCD_Command(0xC0);		/* Go to 2nd line*/
                    display.LCD_String("  Digitado: ");
                    display.LCD_Char(intervalo);
                    //timer.setTimeout(intervalo*1000, &event_Read,0);
                    _delay_ms(2000);
                    if(uart.has_data()){
                        uart.get();
                    }
                    _delay_ms(2000);
                    break;
                }
                _state = Idle;
                break;


        case Get:
            display.LCD_Clear();
            uart.puts("Case Get");
            _delay_ms(2000);
            _state = Idle;
            break;


        case Synck:
            display.LCD_Clear();
            uart.puts("Case Synck");
            _delay_ms(2000);
            _state = Idle;
            break;
    }
}
