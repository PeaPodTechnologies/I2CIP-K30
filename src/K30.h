#ifndef I2CIP_K30_H
#define I2CIP_K30_H

#include <Arduino.h>

#include <I2CIP.h>

#define I2CIP_K30_ADDRESS 0x68
#define I2CIP_K30_DELAY 10

#ifdef I2CIP_USE_GUARANTEES
#define I2CIP_GUARANTEE_K30 30
class K30;
I2CIP_GUARANTEE_DEFINE(K30, I2CIP_GUARANTEE_K30);
#endif

// const char i2cip_k30_id_progmem[] PROGMEM = {"K30"};

class K30 : public I2CIP::Device, public I2CIP::InputInterface<uint16_t, void*>
  #ifdef I2CIP_USE_GUARANTEES
  , public Guarantee<K30>
  #endif
  {
  I2CIP_DEVICE_CLASS_BUNDLE(K30);
  I2CIP_INPUT_USE_RESET(uint16_t, void*, void* const);

  I2CIP_INPUT_USE_TOSTRING(uint16_t, "%u");
  I2CIP_INPUT_ADD_PRINTCACHE(uint16_t, "%u ppm CO2");
  
  #ifdef I2CIP_USE_GUARANTEES
  I2CIP_CLASS_USE_GUARANTEE(K30, I2CIP_GUARANTEE_K30);
  #endif
  
  private:
      bool initialized = false;

      #ifdef MAIN_CLASS_NAME
      friend class MAIN_CLASS_NAME;
      #endif
    public:
      K30(i2cip_fqa_t fqa, const i2cip_id_t& id);

      virtual ~K30() { }

      /**
       * Read from the Nunchuck.
       * @param dest Destination heap (pointer reassigned, not overwritten)
       * @param args Number of bytes to read
       **/
      i2cip_errorlevel_t get(uint16_t& dest, void* const& args = nullptr) override;
};

#endif