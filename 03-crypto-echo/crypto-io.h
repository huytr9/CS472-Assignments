#ifndef CRYPTO_IO_H
#define CRYPTO_IO_H

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "crypto-lib.h"
#include "protocol.h"

// a broken connection should return an error instead of ending the program.
static int prepare_socket(int fd) {
#ifdef SO_NOSIGPIPE
  int enabled = 1;
  return setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &enabled, sizeof(enabled));
#else
  (void)fd;
  return signal(SIGPIPE, SIG_IGN) == SIG_ERR ? -1 : 0;
#endif
}

// send may write only part of the buffer, so keep going until it is done.
static int write_bytes(int fd, const void *buffer, size_t length) {
  const uint8_t *bytes = buffer;
  size_t sent = 0;
  while (sent < length) {
    ssize_t count = send(fd, bytes + sent, length - sent, 0);
    if (count < 0 && errno == EINTR) continue;
    if (count <= 0) {
      if (count == 0) errno = EPIPE;
      return -1;
    }
    sent += (size_t)count;
  }
  return 0;
}

// return 1 for a full read, 0 for a clean close, and -1 for an error.
static int read_bytes(int fd, void *buffer, size_t length) {
  uint8_t *bytes = buffer;
  size_t received = 0;
  while (received < length) {
    ssize_t count = recv(fd, bytes + received, length - received, 0);
    if (count < 0 && errno == EINTR) continue;
    if (count < 0) return -1;
    if (count == 0) {
      if (received == 0) return 0;
      errno = EPROTO;
      return -1;
    }
    received += (size_t)count;
  }
  return 1;
}

static int send_message(int fd, const crypto_msg_t *message) {
  if (message->header.payload_len > MAX_MSG_DATA_SIZE) {
    errno = EMSGSIZE;
    return -1;
  }
  // keep local values in host order and convert only the bytes being sent.
  crypto_pdu_t header = message->header;
  header.payload_len = htons(message->header.payload_len);
  if (write_bytes(fd, &header, sizeof(header)) < 0) return -1;

  if (message->header.msg_type == MSG_KEY_EXCHANGE &&
      message->header.payload_len == sizeof(crypto_key_t)) {
    crypto_key_t key;
    memcpy(&key, message->payload, sizeof(key));
    key = htons(key);
    return write_bytes(fd, &key, sizeof(key));
  }
  return write_bytes(fd, message->payload, message->header.payload_len);
}

// callers allocate buffer_size + 1 bytes so displayed text can end with a null.
static int receive_message(int fd, crypto_msg_t *message) {
  int result = read_bytes(fd, &message->header, sizeof(crypto_pdu_t));
  if (result != 1) return result;
  message->header.payload_len = ntohs(message->header.payload_len);
  if (message->header.payload_len > MAX_MSG_DATA_SIZE) {
    errno = EMSGSIZE;
    return -1;
  }
  if (message->header.payload_len > 0) {
    result = read_bytes(fd, message->payload, message->header.payload_len);
    if (result != 1) {
      if (result == 0) errno = EPROTO;
      return -1;
    }
  }
  message->payload[message->header.payload_len] = '\0';
  if (message->header.msg_type == MSG_KEY_EXCHANGE &&
      message->header.payload_len == sizeof(crypto_key_t)) {
    crypto_key_t key;
    memcpy(&key, message->payload, sizeof(key));
    key = ntohs(key);
    memcpy(message->payload, &key, sizeof(key));
  }
  return 1;
}

static void show_message(crypto_msg_t *message, crypto_key_t key, int mode) {
  print_msg_info(message, key, mode);
  if (message->header.msg_type == MSG_ERROR) {
    printf("[ERROR] Server: %.*s\n", (int)message->header.payload_len,
           (char *)message->payload);
  }
}

#endif
