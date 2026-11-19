# dp Project, Part 1: Interpretation Questions

**Name:** Beckham Truong
**Drexel ID:** bt592
**Parameters** (paste the output of `./params.sh <your-id>`):

DREXEL_ID=bt592
DATA_DROP=31
ACK_DROP=25
SIZE_KB=72
DELAY=20


## Ground rules

- Answer every question in this file, below the question.
- Base every answer on **your** `evidence/` files. When a question asks for evidence, quote the exact log lines (copy them, keep the timestamps) and name the file they came from.
- An answer that does not match your evidence, or that could have been written without running your code, will not receive credit, however well written.
- Short and precise beats long and general. Most answers need a few sentences plus the quoted lines.

Useful commands:

```bash
grep 'pkt=17 ' evidence/merged-p-data.txt          # one packet's story (note the space)
grep -E 'TIMEOUT|DROP|DUP' evidence/merged-p-ack.txt
grep -v NET evidence/client-p-delay.txt            # status lines only
```

---

## P. Prediction (5 points)

**Complete and commit this section BEFORE you run `make evidence`.** Use only your parameters and your understanding of the protocol. The TA checks that this commit comes before the commit that adds `evidence/`. You are graded on your reasoning, not on whether you turn out to be right.

1. In your `p-data` run the client loses DATA packet `DATA_DROP` once. Describe, step by step, what the client and server will do from that moment until packet `DATA_DROP + 1` is sent. Will the server log a `DUP`? Why or why not?
2. In your `p-ack` run the server loses the ACK for packet `ACK_DROP` once. Same question: step by step, and will the server log a `DUP`?
3. From the client's point of view (only what the client can observe), will these two runs look different? Why?
4. Your `p-delay` run sends `SIZE_KB` KB with a random delay of 0 to `DELAY` ms on every packet in each direction. Estimate how long the transfer will take. Show your arithmetic.

*Your prediction:*

1. Packet 31 would have been dropped before reaching the server. The client will now wait for about 250 milliseconds and then resend packet 31 with the same number and offset. The server will acknowledge it and then send an ACK 31, and the client may now send packet 32. In my opinion there would be no DUP in this case

2. Now the server receives packet 25 and replicates the bytes in it, while ACK 25 is lost. This results in the client timing out and retransmitting packet 25 and here the server needs to register a DUPLICATE and replicate the same message without any change while sending the ACK 25 again. The client now sends packet 26

3. The transport will observe the following in both iterations: there was successful delivery, no ACK from that side prior to the deadline, retransmission, and finally ACK comes. The transport cannot say in which direction the packet loss occurred. We can see the difference in the logs of the simulator, but not in the transport

4. The message size is 72*1024 = 73728 bytes with a maximum of 480 bytes per packet it requires ceil(73,728/480) = 154 DATA packets. There is an average delay of 20/2 = 10 ms for each direction so I will calculate 154 * (10 + 10) = 3080 ms or 3.08 seconds to transfer the data. Real timing may be even longer since there is processing overhead for the programs to sleep

---

## 1. A timeout is not proof of loss (8 points)

Use `evidence/merged-p-data.txt` and `evidence/merged-p-ack.txt`.

a. Quote the lines around the TIMEOUT in each run (from the drop through the next ACK the client receives).

b. In one run the DATA was lost; in the other the DATA arrived and the ACK was lost. List exactly what the client's **transport code** (`send_data_packet()`) observed in each case. Are they different?

   The client's `network:` statistics line *does* differ between `client-p-data.txt` and `client-p-ack.txt`. Quote both lines, explain why they differ, and explain why `send_data_packet()` cannot use that information.

c. Give one more cause of a timeout that involves no loss at all, and point to a TIMEOUT in `evidence/merged-slowack.txt` that shows it. Explain why the client's response is the same in all three cases.

*Answer:*

a. The DATA-drop run never delivered the first copy of packet 31:

From `evidence/merged-p-data.txt`:

3.140 ms  dp-client: NET: OUT DATA      pkt=31 bytes=508 DROP (forced)
253.415 ms  dp-client: NET: TIMEOUT DATA     pkt=31 retransmitting
253.530 ms  dp-client: NET: OUT DATA    pkt=31 bytes=508
253.745 ms  dp-server: NET: IN  DATA    pkt=31 bytes=508
253.802 ms  dp-server: NET: OUT ACK     pkt=31 bytes=28
253.908 ms  dp-client: NET: IN  ACK     pkt=31 bytes=28

The ACK-drop run delivered packet 25, but its first ACK did not reach the client:

From `evidence/merged-p-ack.txt`:

2.581 ms  dp-client: NET: OUT DATA        pkt=25 bytes=508
2.610 ms  dp-server: NET: IN  DATA        pkt=25 bytes=508
2.624 ms  dp-server: NET: OUT ACK         pkt=25 bytes=28 DROP (forced)
253.609 ms  dp-client: NET: TIMEOUT DATA        pkt=25 retransmitting
253.651 ms  dp-client: NET: OUT DATA        pkt=25 bytes=508
253.793 ms  dp-server: NET: IN  DATA        pkt=25 bytes=508
253.819 ms  dp-server: NET: DUP DATA        pkt=25 already delivered, re-ACK
253.834 ms  dp-server: NET: OUT ACK         pkt=25 bytes=28
253.903 ms  dp-client: NET: IN  ACK         pkt=25 bytes=28

b. In both the cases, `send_data_packet()` receives success from `net_simulate()`, times out without receiving the matching ACK, resends the same packet, and then receives the matching ACK. Packet numbers vary as these are separate experiments, but the transport layer does not have any idea whether DATA or ACK is lost. Client statistics in both the cases were

From `evidence/client-p-data.txt`:

dp-client: network: attempted=157 dropped=1 delivered=156 delayed=0 avg-delay=0.00ms

From `evidence/client-p-ack.txt`:

dp-client: network: attempted=157 dropped=0 delivered=157 delayed=0 avg-delay=0.00ms

The drop occurs in the first client in an outgoing DATA packet. However, in the second trial, the drop takes place on the server end, therefore, resulting in a count of zero drops in the client end. Such statistics and DROP tags are provided by the simulator; the transport does not check for such things, neither will a sender know about them in reality

c. The response is delayed but not missing. In `slowack` test, the acknowledgment for packet number 8 was delayed by 307 milliseconds whereas the timeout was only 250 milliseconds:

From `evidence/merged-slowack.txt`:

1466.041 ms  dp-client: NET: OUT DATA        pkt=8 bytes=508
1466.142 ms  dp-server: NET: IN  DATA        pkt=8 bytes=508
1466.191 ms  dp-server: NET: OUT ACK         pkt=8 bytes=28 delay=307ms
1717.119 ms  dp-client: NET: TIMEOUT DATA        pkt=8 retransmitting
1717.241 ms  dp-client: NET: OUT DATA        pkt=8 bytes=508
1778.212 ms  dp-server: NET: IN  DATA        pkt=8 bytes=508
1778.257 ms  dp-client: NET: IN  ACK         pkt=8 bytes=28

There is a delay but no loss of data in the test. The client has timed out for the resend of packet 8 since it doesn’t have an ACK to match it. It doesn’t matter if DATA has been lost, the ACK is lost, or it is delayed
---

## 2. Duplicate detection (8 points)

Use `evidence/merged-p-ack.txt`.

a. Quote the `DUP` line and the lines just before and after it. Which packet number is it?

b. Explain, step by step, how the server knew this packet was a duplicate. Name the variable in your `dp-proto.c` that it compared against.

c. The server sent an ACK for the duplicate. What would happen to the client if it did not? What would happen to the received message if the server copied the duplicate's payload instead of discarding it? Use your actual `SIZE_KB` and `ACK_DROP` to say exactly which bytes would end up wrong.

*Answer:*

a. The duplicate was packet 25. These lines show it arriving again and being ACKed again:

From `evidence/merged-p-ack.txt`:

253.651 ms  dp-client: NET: OUT DATA        pkt=25 bytes=508
253.793 ms  dp-server: NET: IN  DATA        pkt=25 bytes=508
253.819 ms  dp-server: NET: DUP DATA        pkt=25 already delivered, re-ACK
253.834 ms  dp-server: NET: OUT ACK         pkt=25 bytes=28
253.903 ms  dp-client: NET: IN  ACK         pkt=25 bytes=28


b. In the case of packet 25 when the first copy was being received, the server examined its values, duplicated it, and incremented the value of `conn->recv_next_packet` to 26. However the second copy also had the value of 25. Since 25 was smaller than 26, `dp_recv()` noted a DUP and acknowledged it without duplicating bytes

c. If the server fails to re-ACK it, then the client will continue to wait for ACK 25 and continue to transmit until all retries are exhausted. It is impossible for the client to realize that the first packet was already acknowledged

Message size is 72*1024 = 73728 bytes. Packet number 25 should be at an offset of (25-1) *480 = 11520 and occupies bytes 11520 to 11999 starting from zero, so if I append this duplicate at this position, it will move the old bytes from 11520 to 11999 to positions 12000 to 12479 because these positions are for packet number 26. This means my offset check will fail when packet 26 arrives. Without offset checking, appending everything left to the end will cause a shift of 480 bytes, resulting in 74208 bytes instead of 73728 bytes. Moving this duplicate to its correct position will just overwrite the same bytes, which will not add up as new data

## 3. Packet number vs message offset (7 points)

Use `evidence/merged-p-data.txt`.

a. Your lost packet `DATA_DROP` was retransmitted. What were its packet number and message offset on the original transmission and on the retransmission? (The offset is not printed in the log; compute it from the packet number and explain how.)

b. Packet number and message offset both increase by a fixed step here. Explain why they are still two different ideas: what does each one identify, and who uses it? TCP uses a single byte sequence number for both jobs. Why can TCP get away with that?

c. Suppose the protocol later allowed a retransmission to carry a new packet number (QUIC does this). Which field would the receiver then have to use to detect duplicates, and why?

*Answer:*

a. Both transmissions used packet number 31:

From `evidence/merged-p-data.txt`:

3.140 ms  dp-client: NET: OUT DATA        pkt=31 bytes=508 DROP (forced)
253.415 ms  dp-client: NET: TIMEOUT DATA        pkt=31 retransmitting
253.530 ms  dp-client: NET: OUT DATA        pkt=31 bytes=508

No offsets are printed on the log file. First 30 packets contain 480 bytes so packet 31 begins at 30*480 = 14400. Both files share the same offset number since `send_data_packet()` resends the exact same packet

b. Packet number identifies the protocol packet. Sender uses this number to find out corresponding ACK while receiver compares this value with `conn->recv_next_packet`. Offset identifies the location in the application-level message, which allows the receiver to determine the correct place for the payload. These numbers happen to increase at the same time because in Part 1, there are fixed size fragments sent over one stream. However they measure different values - packets versus bytes. TCP sequence numbers identify both bytes acknowledged and the location in the byte stream regardless of the segment size

c. In case the retransmitted packet received a new packet number then comparing the packet numbers wouldn't help the receiver realize that the data had already been accepted. This is because the receiver would have to compare the offset of the message within the payload or the stream offset and length to the byte ranges already received

---

## 4. FIN vs message length (5 points)

Use `evidence/merged-p-data.txt`.

a. Quote the last DATA packet of your transfer. How many payload bytes did it carry, and how do you know from the log line? Check your answer against `SIZE_KB`.

b. Every DATA packet already carries `message_length`. What does FIN tell the receiver that `message_length` does not? Think about Part 2, where one connection will carry several streams that end at different times.

*Answer:*

a. The last DATA packet was number 154:

From `evidence/merged-p-data.txt`:

265.890 ms  dp-client: NET: OUT DATA        pkt=154 bytes=316
265.916 ms  dp-server: NET: IN  DATA        pkt=154 bytes=316

The log shows 316 bytes on the wire with the 28 byte header included. The data size was 316-28 = 288 bytes. This equates to 73728-(153*480) = 288 which means that 154 packets can send the entire message

b. message_length specifies the number of bytes the message should have. FIN indicates that this is the end of the sender’s data transmission. The receiver needs both the number of received bytes should be equal to the declared length and the FIN should be set for the last packet. As the log program does not output flags the quote above does not confirm that the FIN flag was used. dp_send() sets it, dp_recv() checks it prior to considering the transfer completed. With multiple streams, one stream may complete its transfer but other streams on this connection will continue their work

---

## 5. Stop-and-wait performance (10 points)

Use `evidence/client-p-delay.txt` and `evidence/merged-p-delay.txt`.

a. Report the measured transfer time and the number of DATA packets from the client's `sent` line. Compare with your Prediction.

b. Build a simple model: each DATA packet waits for one round trip, and each direction adds on average half of `DELAY` ms. What does the model predict? By how much (percent) does the measurement differ?

c. Explain the difference. Look at a few consecutive packets in the merged log: how long does one DATA/ACK exchange actually take, compared with the two `delay=` values printed for it?

d. Your `clean` test sent 1 MiB in a few seconds on localhost, where the round trip is a fraction of a millisecond. If the round-trip time were 100 ms, how long would stop-and-wait take for 1 MiB? What change to the protocol would fix this? (Part 2 does it.)

*Answer:*

a. The client measured:

From `evidence/client-p-delay.txt`:

dp-client: sent 73728 bytes in 154 DATA packets, 3487.3 ms

The transfer took 3487ms for 154 DATA packets. My prediction was 3080 ms so the run took 406 ms longer

b. Every direction contributes a time of 20/2 = 10 ms. Each one way transfer DATA/ACK will therefore require 20 ms and the total time in the simple model is 154*20 = 3080 ms. The percentage increase in measurement compared to the model is (3487-3080)/3080*100 = approx 13 percent. This is data transmission time only

c. The first two exchanges show why adding only the printed delays is not enough:

From `evidence/merged-p-delay.txt`:


32.030 ms  dp-client: NET: OUT DATA        pkt=1 bytes=508 delay=3ms
35.909 ms  dp-server: NET: IN  DATA        pkt=1 bytes=508
35.971 ms  dp-server: NET: OUT ACK         pkt=1 bytes=28 delay=5ms
42.388 ms  dp-client: NET: IN  ACK         pkt=1 bytes=28
42.448 ms  dp-client: NET: OUT DATA        pkt=2 bytes=508 delay=3ms
46.333 ms  dp-server: NET: IN  DATA        pkt=2 bytes=508
46.397 ms  dp-server: NET: OUT ACK         pkt=2 bytes=28 delay=8ms
56.245 ms  dp-client: NET: IN  ACK         pkt=2 bytes=28

Packet 1 had an actual time of 42.388-32.030 = 10.358ms although its two configured delays sum up to only 3+5 = 8 ms. Packet 2 took 56.245-42.448 = 13.797 ms while the sum of its configured delays was 3+8 = 11 ms. Delay recording takes place before the sleep operation in the simulator. The sleep, scheduling also the socket work and delay recording take time. Random draw is also not always equal to 10 ms on a limited amount of steps. For the DATA and ACK lines the total configured delays amount to 2864 ms, which leaves 623.3 ms for other delays.

d. One MiB requires ceil(1,048,576/480) = 2185 DATA segments. With 100ms round trip time the stop and wait protocol will require 2185*0.1 = 218.5 seconds (or 3 minutes 38.5 seconds) without the additional. The sliding window allows more than one packet to be in transit rather than waiting for each packet for the whole round trip

## 6. Socket timeouts and deadlines (6 points)

a. In `merged-p-data.txt`, how long after the drop did the TIMEOUT fire? Where does that number come from in the code?

b. `SO_RCVTIMEO` makes `recvfrom()` give up after a set time. Why would a network program that waits forever for a reply be a poor design? Give one example outside this assignment and what the program should do when the timeout fires.

c. The provided `recv_until()` waits against a fixed deadline rather than calling `recvfrom()` with a fresh `SO_RCVTIMEO` each time. Describe a situation in this protocol where the fresh-timeout approach would wait much longer than 250 ms.

*Answer:*

a. These events were 253.415-3.140 = 250.275 ms apart:

From `evidence/merged-p-data.txt`:

3.140 ms  dp-client: NET: OUT DATA        pkt=31 bytes=508 DROP (forced)
253.415 ms  dp-client: NET: TIMEOUT DATA        pkt=31 retransmitting


Setting is taken from `DP_DEFAULT_TIMEOUT_MS`, which is 250 based on `dp-proto.h`. In `init_connection()` value is put into `conn->timeout_ms`. Then, after the message is sent, `send_data_packet()` runs `deadline_after(conn->timeout_ms)` and waits using `recv_until()`. Small number above 250ms is reasonable considering scheduling and processing time

b. Infinite waiting would lock the application if its peer goes down or if the link is down. For ex like the weather application should not hang indefinitely when its server stops responding to it. The application can give up after some timeout period and allow the user to try again. If it came to it dp retries on timeout and returns -2 after reaching its retry limit

c. A late ACK for an older packet can arrive while the sender is waiting for the next packet's ACK. This happened in `slowack`:

From `evidence/merged-slowack.txt`:

1778.335 ms  dp-client: NET: OUT DATA        pkt=9 bytes=508
1862.591 ms  dp-client: NET: IN  ACK         pkt=8 bytes=28

The Packet 9 is transmitted at 1778.335 ms while an old ACK 8 is received at 1862.591 ms. Assuming the old ACK triggers a new 250 ms timeout then the timeout for ACK 9 might go to 2112.591 ms that is 334.256 ms after its transmission. The absolute deadline will be about 2028.335 ms. Other unrelated packets may extend the new timeout but not the absolute deadline
---

## 7. Closing reliably (6 points)

Use `evidence/merged-loss50.txt` (nearly the same for everyone, since the seeds are fixed).

a. Find a CONN_ACK that was dropped. What did the client do, and which of your functions answered the client's next CONNECT? Quote the lines.

b. Find a CLOSE_ACK that was dropped. Quote the lines showing the client's retransmitted CLOSE being answered.

c. The server keeps answering for about two seconds after the client goes quiet. What would go wrong if it exited immediately after sending its first CLOSE_ACK? Can the client ever be completely sure the server received its final message? (This is a version of the Two Generals problem.) TCP faces the same problem when closing; which TCP state plays the role of dp's linger?

*Answer:*

a. The first server CONN_ACK was dropped:

From `evidence/merged-loss50.txt`:

251.267 ms  dp-server: NET: IN  CONNECT     pkt=0 bytes=28
251.305 ms  dp-server: NET: OUT CONN_ACK    pkt=0 bytes=28 DROP
502.120 ms  dp-client: NET: TIMEOUT CONNECT     pkt=0 retransmitting
502.218 ms  dp-client: NET: OUT CONNECT     pkt=0 bytes=28
502.416 ms  dp-server: NET: IN  CONNECT     pkt=0 bytes=28
502.463 ms  dp-server: NET: OUT CONN_ACK    pkt=0 bytes=28
502.566 ms  dp-client: NET: IN  CONN_ACK    pkt=0 bytes=28

The client timed out and resent a CONNECT message using the same connection ID. The first CONN_ACK had been sent by `dp_accept()` and it exited. My `dp_recv()` processed the second connect message by calling `send_connect_ack()` again

b. This run's original `merged-loss50.txt` did not drop a CLOSE_ACK. It dropped the first CLOSE instead:

From `evidence/merged-loss50.txt`:

16823.682 ms  dp-client: NET: OUT CLOSE       pkt=19 bytes=28 DROP
17074.699 ms  dp-client: NET: TIMEOUT CLOSE       pkt=19 retransmitting
17074.753 ms  dp-client: NET: OUT CLOSE       pkt=19 bytes=28
17075.060 ms  dp-server: NET: IN  CLOSE       pkt=19 bytes=28
17075.101 ms  dp-server: NET: OUT CLOSE_ACK   pkt=19 bytes=28
17075.176 ms  dp-client: NET: IN  CLOSE_ACK   pkt=19 bytes=28

From `evidence/merged-close-ack-extra.txt`:

6783.884 ms  dp-client: NET: OUT CLOSE       pkt=19 bytes=28
6783.946 ms  dp-server: NET: IN  CLOSE       pkt=19 bytes=28
6783.978 ms  dp-server: NET: OUT CLOSE_ACK   pkt=19 bytes=28 DROP
7034.913 ms  dp-client: NET: TIMEOUT CLOSE       pkt=19 retransmitting
7034.935 ms  dp-client: NET: OUT CLOSE       pkt=19 bytes=28
7035.116 ms  dp-server: NET: IN  CLOSE       pkt=19 bytes=28
7035.209 ms  dp-server: NET: OUT CLOSE_ACK   pkt=19 bytes=28 DROP
7285.446 ms  dp-client: NET: TIMEOUT CLOSE       pkt=19 retransmitting
7285.471 ms  dp-client: NET: OUT CLOSE       pkt=19 bytes=28
7285.650 ms  dp-server: NET: IN  CLOSE       pkt=19 bytes=28
7285.724 ms  dp-server: NET: OUT CLOSE_ACK   pkt=19 bytes=28
7285.846 ms  dp-client: NET: IN  CLOSE_ACK   pkt=19 bytes=28

CLOSE 19 was continually resent by the client and the function dp_wait_for_close() remained alive answering all of them until CLOSE_ACK reached the client. This is an additional proof rather than the statement that the original trace had a dropped CLOSE_ACK

c. The server having left immediately after receiving its first CLOSE_ACK would make the loss of ACK cause the client to repeat transmissions to a server that had already left. The linger period is 8*250 = 2000 milliseconds of silence, enough time for it to respond to any repetition. Matching with the server's CLOSE_ACK means that the server has received the client's CLOSE message. It doesn't guarantee both parties with assurance that all last messages have been received and adding an additional acknowledgment will only add one more last message that could get lost
