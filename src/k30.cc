#include <k30.h>

bool K30::_id_set = false;
char K30::_id[I2CIP_ID_SIZE];

using namespace I2CIP;

void K30::loadID(void) {
  uint8_t idlen = strlen_P(wiipod_k30_id_progmem);

  // Read in PROGMEM
  for (uint8_t k = 0; k < idlen; k++) {
    char c = pgm_read_byte_near(wiipod_k30_id_progmem + k);
    K30::_id[k] = c;
  }

  K30::_id[idlen] = '\0';
  K30::_id_set = true;

  #ifdef WIIPOD_DEBUG_SERIAL
    DEBUG_DELAY();
    WIIPOD_DEBUG_SERIAL.print(F("K30 ID Loaded: '"));
    WIIPOD_DEBUG_SERIAL.print(K30::_id);
    WIIPOD_DEBUG_SERIAL.print(F("' @0x"));
    WIIPOD_DEBUG_SERIAL.println((uintptr_t)(&K30::_id[0]), HEX);
    DEBUG_DELAY();
  #endif
}

// Handles ID pointer assignment too
// NEVER returns nullptr, unless out of memory
Device* K30::k30Factory(const i2cip_fqa_t& fqa, const i2cip_id_t& id) {
  if(!K30::_id_set || id == nullptr) {
    loadID();

    (Device*)(new K30(fqa, id == nullptr ? _id : id));
  }

  return (Device*)(new K30(fqa, id));
}

Device* K30::k30Factory(const i2cip_fqa_t& fqa) { return k30Factory(fqa, K30::getStaticIDBuffer()); }

K30::K30(const i2cip_fqa_t& fqa, const i2cip_id_t& id) : Device(fqa, id), InputInterface<uint16_t, void*>((Device*)this) { }
K30::K30(const i2cip_fqa_t& fqa) : K30(fqa, K30::_id) { }

// K30::K30(const uint8_t& wire, const uint8_t& module, const uint8_t& addr) : K30(I2CIP_FQA_CREATE(wire, module, I2CIP_MUX_BUS_DEFAULT, addr)) { }

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
    clearCache();
    errlev = I2CIP_ERR_SOFT;
  }

  return errlev;
}

// G - Getter type: char* (null-terminated; writable heap)
void K30::clearCache(void) {
  this->setCache(0);

  #ifdef I2CIP_DEBUG_SERIAL
    DEBUG_DELAY();
    I2CIP_DEBUG_SERIAL.print(F("K30 Cache Cleared (Zeroed)\n"));
    DEBUG_DELAY();
  #endif
}