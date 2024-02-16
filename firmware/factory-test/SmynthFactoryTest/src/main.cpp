#include <Arduino.h>
#include <Wire.h>

#include <driver/i2s.h>
#include <hal/i2s_types.h>

#include "SyEngine.hpp"
#include "hal/i2c_register_based_protocol.h"

#define DAC_ADDRESS 0x10

#define ES8388_REG_CHIPCONTROL1 0x00
#define ES8388_REG_CHIPCONTROL2 0x01
#define ES8388_REG_CHIPPOWERMGMT 0x02
#define ES8388_REG_DACPOWER 0x04
#define ES8388_REG_MASTERMODE 0x08
#define ES8388_REG_DACCONTROL1 0x17
#define ES8388_REG_DACCONTROL2 0x18
#define ES8388_REG_DACCONTROL3 0x19
#define ES8388_REG_LDACVOL 0x1a
#define ES8388_REG_RDACVOL 0x1b
#define ES8388_REG_DACCONTROL17 0x27
#define ES8388_REG_DACCONTROL20 0x2a
#define ES8388_REG_DACCONTROL21 0x2b
#define ES8388_REG_LOUT1VOL 0x2e
#define ES8388_REG_ROUT1VOL 0x2f
#define ES8388_REG_LOUT2VOL 0x30
#define ES8388_REG_ROUT2VOL 0x31

#define ES8388_REG_CHIPCONTROL1_DACADC_SAME_FS 0x10
#define ES8388_REG_CHIPCONTROL1_VMIDSEL_500K 0x12

#define ES8388_REG_CHIPPOWER_RESET 0xf0
#define ES8388_REG_CHIPCONTROL2_LPVREFBUF 0x10
#define ES8388_REG_DACPOWER_LOUT1_EN 0x20
#define ES8388_REG_DACPOWER_LOUT2_EN 0x08
#define ES8388_REG_DACPOWER_ROUT1_EN 0x10
#define ES8388_REG_DACPOWER_ROUT2_EN 0x04

#define ES8388_REG_DACCONTROL1_WORDLEN_16BIT 0x18

#define ES8388_REG_DACCONTROL2_DACFSRATIO_256 0x02

#define ES8388_REG_DACCONTROL17_DAC_TO_MIX 0x80
#define ES8388_REG_DACCONTROL20_DAC_TO_MIX 0x80

#define ES8388_REG_MASTERMODE_MASTER 0x80
#define ES8388_REG_MASTERMODE_MCLKDIV2 0x40

#define ES8388_REG_DACCONTROL21_DACADC_LRCK_SAME 0x80

#define I2S_NUM i2s_port_t::I2S_NUM_0

QueueHandle_t i2cQueue;

SyEngine syEngine;

#define DMA_BUFFER_LEN SAMPLE_BUFFER_FRAME_COUNT
#define NUM_CHANNELS 2
#define SAMPLE_BUFFER_FRAME_COUNT 1024
#define SAMPLE_BUFFER_NUM_SAMPLES (SAMPLE_BUFFER_FRAME_COUNT * NUM_CHANNELS)
#define SAMPLE_BUFFER_SIZE (SAMPLE_BUFFER_NUM_SAMPLES * sizeof(uint16_t))

int16_t waveFormBuffer[SAMPLE_BUFFER_NUM_SAMPLES];

void createWaveform()
{
  syEngine.configureToneGenerator();
  /*  for (int i = 0; i < WAVEFORMLEN; i++)
    {
      waveFormChannels[i * 2] = sin(6.28 * i / 44100 * 440) * 15000 * ((double)abs(11000 - i) / 11000);
      waveFormChannels[i * 2 + 1] = sin(6.28 * i / 44100 * 660) * 15000 * ((double)abs(11000 - i) / 11000);
    }*/
}

void setup()
{
  int res = 0;

  delay(1000);

  Serial.begin(115200);
  Serial.printf("Serial start\n");

  Wire.setPins(33, 32);
  Wire.begin();

  i2c_regp_write_register(DAC_ADDRESS, ES8388_REG_CHIPCONTROL1, ES8388_REG_CHIPCONTROL1_DACADC_SAME_FS | ES8388_REG_CHIPCONTROL1_VMIDSEL_500K);
  i2c_regp_write_register(DAC_ADDRESS, ES8388_REG_CHIPCONTROL2, ES8388_REG_CHIPCONTROL2_LPVREFBUF);
  i2c_regp_write_register(DAC_ADDRESS, ES8388_REG_CHIPPOWERMGMT, 0);
  i2c_regp_write_register(DAC_ADDRESS, ES8388_REG_MASTERMODE, 0);

  i2c_regp_write_register(DAC_ADDRESS, ES8388_REG_DACCONTROL1, ES8388_REG_DACCONTROL1_WORDLEN_16BIT);
  i2c_regp_write_register(DAC_ADDRESS, ES8388_REG_DACCONTROL2, ES8388_REG_DACCONTROL2_DACFSRATIO_256);
  i2c_regp_write_register(DAC_ADDRESS, ES8388_REG_DACCONTROL17, ES8388_REG_DACCONTROL17_DAC_TO_MIX | (2 << 3));
  i2c_regp_write_register(DAC_ADDRESS, ES8388_REG_DACCONTROL20, ES8388_REG_DACCONTROL20_DAC_TO_MIX | (2 << 3));
  i2c_regp_write_register(DAC_ADDRESS, ES8388_REG_DACCONTROL21, ES8388_REG_DACCONTROL21_DACADC_LRCK_SAME);

  i2c_regp_write_register(DAC_ADDRESS, ES8388_REG_DACPOWER, ES8388_REG_DACPOWER_LOUT1_EN | ES8388_REG_DACPOWER_LOUT2_EN | ES8388_REG_DACPOWER_ROUT1_EN | ES8388_REG_DACPOWER_ROUT2_EN);
  i2c_regp_write_register(DAC_ADDRESS, ES8388_REG_DACPOWER, ES8388_REG_DACPOWER_LOUT1_EN | ES8388_REG_DACPOWER_LOUT2_EN | ES8388_REG_DACPOWER_ROUT1_EN | ES8388_REG_DACPOWER_ROUT2_EN);

  i2c_regp_write_register(DAC_ADDRESS, ES8388_REG_LOUT1VOL, 0xa);
  i2c_regp_write_register(DAC_ADDRESS, ES8388_REG_ROUT1VOL, 0xa);
  i2c_regp_write_register(DAC_ADDRESS, ES8388_REG_LOUT2VOL, 0xa);
  i2c_regp_write_register(DAC_ADDRESS, ES8388_REG_ROUT2VOL, 0xa);

  i2c_regp_write_register(DAC_ADDRESS, ES8388_REG_LDACVOL, 0);
  i2c_regp_write_register(DAC_ADDRESS, ES8388_REG_RDACVOL, 0);

  i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(i2s_mode_t::I2S_MODE_MASTER | i2s_mode_t::I2S_MODE_TX),
      .sample_rate = 44100,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // Interrupt level 1, default 0
      .dma_buf_count = 3,
      .dma_buf_len = DMA_BUFFER_LEN,
      .use_apll = true,
      .tx_desc_auto_clear = false,
      .fixed_mclk = 0,
      .mclk_multiple = I2S_MCLK_MULTIPLE_256,
      .bits_per_chan = I2S_BITS_PER_CHAN_DEFAULT};

  static const i2s_pin_config_t pin_config = {
      .mck_io_num = 0,
      .bck_io_num = 27,
      .ws_io_num = 25,
      .data_out_num = 26,
      .data_in_num = I2S_PIN_NO_CHANGE};

  res = i2s_driver_install(I2S_NUM, &i2s_config, 16, &i2cQueue);
  Serial.printf("i2s_driver_install %d\n", res);

  res = i2s_set_pin(I2S_NUM, &pin_config);
  Serial.printf("i2s_set_pin %d\n", res);

  setCpuFrequencyMhz(240);
}

int loopIter = 0;
void loop()
{
  int microsPreRender = micros();
  syEngine.renderSamples(waveFormBuffer, 2, SAMPLE_BUFFER_FRAME_COUNT);
  int microsPostRender = micros();

  size_t written = 0;
  int res = i2s_write(I2S_NUM, waveFormBuffer, SAMPLE_BUFFER_SIZE, &written, portMAX_DELAY);
  int microsPostWrite = micros();

  loopIter++;

  if (!(loopIter & 0xf) || (loopIter < 10))
  {
    Serial.printf("CPU freq %d\n", getCpuFrequencyMhz());
    Serial.printf("[%d] Time for generating %d samples: %dus; write:%dus; Res=%d; Written = %d\n", loopIter, SAMPLE_BUFFER_FRAME_COUNT, microsPostRender - microsPreRender, microsPostWrite - microsPostRender, res, written);
  }
}