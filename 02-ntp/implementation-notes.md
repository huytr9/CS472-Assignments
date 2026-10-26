# Implementation Notes

The client send a request to a time server and reads the reply, it then converts the time value and byte order and use four timestamps to est the network delay and how far the local clock is either ahead or behind the actual server. 

Code I written, this is what the four timestamps to calculate the network delay and also estimate how far the clocks are apart "calculate_ntp_offset()":

"result->delay = (t4-t1)-(t3-t2);
result->offset = ((t2-t1)+(t3-t4))/2.0" 

Then it'll display the result without changing the computer clock, the code that we already had handled the network connection and sending and receiving the packets.

# Challenges I had/faced throughout

Challenges that I faced throughout were like time conversion, byte orders, and also double checking the calculations. While I was testing, the NIST had about 52 ms of delay while Cloudflare had about 10 ms which shows that different servers queries can have different delays.
Another thing I encountered was that the NIST result showed the local clock was about 31 ms behind the server.