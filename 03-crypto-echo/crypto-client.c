/**
 * =============================================================================
 * STUDENT ASSIGNMENT: CRYPTO-CLIENT.C
 * =============================================================================
 * 
 * ASSIGNMENT OBJECTIVE:
 * Implement a TCP client that communicates with a server using an encrypted
 * protocol. Your focus is on socket programming and network communication.
 * The cryptographic functions are provided for you in crypto-lib.
 * 
 * =============================================================================
 * WHAT YOU NEED TO IMPLEMENT:
 * =============================================================================
 * 
 * 1. SOCKET CONNECTION (start_client function):
 *    - Create a TCP socket using socket()
 *    - Configure the server address structure (struct sockaddr_in)
 *    - Connect to the server using connect()
 *    - Handle connection errors appropriately
 *    - Call your communication loop function
 *    - Close the socket when done
 * 
 * 2. CLIENT COMMUNICATION LOOP:
 *    - Create a function that handles the request/response cycle
 *    - Allocate buffers for sending and receiving data
 *    - Maintain a session key (crypto_key_t) for encryption
 *    - Loop to:
 *      a) Get user command using get_command() (provided below)
 *      b) Build a PDU (Protocol Data Unit) from the command
 *      c) Send the PDU to the server using send()
 *      d) Receive the server's response using recv()
 *      e) Process the response (extract key, decrypt data, etc.)
 *      f) Handle exit commands and connection closures
 *    - Free allocated buffers before returning
 * 
 * 3. PDU CONSTRUCTION:
 *    - Consider creating a helper function to build PDUs
 *    - Fill in the PDU header (msg_type, direction, payload_len)
 *    - For MSG_DATA: copy plaintext to payload
 *    - For MSG_ENCRYPTED_DATA: use encrypt_string() to encrypt before copying
 *    - For MSG_KEY_EXCHANGE: no payload needed
 *    - For command messages: no payload needed
 *    - Return the total PDU size (header + payload)
 * 
 * =============================================================================
 * ONE APPROACH TO SOLVE THIS PROBLEM:
 * =============================================================================
 * 
 * FUNCTION STRUCTURE:
 * 
 * void start_client(const char* addr, int port) {
 *     // 1. Create TCP socket
 *     // 2. Configure server address (sockaddr_in)
 *     // 3. Connect to server
 *     // 4. Print connection confirmation
 *     // 5. Call your communication loop function
 *     // 6. Close socket
 *     // 7. Print disconnection message
 * }
 * 
 * int client_loop(int socket_fd) {
 *     // 1. Allocate buffers (send, receive, input)
 *     // 2. Initialize session_key to NULL_CRYPTO_KEY
 *     // 3. Loop:
 *     //    a) Call get_command() to get user input
 *     //    b) Build PDU from command (use helper function)
 *     //    c) Send PDU using send()
 *     //    d) If exit command, break after sending
 *     //    e) Receive response using recv()
 *     //    f) Handle recv() return values (0 = closed, <0 = error)
 *     //    g) Process response:
 *     //       - If MSG_KEY_EXCHANGE: extract key from payload
 *     //       - If MSG_ENCRYPTED_DATA: decrypt using decrypt_string()
 *     //       - Print results
 *     //    h) Loop back
 *     // 4. Free buffers
 *     // 5. Return success/error code
 * }
 * 
 * int build_packet(const msg_cmd_t *cmd, crypto_msg_t *pdu, crypto_key_t key) {
 *     // 1. Set pdu->header.msg_type = cmd->cmd_id
 *     // 2. Set pdu->header.direction = DIR_REQUEST
 *     // 3. Based on cmd->cmd_id:
 *     //    - MSG_DATA: copy cmd->cmd_line to payload, set length
 *     //    - MSG_ENCRYPTED_DATA: encrypt cmd->cmd_line, set length
 *     //    - MSG_KEY_EXCHANGE: set length to 0
 *     //    - Command messages: set length to 0
 *     // 4. Return sizeof(crypto_pdu_t) + payload_len
 * }
 * 
 * =============================================================================
 * IMPORTANT PROTOCOL DETAILS:
 * =============================================================================
 * 
 * PDU STRUCTURE:
 *   typedef struct crypto_pdu {
 *       uint8_t  msg_type;      // MSG_DATA, MSG_ENCRYPTED_DATA, etc.
 *       uint8_t  direction;     // DIR_REQUEST or DIR_RESPONSE
 *       uint16_t payload_len;   // Length of payload in bytes
 *   } crypto_pdu_t;
 * 
 *   typedef struct crypto_msg {
 *       crypto_pdu_t header;
 *       uint8_t      payload[]; // Flexible array
 *   } crypto_msg_t;
 * 
 * MESSAGE TYPES (from protocol.h):
 *   MSG_KEY_EXCHANGE     - Request/send encryption key
 *   MSG_DATA             - Plain text message
 *   MSG_ENCRYPTED_DATA   - Encrypted message (requires session key)
 *   MSG_CMD_CLIENT_STOP  - Client exit command
 *   MSG_CMD_SERVER_STOP  - Server shutdown command
 * 
 * TYPICAL MESSAGE FLOW:
 *   1. Client sends MSG_KEY_EXCHANGE request
 *   2. Server responds with MSG_KEY_EXCHANGE + key in payload
 *   3. Client extracts key: memcpy(&session_key, response->payload, sizeof(crypto_key_t))
 *   4. Client can now send MSG_ENCRYPTED_DATA
 *   5. Server responds with MSG_ENCRYPTED_DATA
 *   6. Client decrypts using decrypt_string()
 * 
 * =============================================================================
 * CRYPTO LIBRARY FUNCTIONS YOU'LL USE:
 * =============================================================================
 * 
 * int encrypt_string(crypto_key_t key, uint8_t *out, uint8_t *in, size_t len)
 *   - Encrypts a string before sending
 *   - Returns number of encrypted bytes or negative on error
 * 
 * int decrypt_string(crypto_key_t key, uint8_t *out, uint8_t *in, size_t len)
 *   - Decrypts received data
 *   - Returns number of decrypted chars or negative on error
 *   - NOTE: Output is NOT null-terminated, you must add '\0'
 * 
 * void print_msg_info(crypto_msg_t *msg, crypto_key_t key, int mode)
 *   - Prints PDU details for debugging
 *   - Use CLIENT_MODE for the mode parameter
 *   - VERY helpful for debugging your protocol!
 * 
 * =============================================================================
 * DEBUGGING TIPS:
 * =============================================================================
 * 
 * 1. Use print_msg_info() before sending and after receiving
 * 2. Check return values from ALL socket operations
 * 3. Verify payload_len matches actual data length
 * 4. Remember: recv() may return less bytes than expected
 * 5. Encrypted data requires a valid session key (check for NULL_CRYPTO_KEY)
 * 6. Use printf() liberally to trace program flow
 * 
 * =============================================================================
 * TESTING RECOMMENDATIONS:
 * =============================================================================
 * 
 * 1. Start simple: Get plain MSG_DATA working first
 * 2. Test key exchange: Send '#' command
 * 3. Test encryption: Send '!message' after key exchange
 * 4. Test exit commands: '-' for client exit, '=' for server shutdown
 * 5. Test error cases: What if server closes unexpectedly?
 * 
 * Good luck! Remember: Focus on the socket operations. The crypto is done!
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdint.h>
#include "crypto-client.h"
#include "crypto-lib.h"
#include "protocol.h"
#include "crypto-io.h"


/* =============================================================================
 * STUDENT TODO: IMPLEMENT THIS FUNCTION
 * =============================================================================
 * This is the main client function. You need to:
 * 1. Create a TCP socket
 * 2. Connect to the server
 * 3. Call your communication loop
 * 4. Clean up and close the socket
 * 
 * Parameters:
 *   addr - Server IP address (e.g., "127.0.0.1")
 *   port - Server port number (e.g., 1234)
 */
extern int client_sockfd;

// leave room for the five characters the server adds to the reply.
static int build_request(const msg_cmd_t *command, crypto_msg_t *request,
    crypto_key_t session_key) {
  request->header.msg_type = command->cmd_id;
  request->header.direction = DIR_REQUEST;
  request->header.payload_len = 0;
  if (command->cmd_id != MSG_DATA && command->cmd_id != MSG_ENCRYPTED_DATA) {
    return 0;
  }
  size_t length = strlen(command->cmd_line);
  if (length > MAX_MSG_DATA_SIZE - 5) {
    printf("[ERROR] Message is too long. Maximum is %zu characters.\n",
           (size_t)MAX_MSG_DATA_SIZE - 5);
    return -1;
  }
  if (command->cmd_id == MSG_ENCRYPTED_DATA) {
    if (session_key == NULL_CRYPTO_KEY) {
      printf("[ERROR] Use # to exchange keys before sending encrypted data.\n");
      return -1;
    }
    // check the allowed characters before passing text to the library.
    if (strspn(command->cmd_line,
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 ,") != length) {
      printf("[ERROR] Encrypted text can only use letters, digits, spaces, and commas.\n");
      return -1;
    }
    if (length > 0 && encrypt_string(session_key, request->payload,
        (uint8_t *)command->cmd_line, length) < 0) {
      printf("[ERROR] Could not encrypt message.\n");
      return -1;
    }
  } else {
    memcpy(request->payload, command->cmd_line, length);
  }
  request->header.payload_len = (uint16_t)length;
  request->payload[length] = '\0';
  return 0;
}

static int client_loop(int fd) {
  crypto_msg_t *request = calloc(1, BUFFER_SIZE + 1);
  crypto_msg_t *response = calloc(1, BUFFER_SIZE + 1);
  if (!request || !response) {
    free(request);
    free(response);
    fprintf(stderr, "Could not allocate message buffers.\n");
    return -1;
  }
  crypto_key_t session_key = NULL_CRYPTO_KEY;
  char input[MAX_MSG_DATA_SIZE + 1];
  int result = 0;
  printf("Type ? for help, - to leave, or = to stop the server.\n");
  while (1) {
    msg_cmd_t command;
    int status = get_command(input, sizeof(input), &command);
    // stop on end of input instead of repeatedly asking for another line.
    if (status != CMD_EXECUTE) {
      if (feof(stdin) || ferror(stdin)) break;
      continue;
    }
    if (strlen(input) == sizeof(input) - 1) {
      int ch;
      while ((ch = getchar()) != '\n' && ch != EOF) {}
      printf("[ERROR] Input line is too long.\n");
      continue;
    }
    if (build_request(&command, request, session_key) < 0) continue;
    show_message(request, session_key, CLIENT_MODE);
    if (send_message(fd, request) < 0) {
      perror("send request");
      result = -1;
      break;
    }
    // stop commands have no reply, so do not wait for one.
    if (command.cmd_id == MSG_CMD_CLIENT_STOP ||
        command.cmd_id == MSG_CMD_SERVER_STOP) break;
    status = receive_message(fd, response);
    if (status != 1) {
      if (status == 0) fprintf(stderr, "Server disconnected.\n");
      else perror("receive response");
      result = -1;
      break;
    }
    if (response->header.direction != DIR_RESPONSE ||
        (response->header.msg_type != command.cmd_id &&
         response->header.msg_type != MSG_ERROR)) {
      fprintf(stderr, "Unexpected response header.\n");
      result = -1;
      break;
    }
    if (response->header.msg_type == MSG_KEY_EXCHANGE) {
      if (response->header.payload_len != sizeof(session_key)) {
        fprintf(stderr, "Invalid key exchange response.\n");
        result = -1;
        break;
      }
      crypto_key_t received_key;
      memcpy(&received_key, response->payload, sizeof(received_key));
      uint8_t encryption_key = GET_ENCRYPTION_KEY(received_key);
      uint8_t decryption_key = GET_DECRYPTION_KEY(received_key);
      if (encryption_key == 0 || encryption_key >= 64 || !(encryption_key & 1) ||
          decryption_key == 0 || decryption_key >= 64 || !(decryption_key & 1)) {
        fprintf(stderr, "Invalid session key.\n");
        result = -1;
        break;
      }
      session_key = received_key;
    }
    // the provided display function also decrypts encrypted replies.
    show_message(response, session_key, CLIENT_MODE);
  }
  free(request);
  free(response);
  return result;
}

void start_client(const char* addr, int port) {
  struct sockaddr_in server = {0};
  server.sin_family = AF_INET;
  server.sin_port = htons((uint16_t)port);
  if (inet_pton(AF_INET, addr, &server.sin_addr) != 1) {
    fprintf(stderr, "Invalid server address: %s\n", addr);
    exit(EXIT_FAILURE);
  }
  client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (client_sockfd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  if (prepare_socket(client_sockfd) < 0 ||
      connect(client_sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
    perror("connect");
    close(client_sockfd);
    client_sockfd = -1;
    exit(EXIT_FAILURE);
  }
  printf("Connected to server %s:%d\n", addr, port);
  int result = client_loop(client_sockfd);
  close(client_sockfd);
  client_sockfd = -1;
  printf("Client disconnected.\n");
  if (result < 0) exit(EXIT_FAILURE);
}


/* =============================================================================
 * PROVIDED HELPER FUNCTION: get_command()
 * =============================================================================
 * This function is FULLY IMPLEMENTED for you. It handles user input and
 * interprets special command characters.
 * 
 * HOW TO USE:
 *   char input_buffer[MAX_MSG_DATA_SIZE];
 *   msg_cmd_t command;
 *   int result = get_command(input_buffer, MAX_MSG_DATA_SIZE, &command);
 *   if (result == CMD_EXECUTE) {
 *       // command.cmd_id contains the message type
 *       // command.cmd_line contains the message text (or NULL)
 *   } else {
 *       // CMD_NO_EXEC means skip this command (like '?' for help)
 *   }
 * 
 * COMMAND FORMAT:
 *   Regular text      -> MSG_DATA (plain text message)
 *   !<message>        -> MSG_ENCRYPTED_DATA (encrypt the message)
 *   #                 -> MSG_KEY_EXCHANGE (request encryption key)
 *   -                 -> MSG_CMD_CLIENT_STOP (exit client)
 *   =                 -> MSG_CMD_SERVER_STOP (shutdown server)
 *   ?                 -> Show help (returns CMD_NO_EXEC)
 * 
 * RETURN VALUES:
 *   CMD_EXECUTE  - Command should be sent to server (use cmd_id and cmd_line)
 *   CMD_NO_EXEC  - Command was handled locally (like help), don't send
 * 
 * IMPORTANT NOTES:
 *   - The returned cmd_line is a pointer into cmd_buff (no need to free)
 *   - For commands without data (like '#'), cmd_line will be NULL
 *   - For '!' commands, cmd_line points to text AFTER the '!' character
 */
int get_command(char *cmd_buff, size_t cmd_buff_sz, msg_cmd_t *msg_cmd)
{
    if ((cmd_buff == NULL) || (cmd_buff_sz == 0)) return CMD_NO_EXEC;

    printf("> ");
    fflush(stdout);
    
    // Get input from user
    if (fgets(cmd_buff, cmd_buff_sz, stdin) == NULL) {
        printf("[WARNING] Error reading input command.\n\n");
        return CMD_NO_EXEC;
    }
    
    // Remove trailing newline
    cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

    // Interpret the command based on first character
    switch (cmd_buff[0]) {
        case '!':
            // Encrypted message - everything after '!' is the message
            msg_cmd->cmd_id = MSG_ENCRYPTED_DATA;
            msg_cmd->cmd_line = cmd_buff + 1; // Skip the '!' character
            return CMD_EXECUTE;
            
        case '#':
            // Key exchange request - no message data
            msg_cmd->cmd_id = MSG_KEY_EXCHANGE;
            msg_cmd->cmd_line = NULL;
            return CMD_EXECUTE;
            
        case '$':
            // Digital signature (not implemented in this assignment)
            msg_cmd->cmd_id = MSG_DIG_SIGNATURE;
            msg_cmd->cmd_line = NULL;
            printf("[INFO] Digital signature command not implemented yet.\n\n");
            return CMD_NO_EXEC;
            
        case '-':
            // Client exit command
            msg_cmd->cmd_id = MSG_CMD_CLIENT_STOP;
            msg_cmd->cmd_line = NULL;
            return CMD_EXECUTE;
            
        case '=':
            // Server shutdown command
            msg_cmd->cmd_id = MSG_CMD_SERVER_STOP;
            msg_cmd->cmd_line = NULL;
            return CMD_EXECUTE;
            
        case '?':
            // Help - display available commands
            msg_cmd->cmd_id = MSG_HELP_CMD;
            msg_cmd->cmd_line = NULL;
            printf("Available commands:\n");
            printf("  <message>  : Send plain text message\n");
            printf("  !<message> : Send encrypted message (requires key exchange first)\n");
            printf("  #          : Request key exchange from server\n");
            printf("  ?          : Show this help message\n");
            printf("  -          : Exit the client\n");
            printf("  =          : Exit the client and request server shutdown\n\n");
            return CMD_NO_EXEC;
            
        default:
            // Regular text message
            msg_cmd->cmd_id = MSG_DATA;
            msg_cmd->cmd_line = cmd_buff;
            return CMD_EXECUTE;
    }
    
    return CMD_NO_EXEC;
}
