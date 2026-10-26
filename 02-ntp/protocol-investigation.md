# NTP Protocol Design Investigation

## Investigation 1: Why does NTP need four timestamps?

### Implementation Context

It uses four timestamps to try to figure out the network delay and how far the local clock is ahead or behind the server "calculate_ntp_offset()" It would convert the timestamp inton a number of seconds so the formula can use it "ntp_time_to_double()".
Also four times are when the cilent send the request "request->xmit_time" when the server receives it "response->recv_time" when the server the send the reply "response->xmit_time" and also when the client recieves it "recv_time" the starter code would record the last time right after receiving it.

"" - means the code i wrote**

The question I want to explore is why all the four times are needed instead of just comparing the server time with the local.

One thing I found confusing was that at first i wasn't sure why we needed four timesstamps. I thought like comparing the server times with the client would be sufficient enough, I wanted to udnerstand how the time spent traveling across the network affects that comparison. 

### Investigation Journey

#### 1. Why can't I just compare the server time with my computer time?

The server reply takes time inorder to actually reach the client, comparing only those two times would mix the network travel time with the difference between the clocks. Using all four timestamps helps account for that travel time.

#### 2. Why do I need the time the server received the request and the time it sent the reply?

The two times show how long the server is spent handling the request, it lets the client separate the server processing time different from the time spent traveling across the network.

Test ex: The request and reply took about 100ms, and the server spent 20 ms processing it, the network delay would be 80ms. This shows why the calculation subtracts the server processing time instead of actually counting the whole round trip as network delay "calculate_ntp_offset()"

#### 3. How does the code figure out how far my clock is off?

The calculation puts together the time differneces between both directions to estimate how far the clocks are apart, it assume the request and reply would take about the same amount of time to travel. As a result it'll tell if the clients clock is ahead or behind the server.

#### 4. What if the request takes longer to travel than the reply?

If the request takes longer to travel than the reply then the estimate can be off because the calculation again assumes that both of them have a alike travel time in both directions. If on of the direction takes longer then part of that difference can look like a clock error.

#### 5. Would averaging the results from different servers fix that?

Averaging the results from different server can reduce some of the random variation but it also doesn't gurantee the exact time. A lot of measurements can still share the same kind of error such as one of the network direction consistenly taking longer.


### Design Rationale

Technical synthesis -

Four tiomestamps lets the client remove the server interval and estimate the clock difference by using both of the directions. While a single server timestamp can't separate the clocks difference from reply delivery time. The unequal path example also shows a remanining limitation. This means that the result is still an estimate especially if the request takes longer to travel than the reply. The four timestamps give the client more info than just one, but they cant gurantee that the clocks are perfectly sync.


### Implementation Insight

At first I thought that the whole round trip time was just the network travel time. But now that I understand that the server also takes time to handle that request, the calculation remove the processing time to estiamte the network delay. Another thing that I also understand now is that the clock difference is still an estimate because the request and reply may take different amount of time to travel.

## Investigation 2: Why does network byte order matter?

### Implementation Context

The client changes the byte order of the timestamps before sending them and changes them back before using them in calculations "ntp_ts_to_net()" and "ntp_ts_to_host()". It also does this for the other fields that use more than one byte "ntp_to_net()" and  "ntp_to_host()". The starter code calls these functions before sending the packet and before processing the reply

The question I want to explore is why the numbers need to be converted if they already look correct when printed. Another part of that question is why some fields need conversion while others stay the same.

### Investigation Journey

#### 1. Why do I have to change the byte order before sending the packet?

Compauters store the bytes of number in different orders so the server might not be able to read them the same way the client does. Network byte order gives both sides the same exact format to use, this helps them read the numbers correctly even if the compiuters store them differently.

#### 2. What would happen if I skipped those conversions?

If the order doesn't match what the server expects then it could read a different number than anticapated, the packet could still have the right siz but the times inside it could be wrong/incorrect. This means that even if you send the packet successfully, it doesn't always mean the values were sent correctly.

Test ex: When I was testing, the reply had a reference ID of 0x4E495354, which the program had displayed as NIST. Decodoing the function put the value into network byte order beofre actually reading the four characters "decode_reference_id()", this gives an example of why the byte order matters
#### 3. Why do I have to convert both seconds and the fraction?

The seconds and fraction are two separeate numbers that make up the timestamp. Both needs to be in the correct order so the server can read the whole time correctly. If only the seconds are converted then the fraction could even still be wrong, and this would affect the calculation.
#### 4. Why don't I convert fields like stratum and precision too?

The fields only use one byte, so there are no other bytes within the field to put into a different order, the conversion is neded for values that use more than one byte, that is why the code changes some fields and leave the smaller ones alone.

#### 5. How can I tell if the conversions actually worked?

One way is to check that the bytes match the format the network expects. Another way is to convert a value to network order and back and check that it returns to the original value. Using both checks gives more confidence because two incorrect conversions could undo each other. Testing with real servers also helps check that the client can send and read the packets.

### Design Rationale

Technical synthesis -

Network byte order lets the client and server use the same format for numbers even if their computers store them differently. Without the conversion, the server could read a different value from what the client meant to send. The values are changed back before the client uses them in calculations. This keeps the format used for sending separate from the format used by the computer, and the single-byte fields can stay the same because they do not need reordering.

### Implementation Insight

The main point is that a number looking correct when printed does not always mean its bytes are ready to send over the network. The order of those bytes also matters. Changing the order before sending and changing it back before calculating lets each side use the value correctly. Checking the actual bytes is useful because getting the original number back after two conversions does not prove the network format was correct
