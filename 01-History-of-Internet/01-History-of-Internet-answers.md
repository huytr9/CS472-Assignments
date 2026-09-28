# History of the Internet Assignment

**Name:** Beckham Truong
**Student ID:** bt592


## Part 1: Problem–Solution Mapping Table

| Problem (from the 1974 paper or a modern problem) | Solution proposed by the paper or why it was not addressed | How we see this today |
|---|---|---|
| When different networks uses different addresses. | The solution proposed is a common address format that helps identify the destination network and host | If I switch from home WiFi to a phone hotspot then I can still open the same class website |
| Networks had different packet size limits. | They have proposed/addressed it by having gateaways that would split packets into smaller pieces that could fit to the next network. The pieces would be back together at the new destination. | If I send someone a big photo I don't have to split it up myself because the software sends the data in smaller pieces, and modern systems try to keep packets small enough to fit along the way |
| Packets could get lost | The receiver would let the sender know when the data arrives, if the sender waits too long without getting confirmation then it would send the data again. | If a few packets go missing during a download using TCP then I don't have to click download all over again because the TCP can get those missing pieces sent again. |
| Data could come out of order or more than once | Each piece of data is numbered so when the reciever can put everything in the correct with that would ignore any duplicate pieces. | If I download a PDF for class using TCP then I need the file to come back together correctly. It would put the data in order and removes duplicates, so a piece arriving late doesn't mean the PDF ends up everywhere. |
| A fast sender could overwhelm a receiver | The receiver advertises a window that limits how much data the sender can have outstanding. | If an app on my laptop can't keep up with incoming data the sender needs to slow down. |
| Data have to cross seperate netwroks | Gateways move daya from one network to another which allows different types of networks to communicate with each other | If I join an online game with a friend in another state, I just join the server without picking every network along the way

## Part 2: AI-Assisted Protocol Investigation

### A. Investigation Overview

**Problem chosen from the table:** I chose the third row, which is about packets getting lost while data is being sent across networks.

**Specific scenario:** I sometimes notice that a Discord call starts was sounding like robotic when my connection gets bad, while a file downloading at the same time usually finishes without being damaged. I wanted to understand what happens to the lost packets in each case and why the call keeps moving instead of waiting for every missing piece to be sent again

### B. Key Questions I Asked


1. When my Discord call sounds robotic but my file download is still working are both of them actually losing packets?
2. If they are both losing packets, why can the downloaded file be fixed while the audio I missed during the call cannot?
3. How does a file download figure out exactly which pieces are missing or out of order?
4. Would resending lost voice data make the Discord call sound better or would it create too much delay to be useful?
5. What does Discord do when too many packets are lost at once? Does it lower the audio quality, guess at the missing sound, or do something else?
6. Did Cerf and Kahn's 1974 design already include the main idea used to recover lost file data today, or was that added later?

### C. Connection to the 1974 Paper

With the connection of the paper, they understoof that packets could disappear or become damaged while going through several networks. Their design was responsible to check the complete data at the two ends of the connection. The receiver would see the data it receives then the sender could resend something if not knowing it came. Sequence numbers also helps the receiver see the missing data and puts the packets back in the correct order. That same approach can be seen in a file download using TCP today.

What have changed is that modern applications don't all treat packet loss the same. A file download is more focused on how accurate it is, so waiting for missing data is acceptable. A live voice call is focused on the timing because audio that arrives several seconds late is no longer helpful. Real time applications can use UDP and add their own ways of handling problems, such as hiding a small gap, lowering quality, or continuing with the newest audio. This paper establishes the idea of reliable communication but in todays internet gives applications more choices about whether perfect delivery or low delay matters more

### D. Surprise Finding

Originally I had thought that if you resend every lost packet then it would always make an internet connection work better. I learnt that this is helpful for a file download because  every piece of it must come correctly ordered, even if the download takes a bit longer. During a call, waiting for an old piece of audio can create more delay which is less functional, so it would be better to skip it and continue with the newest sound.

### E. AI Conversation Documentation

** TRANSCRIPT **
ME: when my internet gets bad my discord call starts sounding robotic but my download still works are they both losing packets or is it something else

AI: They could both be losing packets, but they respond to the problem differently. A file download needs every piece of data to arrive correctly. If something is missing, the file could be damaged or might not open. A live call cares more about delivering the newest audio quickly. Waiting too long for missing audio could delay the conversation, so the call may continue without it. That is why the call might sound robotic while the file continues downloading normally.

ME: so if part of the file goes missing how does it know what part it needs to send again

AI: TCP gives the data sequence numbers before sending it. The receiver uses those numbers to determine where each piece belongs and whether anything is missing. It also sends acknowledgments back to confirm which data arrived. If the sender does not receive the expected acknowledgment, or the receiver reports a gap, the missing data can be sent again. Once everything arrives, TCP puts the pieces in the correct order before giving the completed file to the application.

ME: does that mean discord cant resend the missing audio because the call has to keep going

AI: Discord can sometimes resend missing audio, but there is only a short amount of time for doing it. If the replacement arrives before that part of the audio needs to be played, resending it may improve the sound. If it arrives too late, it is no longer useful because the conversation has already moved forward. Hearing an old word several seconds late could make the call even more confusing. Discord has to decide whether recovering the sound is worth the extra delay.

ME: what does it do if the audio cant be sent again in time is that why it starts sounding robotic

AI: That can be one reason the audio sounds robotic. Voice software can try to cover a small gap by stretching nearby audio or estimating what could fit in the missing section. This is called packet-loss concealment. It can hide a small amount of packet loss, but it cannot perfectly recreate audio that never arrived. If several packets are lost close together, there is less information available to cover the gap. This can cause robotic sound, missing words, freezing, or a brief disconnection.

ME: i thought sending lost packets again would always make the connection better so how could it make a call worse

AI: Resending lost packets improves accuracy, but it also takes extra time. That delay is usually acceptable during a file download because getting the complete file matters more than getting it immediately. During a live conversation, delay can cause people to interrupt each other or think the call has frozen. An old piece of audio may have little value by the time it finally arrives. For a live call, receiving most of the audio on time can create a better experience than receiving every piece late.

ME: so is this basically the difference between tcp and udp or is there more to it

AI: It is an important part of the difference. TCP provides reliable and ordered delivery. It tracks data, confirms what arrives, and retransmits missing pieces. UDP sends separate packets without automatically guaranteeing that they will arrive or remain in order. This gives a live application more control over what to do when something is lost. It can ignore an old packet, try a quick retransmission, or use another method to hide the gap. Discord uses UDP to carry live voice data because keeping the delay low is important, although it uses other connections to set up and control the call.

ME: how does this connect back to what cerf and kahn talked about in the 1974 paper

AI: Cerf and Kahn understood that packets could be lost, damaged, duplicated, or delivered out of order while crossing different networks. They proposed handling reliability between the two endpoints. Their design used sequence numbers, acknowledgments, error checking, and retransmissions so the receiving computer could detect problems with the data. These ideas are still used in modern TCP and help explain why a file can finish correctly even when packets are lost along the way.

Their original design later developed into the separate TCP and IP layers used today. IP is responsible for moving packets across different networks, while TCP can add reliable and ordered delivery when an application needs it. Modern applications can also use UDP when receiving data quickly is more important than receiving every packet. What surprised me is that packet loss does not have one solution that works best for everything. A download can wait for missing data, but a live call may work better by skipping old data and continuing with the newest audio.


## Part 3: Reflection

The most surprising problem that interested me the most is packet loss because of my own experience with hearing robotics audio on discord but never knowing what was the cause of it. The ai investigation showed me that a download and a voice call can have the same network problem but they're handled differently. I thought every missing packet should always be sent again but after seeing and learning more about it, I think differently now. I understand that the internet doesn't handle all data the same, because each application has to decide if they want to focus more on speed or accuracy. 