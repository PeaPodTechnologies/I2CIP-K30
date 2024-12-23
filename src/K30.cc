#include <K30.h>

using namespace I2CIP;

I2CIP_DEVICE_INIT_STATIC_ID(K30);
I2CIP_INPUT_INIT_RESET(K30, uint16_t, 0, void*, nullptr);

K30::K30(i2cip_fqa_t fqa, const i2cip_id_t& id) : I2CIP::Device(fqa, id), I2CIP::InputInterface<uint16_t, void*>((I2CIP::Device*)this) { }

i2cip_errorlevel_t K30::get(uint16_t& dest, void* const& args) {
  // 0. Check args
  // if(args != nullptr) {
  //   return I2CIP_ERR_SOFT;
  // }

  // #ifdef WIIPOD_K30_DELAY
  //   delay(WIIPOD_K30_DELAY);
  // #endif

  #ifdef I2CIP_DEBUG_SERIAL
    DEBUG_DELAY();
    I2CIP_DEBUG_SERIAL.print(F("K30 Init... "));
    DEBUG_DELAY();
  #endif
  size_t len = 4;
  uint8_t tx[len] = {
    0x22, // Read Ram (2), 2 Bytes
    0x00, 0x08, // Ram Address 0x08
    0x2A // Checksum == 0x22 + 0x00 + 0x08
  };
  
  // K30 HAS VERY SPECIFIC TIMING REQUIREMENTS
  I2CIP_FQA_TO_WIRE(this->fqa)->end();
  pinMode(SDA, OUTPUT);
  digitalWrite(SDA, LOW);
  delayMicroseconds(300);
  digitalWrite(SDA, HIGH);
  delayMicroseconds(1000);
  I2CIP_FQA_TO_WIRE(this->fqa)->begin();

  // Instruct to take a reading
  i2cip_errorlevel_t errlev = write(tx, len, false); // NO BUS SETTING
  I2CIP_ERR_BREAK(errlev);

  #ifdef I2CIP_DEBUG_SERIAL
    DEBUG_DELAY();
    I2CIP_DEBUG_SERIAL.print(F("Read... "));
    DEBUG_DELAY();
  #endif

  #ifdef WIIPOD_K30_DELAY
    delay(WIIPOD_K30_DELAY);
  #endif

  // Request reading
  size_t i = 4; uint8_t rx[i] = { 0 };
  errlev = read(rx, i, false, false, false); // NO BUS SETTING
  I2CIP_ERR_BREAK(errlev);

  if(i != 4) {
    return I2CIP_ERR_SOFT;
  }

  uint8_t checksum = rx[0] + rx[1] + rx[2];

  #ifdef I2CIP_DEBUG_SERIAL
    DEBUG_DELAY();
    I2CIP_DEBUG_SERIAL.print(F("RX: ["));
    for (size_t j = 0; j < i; j++) {
      I2CIP_DEBUG_SERIAL.print(rx[j], HEX);
      I2CIP_DEBUG_SERIAL.print(F(" "));
      if(i - j != 1) { I2CIP_DEBUG_SERIAL.print(F(", ")); }
    }
    I2CIP_DEBUG_SERIAL.print(F("], Checksum: "));
    I2CIP_DEBUG_SERIAL.print(rx[0], HEX);
    I2CIP_DEBUG_SERIAL.print(F(" + "));
    I2CIP_DEBUG_SERIAL.print(rx[1], HEX);
    I2CIP_DEBUG_SERIAL.print(F(" + "));
    I2CIP_DEBUG_SERIAL.print(rx[2], HEX);
    I2CIP_DEBUG_SERIAL.print(F(" -> "));
    I2CIP_DEBUG_SERIAL.print(checksum, HEX);
  #endif
  
  if (checksum == rx[3]) {
    #ifdef I2CIP_DEBUG_SERIAL
      I2CIP_DEBUG_SERIAL.print(F(" == "));
      I2CIP_DEBUG_SERIAL.println(rx[3], HEX);
      DEBUG_DELAY();
    #endif
    dest = ((uint16_t)rx[1] << 8) + rx[2];
  } else {
    #ifdef I2CIP_DEBUG_SERIAL
      I2CIP_DEBUG_SERIAL.print(F(" != "));
      I2CIP_DEBUG_SERIAL.println(rx[3], HEX);
      DEBUG_DELAY();
    #endif
    errlev = I2CIP_ERR_SOFT;
  }

  return errlev;
}