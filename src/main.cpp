#include <Arduino.h>
#include <SPI.h>
#include <BLEPeripheral.h>

// This struct is based on the format defined at https://bthome.io/format/
//
// you should customise it to include the exact sensors that you want to
// advertise.
// The bleperipheral lib will already add the flags, localname and uuid16,
// so we add the deviceinfo flags and the individual sensor entries.
//
// Note that sensors should be added in numerical order of their ID.
// This one works (sort of) for sending via bleperipheral broadcast(),
// but is too restrictive (we can't bundle the name, flags etc)
struct BTHomeData {
  char devinfo = 0x40; // non-encrypted, v2 format
  const char flagtemp = 0x02; // temperature, decicelcius
  uint16_t temp = 2500;
};

static struct BTHomeData btHomeData;

void updateAdvertisingScanData();
unsigned char getBatteryLevel();

BLEPeripheral blePeripheral = BLEPeripheral();

BLEService bthomeService = BLEService("FCD2");
BLEFixedLengthCharacteristic bthome = BLEFixedLengthCharacteristic("FCD2", BLEBroadcast, sizeof(btHomeData));

unsigned long interval = 4000; // number of milliseconds between broadcast updates
unsigned long saved_stamp = 0; // millies stamp of last broadcast run.

const char localName[] = "AJGBleTest";
#define TX_POWER 0x04;
#define VBAT_MAX_IN_MV 3000
int g_counterValue = 3168;


void setup()
{
  //strcpy(btHomeData.name, (char *) "ajg-test");


  blePeripheral.setLocalName(localName);
  // name disappears if we disable connect :-/
  //blePeripheral.setConnectable(false);
//  blePeripheral.addAttribute(bthomeService);
//  blePeripheral.addAttribute(bthome);
  blePeripheral.begin();

  // delay the manual advdata update so we can get our name out there...
  saved_stamp = millis();
}

void loop()
{
  if ( (millis() - saved_stamp ) > interval ){
    saved_stamp = millis();

    // send btHome data via bleperipheral lib:
    /**
    btHomeData.temp ++;
    bthome.setValue( (unsigned char *)&btHomeData, sizeof(btHomeData) );
    bthome.broadcast();
    **/

    // send data via nrf direct call:
    btHomeData.temp ++;
    updateAdvertisingScanData();


    //sd_app_evt_wait();
  };

}

// lifted from https://github.com/sandeepmistry/arduino-BLEPeripheral/issues/272#issue-497748815
// and modified to suit
void updateAdvertisingScanData()
{
  static unsigned char srData[31];
  unsigned char srDataLen = 0;
  int scanDataSize = 5; // FIXME: must manually ensure this is more than the number of elements you add.
  BLEEirData scanData[scanDataSize];

  int i = 0;

  // - Flags
  scanData[i].length = 0x01;
  scanData[i].type = 0x01;
  scanData[i].data[0] = 0x06;


//**
  // - Local name
  i++;
  scanData[i].length = strlen(localName);
  scanData[i].type = 0x08;
  memcpy(scanData[i].data, localName, strlen(localName));
//**/


  // - BTHome Advertisement

  char bthomepacket[] = {
    // UUID
    0xD2, 0xFC, // UUID in reverse-byte order.

    // device info: bit0: encryption, bit5-7: version
    0x40, // v2, plaintext

    // BTHome Object entries. Ensure they are listed here in NUMERICAL order of type.

    // battery
    0x01, // battery %
    getBatteryLevel(),

    // temperature
    0x02, // temperature
    btHomeData.temp & 0xFF,
    btHomeData.temp >> 8 & 0xFF,

  };

  i++;
  scanData[i].length = sizeof(bthomepacket);
  scanData[i].type = 0x16; // 16bituuid service data
  memcpy(scanData[i].data, bthomepacket, sizeof(bthomepacket));

/**
  // - Tx Power
  i++;
  scanData[i].length = 1;
  scanData[i].type = 0x0A;
  scanData[i].data[0] = TX_POWER;

  // - Manufacturer Data
  i++;
  scanData[i].length = 2 + 4;
  scanData[i].type = 0xFF;
  // Manufacturer ID
  scanData[i].data[0] = 0xFF;
  scanData[i].data[1] = 0xFF;
  // Manufacturer data content
  scanData[i].data[2] = g_counterValue & 0xFF;
  scanData[i].data[3] = (g_counterValue >> 8) & 0xFF;
  scanData[i].data[4] = (g_counterValue >> 16) & 0xFF;
  scanData[i].data[5] = (g_counterValue >> 24) & 0xFF;
**/

  scanDataSize = i +1;

  if (scanDataSize && scanData)
  {
    for (int j = 0; j < scanDataSize; j++)
    {
      srData[srDataLen + 0] = scanData[j].length + 1;
      srData[srDataLen + 1] = scanData[j].type;
      srDataLen += 2;

      memcpy(&srData[srDataLen], scanData[j].data, scanData[j].length);

      srDataLen += scanData[j].length;
    }
  }

  // - Sets only avertising scan data
  sd_ble_gap_adv_data_set(srData, srDataLen, NULL, 0);
}


unsigned char getBatteryLevel(void)
{
  // Configure ADC
  NRF_ADC->CONFIG = (ADC_CONFIG_RES_8bit << ADC_CONFIG_RES_Pos) |
                    (ADC_CONFIG_INPSEL_SupplyOneThirdPrescaling << ADC_CONFIG_INPSEL_Pos) |
                    (ADC_CONFIG_REFSEL_VBG << ADC_CONFIG_REFSEL_Pos) |
                    (ADC_CONFIG_PSEL_Disabled << ADC_CONFIG_PSEL_Pos) |
                    (ADC_CONFIG_EXTREFSEL_None << ADC_CONFIG_EXTREFSEL_Pos);
  NRF_ADC->EVENTS_END = 0;
  NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;

  NRF_ADC->EVENTS_END = 0; // Stop any running conversions.
  NRF_ADC->TASKS_START = 1;

  while (!NRF_ADC->EVENTS_END)
  {
  }

  uint16_t vbg_in_mv = 1200;
  uint8_t adc_max = 255;
  uint16_t vbat_current_in_mv = (NRF_ADC->RESULT * 3 * vbg_in_mv) / adc_max;

  NRF_ADC->EVENTS_END = 0;
  NRF_ADC->TASKS_STOP = 1;
  NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Disabled;

  return (unsigned char)((vbat_current_in_mv * 100) / VBAT_MAX_IN_MV);
}