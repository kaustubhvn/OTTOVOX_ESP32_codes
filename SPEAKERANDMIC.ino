#include <driver/i2s.h>
#include <SD.h>

#define SPI_MISO 5
#define SPI_MOSI 6
#define SPI_SCK 4
#define SPI_CS 7
#define I2S_WS 10
#define I2S_SD 8
#define I2S_SCK 9
#define I2S_PORT I2S_NUM_0
#define I2S_PLAY_PORT I2S_NUM_1
#define I2S_SAMPLE_RATE   (16000)
#define I2S_SAMPLE_BITS   (16)
#define I2S_READ_LEN      (16 * 1024)
#define RECORD_TIME       (10) //Seconds
#define I2S_CHANNEL_NUM   (1)
#define FLASH_RECORD_SIZE (I2S_CHANNEL_NUM * I2S_SAMPLE_RATE * I2S_SAMPLE_BITS / 8 * RECORD_TIME)

#define I2S_DOUT       1
#define I2S_BCLK       2
#define I2S_LRC        3

File file;
const char filename[] = "/recording2.wav";
const int headerSize = 44;

void setup() {
  Serial.begin(9600);
  SDCardInit();
  i2sInit();
  xTaskCreate(i2s_adc, "i2s_adc", 1024 * 2, NULL, 1, NULL);
}

void loop() {
  // Main loop is left empty as recording and playback are handled by tasks
}

void SDCardInit() {
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_CS);

  if (!SD.begin(SPI_CS)) {
    Serial.println("SD card initialization failed!");
    while (1) yield();
  }

  if (SD.exists(filename)) {
    SD.remove(filename);
  }

  file = SD.open(filename, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to create file on SD card!");
    while (1) yield();
  }

  byte header[headerSize];
  wavHeader(header, FLASH_RECORD_SIZE);
  file.write(header, headerSize);
  Serial.println("SD card initialized and WAV header written.");
}

void i2sInit() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = I2S_SAMPLE_RATE,
    .bits_per_sample = i2s_bits_per_sample_t(I2S_SAMPLE_BITS),
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = 0,
    .dma_buf_count = 64,
    .dma_buf_len = 1024,
    .use_apll = 1
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);

  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };

  i2s_set_pin(I2S_PORT, &pin_config);
}

void i2sPlaybackInit() {
  i2s_config_t i2s_play_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = I2S_SAMPLE_RATE,
    .bits_per_sample = i2s_bits_per_sample_t(I2S_SAMPLE_BITS),
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = 1024,
    .use_apll = 1
  };

  i2s_driver_install(I2S_PLAY_PORT, &i2s_play_config, 0, NULL);

  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCLK,
    .ws_io_num = I2S_LRC,
    .data_out_num = I2S_DOUT,
    .data_in_num = -1
  };

  i2s_set_pin(I2S_PLAY_PORT, &pin_config);
}

void i2s_adc(void *arg) {
  int i2s_read_len = I2S_READ_LEN;
  int flash_wr_size = 0;
  size_t bytes_read;

  char* i2s_read_buff = (char*) calloc(i2s_read_len, sizeof(char));
  uint8_t* flash_write_buff = (uint8_t*) calloc(i2s_read_len, sizeof(char));

  i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);
  i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);

  Serial.println(" * Recording Start * ");
  while (flash_wr_size < FLASH_RECORD_SIZE) {
    i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);
    file.write((const byte*) i2s_read_buff, bytes_read);
    flash_wr_size += bytes_read;
    Serial.printf("Sound recording %u%%\n", flash_wr_size * 100 / FLASH_RECORD_SIZE);
  }

  file.close();
  free(i2s_read_buff);
  free(flash_write_buff);

  Serial.println("Recording completed and file saved on SD card.");

  i2sPlaybackInit();
  xTaskCreate(playbackAudio, "playbackAudio", 4096, NULL, 1, NULL);

  vTaskDelete(NULL);
}

void playbackAudio(void *arg) {
  file = SD.open(filename, FILE_READ);
  if (!file) {
    Serial.println("Failed to open file for playback!");
    vTaskDelete(NULL);
  }

  file.seek(headerSize); // Skip WAV header

  char* i2s_write_buff = (char*) malloc(I2S_READ_LEN);
  size_t bytes_written;

  float volumeGain = 4.0; // Increase volume (1.0 = original, >1.0 = louder)

  Serial.println(" * Playback Start * ");
  while (file.available()) {
    size_t bytes_to_read = min(file.available(), (int)I2S_READ_LEN);
    file.read((uint8_t*)i2s_write_buff, bytes_to_read);

    // Apply volume gain
    for (size_t i = 0; i < bytes_to_read / 2; i++) {
      int16_t sample = ((int16_t*)i2s_write_buff)[i]; // Read 16-bit sample
      sample = (int16_t)(sample * volumeGain);        // Apply gain
      if (sample > 32767) sample = 32767;            // Clip to max value
      if (sample < -32768) sample = -32768;          // Clip to min value
      ((int16_t*)i2s_write_buff)[i] = sample;        // Write back modified sample
    }

    i2s_write(I2S_PLAY_PORT, (const char*)i2s_write_buff, bytes_to_read, &bytes_written, portMAX_DELAY);
  }

  // Send silence to flush the buffers
  memset(i2s_write_buff, 0, I2S_READ_LEN);
  for (int i = 0; i < 5; i++) {
    i2s_write(I2S_PLAY_PORT, i2s_write_buff, I2S_READ_LEN, &bytes_written, portMAX_DELAY);
  }

  free(i2s_write_buff);
  file.close();

  // Stop and uninstall the I2S driver
  i2s_zero_dma_buffer(I2S_PLAY_PORT);
  i2s_driver_uninstall(I2S_PLAY_PORT);

  Serial.println("Playback completed and I2S driver stopped.");
  vTaskDelete(NULL);
}



void wavHeader(byte* header, int wavSize) {
  header[0] = 'R';
  header[1] = 'I';
  header[2] = 'F';
  header[3] = 'F';
  unsigned int fileSize = wavSize + headerSize - 8;
  header[4] = (byte)(fileSize & 0xFF);
  header[5] = (byte)((fileSize >> 8) & 0xFF);
  header[6] = (byte)((fileSize >> 16) & 0xFF);
  header[7] = (byte)((fileSize >> 24) & 0xFF);
  header[8] = 'W';
  header[9] = 'A';
  header[10] = 'V';
  header[11] = 'E';
  header[12] = 'f';
  header[13] = 'm';
  header[14] = 't';
  header[15] = ' ';
  header[16] = 0x10;
  header[17] = 0x00;
  header[18] = 0x00;
  header[19] = 0x00;
  header[20] = 0x01;
  header[21] = 0x00;
  header[22] = 0x01;
  header[23] = 0x00;
  header[24] = 0x80;
  header[25] = 0x3E;
  header[26] = 0x00;
  header[27] = 0x00;
  header[28] = 0x00;
  header[29] = 0x7D;
  header[30] = 0x00;
  header[31] = 0x00;
  header[32] = 0x02;
  header[33] = 0x00;
  header[34] = 0x10;
  header[35] = 0x00;
  header[36] = 'd';
  header[37] = 'a';
  header[38] = 't';
  header[39] = 'a';
  header[40] = (byte)(wavSize & 0xFF);
  header[41] = (byte)((wavSize >> 8) & 0xFF);
  header[42] = (byte)((wavSize >> 16) & 0xFF);
  header[43] = (byte)((wavSize >> 24) & 0xFF);
}