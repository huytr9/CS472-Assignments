Beckham Truong (bt592)

## Question 1: TCP vs UDP

TCP is appropriate for this assignment because like as the client and server are connected while they exchange a key and then send messages. TCP gives a reliable ordered byte delivery so the program is able to complete receiving the key before actually processing the encrypted messages that follow. This connection also allows the program to know which key belongs to the current client but TCP doesmn't handle the key itself. It still has to save it nad check that the key exchange worked . Sending encrypted message before key exchange will result in error in this assignment

UDP needs more work because it can lose messages or send them in a different order so if the key response never arrives then the client would not have the key they needed to continue whcih need rules in order to retry the messages and keeping track of each session. Encryption can work with UDP but TCP just makes it easier to build the reliable conversation this assignment needs.

## Question 2: PDU structure

The fields within the PDU header determine how the message should be interpreted. The message type field determines if the message contains plain text, encrypted text a key or an instruction. The direction field determines if the message is a request or a reply. The payload length field determines how many bytes the receiver needs to read from the header onwards. For instance, the key reply has a two byte key, whereas the normal message can have a considerably longer string. These fields ensure that there is no need for the receiver to make any guesses about the data received

Sending messages such as "Hello World" in encrpted is also an alternative approach that we might consider, however in this case we should come up with even more rules for parsing. we would have to clarify the rules on how the message terminates and how to handle separators if the text includes them such like problems arise while working with binary data since it is possible to include characters that are not valid text characters into the message, such as zero byte characters
## Question 3: Payload length and TCP

TCP sends a series of bytes and  a single call to the send function will not necessarily correspond to a call to the recv function. The message may arrive in multiple fragments, or fragments of other messages may arrive at the same time. While it is true that TCP ensures reliable and sequential transmission of bytes, it does not inform the application program about the end of each application level message. The payload length informs the receiver about the number of bytes forming the current message

In the program the receive_message function first reads the four byte header. The length is then converted to network byte order and checked whether it does not exceed the buffer capacity. Then, we continue reading until the full message is received. Similarly, the sending function is also done in a loop since the send function may write less than requested. In case there was no length field, we would have needed another condition for example, a terminating character or a fixed length of the message. Waiting for the connection to close would also not have been enough since the client may send multiple messages before closing the connection

## Question 4: Key exchange and session state

Key exchange occurs at the beginning to ensure that each party has the required information regarding the key before any messages are encrypted and sent. Key exchange involves the creation of key pairs by the server. The server then transfers its key to the client and maintains its own key for the session. The client stores the transferred key for use in future messages. In case a new client connects to the server the key exchange process occurs once more. This ensures that the next client does not use the previous client's key pair

Haedcoded keys would be more efficient but everyone using the same program would use the same fixed keys. Updating the keys would entail updating the programs rather than generating new ones. The key exchange process is unauthenticated as well so wjhenever a connection is lost, the program loses its session state and has to generate new keys again. TCP provides an easily identifiable connection that can be used to relate to the keys. UDP can also be used in this case, but we need to add session management

## Question 5: Direction field

The direction field allows the receiver to verify the sense of the received message. The client makes requests via DIR_REQUEST, while the server provides responses via DIR_RESPONSE. In spite of the fact that both parties know about the roles they play there is always a possibility that something will be wrong with the implementation of the code, and the wrong direction may get inside the message. Verification of the direction field reveals this error rather than let the program go on without any understanding of the message structure

This field will also be heplful when analyzing network traffic. One can look at the header and determine whether it is a request or a response regardless of port number and even if in the future the protocol allows both ends to initiate a request, the direction will indicate the nature of the message. Many changes will be required to correlate the responses and requests if many transactions are going on at the same time. The direction field does not confirm the authorship of the message or guarantee its security