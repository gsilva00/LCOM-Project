#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if(lsb == NULL){
    return 1;
  }
  uint16_t mask = 0x00ff;
  uint16_t temporario = mask & val;     //0x1234 & 0x00ff = 0x0034
  *lsb = temporario;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if(msb == NULL){
    return 1;
  }
  uint16_t mask = 0xff00;
  uint16_t temporario = mask & val;     //0x1234 & 0xff00 = 0x1134
  *msb = temporario >> 8;
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  if(value == NULL){
    return 1;
  }
  uint32_t temporario;
  if( sys_inb(port,&temporario) ){
    return 1;
  }
  *value = temporario;  //temporario: 0x000000ff => value 0xff
  return 0;
}
