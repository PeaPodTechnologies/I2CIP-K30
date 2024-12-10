#ifndef WIIPOD_K30_H
#define WIIPOD_K30_H

#include <Arduino.h>

#include <I2CIP.h>

#define K30_ADDRESS 0x68
#define WIIPOD_K30_DELAY 10

const char wiipod_k30_id_progmem[] PROGMEM = {"K30"};

class K30 : public I2CIP::Device, public I2CIP::InputInterface<uint16_t, void*> {
  private:
      static bool _id_set;
      static char _id[]; // to be loaded from progmem

      bool initialized = false;

      void* const isnull = nullptr;
      K30(const i2cip_fqa_t& fqa);

      #ifdef MAIN_CLASS_NAME
      friend class MAIN_CLASS_NAME;
      #endif
    public:
      K30(const i2cip_fqa_t& fqa, const i2cip_id_t& id);

      static Device* k30Factory(const i2cip_fqa_t& fqa, const i2cip_id_t& id);
      static Device* k30Factory(const i2cip_fqa_t& fqa);

      static void loadID(void);

      virtual ~K30() { }

      // static const char* getStaticIDBuffer() { return K30::_id_set ? K30::_id : nullptr; } // Harsh but fair
      static const char* getStaticIDBuffer() { return K30::_id; }

      /**
       * Read from the Nunchuck.
       * @param dest Destination heap (pointer reassigned, not overwritten)
       * @param args Number of bytes to read
       **/
      i2cip_errorlevel_t get(uint16_t& dest, void* const& args = nullptr) override;

      void clearCache(void) override;
      void* const& getDefaultA(void) const override { return this->isnull; };
};

#endif