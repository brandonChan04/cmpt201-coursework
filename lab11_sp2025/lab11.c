#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED "\e[9;31m"
#define GRN "\e[0;32m"
#define CRESET "\e[0m"

#define handle_error(msg)                                                      \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

size_t read_all_bytes(const char *filename, void *buffer, size_t buffer_size) {
  FILE *file = fopen(filename, "rb");
  if (!file) {
    handle_error("Error opening file");
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  if (file_size > buffer_size) {
    handle_error("File size is too large");
  }

  if (fread(buffer, 1, file_size, file) != file_size) {
    handle_error("Error reading file");
  }

  fclose(file);
  return file_size;
}

void print_file(const char *filename, const char *color) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    handle_error("Error opening file");
  }

  printf("%s", color);
  char line[256];
  while (fgets(line, sizeof(line), file)) {
    printf("%s", line);
  }
  fclose(file);
  printf(CRESET);
}

int verify(const char *message_path, const char *sign_path, EVP_PKEY *pubkey);

int main() {
  // File paths
  const char *message_files[] = {"message1.txt", "message2.txt",
                                 "message3.txt"};
  const char *signature_files[] = {"signature1.sig", "signature2.sig",
                                   "signature3.sig"};
  FILE *pubkey_file = fopen("public_key.pem", "r");
  if (!pubkey_file) {
    handle_error("Error opening public key file");
  }

  EVP_PKEY *pubkey = PEM_read_PUBKEY(pubkey_file, NULL, NULL, NULL);
  fclose(pubkey_file);

  if (!pubkey) {
    fprintf(stderr, "Error reading public key\n");
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
  }
  // Verify each message
  for (int i = 0; i < 3; i++) {
    printf("... Verifying message %d ...\n", i + 1);
    int result = verify(message_files[i], signature_files[i], pubkey);

    if (result < 0) {
      printf("Unknown authenticity of message %d\n", i + 1);
      print_file(message_files[i], CRESET);
    } else if (result == 0) {
      printf("Do not trust message %d!\n", i + 1);
      print_file(message_files[i], RED);
    } else {
      printf("Message %d is authentic!\n", i + 1);
      print_file(message_files[i], GRN);
    }
  }

  EVP_PKEY_free(pubkey);

  return 0;
}

/*
    Verify that the file `message_path` matches the signature `sign_path`
    using `pubkey`.
    Returns:
         1: Message matches signature
         0: Signature did not verify successfully
        -1: Message is does not match signature
*/
int verify(const char *message_path, const char *sign_path, EVP_PKEY *pubkey) {
#define MAX_FILE_SIZE 512
  unsigned char message[MAX_FILE_SIZE];
  unsigned char signature[MAX_FILE_SIZE];

  if (!pubkey) {
    fprintf(stderr, "public key null\n");
    return -1;
  }

  size_t message_len = read_all_bytes(message_path, message, MAX_FILE_SIZE);
  size_t signature_len = read_all_bytes(sign_path, signature, MAX_FILE_SIZE);

  EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
  if (!mdctx) {
    fprintf(stderr, "Error creating digest context\n");
    ERR_print_errors_fp(stderr);
    return -1;
  }

  if (EVP_DigestVerifyInit(mdctx, NULL, EVP_sha256(), NULL, pubkey) != 1) {
    fprintf(stderr, "Error initializing verification\n");
    ERR_print_errors_fp(stderr);
    EVP_MD_CTX_free(mdctx);
    return -1;
  }

  if (EVP_DigestVerifyUpdate(mdctx, message, message_len) != 1) {
    fprintf(stderr, "Error updating verification\n");
    ERR_print_errors_fp(stderr);
    EVP_MD_CTX_free(mdctx);
    return -1;
  }

  int verify_result = EVP_DigestVerifyFinal(mdctx, signature, signature_len);
  EVP_MD_CTX_free(mdctx);

  if (verify_result == 1) {
    return 1;
  } else if (verify_result == 0) {
    return 0;
  } else {
    fprintf(stderr, "Error during final verification step\n");
    ERR_print_errors_fp(stderr);
    return -1;
  }
}
