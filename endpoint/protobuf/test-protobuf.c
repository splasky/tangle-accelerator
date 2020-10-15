#include <mbedtls/cipher.h>
#include <mbedtls/md.h>
#include <mbedtls/platform_util.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "b64.c/b64.h"
#include "brotli/decode.h"
#include "brotli/encode.h"
#include "obd2.pb-c.h"

#define mbedtls_fprintf fprintf
#define mbedtls_printf printf

#define MODE_ENCRYPT 0
#define MODE_DECRYPT 1
#define MBEDTLS_EXIT_SUCCESS EXIT_SUCCESS
#define MBEDTLS_EXIT_FAILURE EXIT_FAILURE

#define TEST_PRIVATE_KEY "ABCDEFGGGGGGGGGGGGGGGGGGGGG"

int string_compress(uint8_t* origin, size_t len, uint8_t** out_buffer, size_t* out_size) {
  int lgwin = BROTLI_DEFAULT_WINDOW;
  int bufferSize = len;
  uint8_t* buffer = malloc(bufferSize);
  uint8_t* inBuffer = malloc(len);

  memcpy(inBuffer, origin, len);

  int quality = 11;
  size_t encodedSize = bufferSize;

  bool brotliStatus;

  brotliStatus =
      BrotliEncoderCompress(quality, lgwin, BROTLI_MODE_GENERIC, len, (const uint8_t*)inBuffer, &encodedSize, buffer);

  printf("origin: %zu, compress: %zu\n", len, encodedSize);

  *out_buffer = buffer;
  *out_size = encodedSize;

  free(inBuffer);
}

int aes_256_cbc_encrypt(uint8_t* plaintext, size_t plaintext_len, char* key, uint8_t* ciphertext,
                        size_t* ciphertext_len, uint8_t* hmac, uint8_t* iv) {
  unsigned char IV[16];
  size_t keylen = strlen(key);
  unsigned char digest[MBEDTLS_MD_MAX_SIZE];
  unsigned char buffer[1024];
  unsigned char output[1024];
  int exit_code = MBEDTLS_EXIT_FAILURE;

  const mbedtls_cipher_info_t* cipher_info;
  const mbedtls_md_info_t* md_info;
  mbedtls_cipher_context_t cipher_ctx;
  mbedtls_md_context_t md_ctx;

  mbedtls_cipher_init(&cipher_ctx);
  mbedtls_md_init(&md_ctx);

  cipher_info = mbedtls_cipher_info_from_string("AES-256-CBC");

  int ret = 0;
  if ((ret = mbedtls_cipher_setup(&cipher_ctx, cipher_info)) != 0) {
    mbedtls_fprintf(stderr, "mbedtls_cipher_setup failed\n");
    goto exit;
  }

  md_info = mbedtls_md_info_from_string("SHA256");

  if (mbedtls_md_setup(&md_ctx, md_info, 1) != 0) {
    mbedtls_fprintf(stderr, "mbedtls_md_setup failed\n");
    goto exit;
  }

  // start aes cipher
  mbedtls_md_starts(&md_ctx);
  mbedtls_md_update(&md_ctx, buffer, 8);
  mbedtls_md_finish(&md_ctx, digest);

  memcpy(IV, digest, 16);

  /*
   * Append the IV at the beginning of the output.
   */
  memcpy(iv, IV, 16);
  /*
   * Hash the IV and the secret key together 8192 times
   * using the result to setup the AES context and HMAC.
   */
  memset(digest, 0, 32);
  memcpy(digest, IV, 16);

  for (int i = 0; i < 8192; i++) {
    mbedtls_md_starts(&md_ctx);
    mbedtls_md_update(&md_ctx, digest, 32);
    mbedtls_md_update(&md_ctx, (uint8_t*)key, keylen);
    mbedtls_md_finish(&md_ctx, digest);
  }

  if (mbedtls_cipher_setkey(&cipher_ctx, digest, cipher_info->key_bitlen, MBEDTLS_ENCRYPT) != 0) {
    mbedtls_fprintf(stderr, "mbedtls_cipher_setkey() returned error\n");
    goto exit;
  }
  if (mbedtls_cipher_set_iv(&cipher_ctx, IV, 16) != 0) {
    mbedtls_fprintf(stderr, "mbedtls_cipher_set_iv() returned error\n");
    goto exit;
  }
  if (mbedtls_cipher_reset(&cipher_ctx) != 0) {
    mbedtls_fprintf(stderr, "mbedtls_cipher_reset() returned error\n");
    goto exit;
  }

  mbedtls_md_hmac_starts(&md_ctx, digest, 32);

  /*
   * Encrypt and write the ciphertext.
   */
  int offset;
  size_t ilen, olen;
  size_t length = 0;
  for (offset = 0; offset < plaintext_len; offset += mbedtls_cipher_get_block_size(&cipher_ctx)) {
    ilen = (plaintext_len - offset > mbedtls_cipher_get_block_size(&cipher_ctx))
               ? mbedtls_cipher_get_block_size(&cipher_ctx)
               : (unsigned int)(plaintext_len - offset);

    memcpy(buffer, &plaintext[offset], ilen);
    if (mbedtls_cipher_update(&cipher_ctx, buffer, ilen, output, &olen) != 0) {
      mbedtls_fprintf(stderr, "mbedtls_cipher_update() returned error\n");
      goto exit;
    }

    mbedtls_md_hmac_update(&md_ctx, output, olen);
    memcpy(&ciphertext[offset], output, olen);

    length += olen;
  }

  if (mbedtls_cipher_finish(&cipher_ctx, output, &olen) != 0) {
    mbedtls_fprintf(stderr, "mbedtls_cipher_finish() returned error\n");
    goto exit;
  }
  mbedtls_md_hmac_update(&md_ctx, output, olen);
  memcpy(&ciphertext[offset], output, olen);
  length += olen;
  *ciphertext_len = length;

  /*
   * Finally write the HMAC.
   */
  mbedtls_md_hmac_finish(&md_ctx, digest);
  memcpy(hmac, digest, mbedtls_md_get_size(md_info));
  exit_code = 0;

exit:

  mbedtls_cipher_free(&cipher_ctx);
  mbedtls_md_free(&md_ctx);

  return exit_code;
}

int main(int argc, const char* argv[]) {
  Obd2__OBD2Data obd2 = OBD2__OBD2_DATA__INIT;
  Obd2__OBD2MsgStruct msg_struct = OBD2__OBD2_MSG_STRUCT__INIT;
  Obd2__MsgStruct msg = OBD2__MSG_STRUCT__INIT;

  void* obd2_buf;
  void* msg_struct_buf;
  void* msg_buf;

  unsigned obd2_buf_len;
  unsigned msg_struct_buf_len;
  unsigned msg_buf_len;

  obd2.vin = "1234567890ABCDEF";
  obd2.engine_load = 0XFF;
  obd2.engine_coolant_temperature = 0XFF;
  obd2.fuel_pressure = 0XFF;
  obd2.engine_speed = 0XFF;
  obd2.vehicle_speed = 0XFF;
  obd2.intake_air_temperature = 0XFF;
  obd2.mass_air_flow = 0XFF;
  obd2.fuel_tank_level_input = 0XFF;
  obd2.absolute_barometric_pressure = 0XFF;
  obd2.control_module_voltage = 0XFF;
  obd2.throttle_position = 0XFF;
  obd2.ambient_air_temperature = 0XFF;
  obd2.relative_accelerator_pedal_position = 0XFF;
  obd2.engine_oil_temperature = 0XFF;
  obd2.engine_fuel_rate = 0XFF;
  obd2.service_distance = 0XFFFF;
  obd2.anti_lock_barking_active = 0XFFFF;
  obd2.steering_wheel_angle = 0XFFFF;
  obd2.position_of_doors = 0XFFFF;
  obd2.right_left_turn_signal_light = 0XFFFF;
  obd2.alternate_beam_head_light = 0XFFFF;
  obd2.high_beam_head_light = 0XFFFF;

  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);

  msg_struct.deviceid = "1234567890ABCDEFG";
  msg_struct.timestamp = t.tv_sec;
  msg_struct.obd2_data = &obd2;

  obd2_buf_len = obd2__obd2_data__get_packed_size(&obd2);
  msg_struct_buf_len = obd2__obd2_msg_struct__get_packed_size(&msg_struct);

  obd2_buf = malloc(obd2_buf_len);
  msg_struct_buf = malloc(msg_struct_buf_len);

  obd2__obd2_data__pack(&obd2, obd2_buf);
  obd2__obd2_msg_struct__pack(&msg_struct, msg_struct_buf);

  size_t encodeSize = 0;
  uint8_t* encode_result;
  string_compress(msg_struct_buf, msg_struct_buf_len, &encode_result, &encodeSize);

  uint8_t* ciphertext = malloc(encodeSize + 16);
  size_t ciphertext_len = encodeSize;
  uint8_t hmac[32] = {};
  uint8_t iv[16] = {};
  aes_256_cbc_encrypt(encode_result, encodeSize, TEST_PRIVATE_KEY, ciphertext, &ciphertext_len, hmac, iv);

  printf("ciphertext len: %zu\n", ciphertext_len);

  ProtobufCBinaryData bhmac;
  ProtobufCBinaryData biv;
  ProtobufCBinaryData bdata;

  bdata.data = ciphertext;
  bdata.len = ciphertext_len;

  biv.data = iv;
  biv.len = 32;

  bhmac.data = hmac;
  bhmac.len = 16;

  msg.data = bdata;
  msg.hmac = bhmac;
  msg.iv = biv;

  msg_buf_len = obd2__msg_struct__get_packed_size(&msg);
  printf("msg_buf_len len: %zu\n", msg_buf_len);
  msg_buf = malloc(msg_buf_len);
  obd2__msg_struct__pack(&msg, msg_buf);

  char* enc = b64_encode(msg_buf, msg_buf_len);

  printf("len: %d str:%s\n", strlen(enc), enc);

  free(encode_result);
  free(ciphertext);

  free(enc);
  free(obd2_buf);
  free(msg_struct_buf);
  free(msg_buf);

  return 0;
}
