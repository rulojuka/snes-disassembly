# Top Gear 2

I started working on Top Gear using bsnes-plus. The main goal was to create a password decoder/encode inspired by Bisqwit's [playlist of Cracking Videogame Passwords](https://www.youtube.com/watch?v=0eQyYrSQPew&list=PLzLzYGEbdY5nEFQsxzFanSDv_38Hz0w7B)

After a while I discovered that someone else had this same idea a long time ago, you can find it [here](https://www.speedrun.com/top_gear_2/resources/u86lv) or searching the internet for the creator: rimsky82.
He also found a [very neat thing about the Gas Tank](https://forum.gamehacking.org/forum/video-game-hacking-and-development/hacker-threads/5144-rimsky82-s-codes?p=141792#post141792)

After seeing his password encoder/decoder I was pretty glad that mine gave the same results with all the inputs I tried, so I called this project a success and closed it, even if I was a little disappointed that no easter egg seem to be included in the password system.

## Files

### Top Gear 2 (U) [!].cpu.sym

Contains the labels used by snes-plus

### Top Gear 2 (USA).wch

RAM Watch file for Bizhawk 2.6.1
It contains some addresses found basically with curiosity/trial and error using bizhawk's RAM Search tool.

### top-gear-2-password-encoder.cpp

My implementation of the encoder as close to what the SNES does.

### top-gear-2-password-encoder-simplified.cpp

My implementation of the encoder with some refactored code (that should return the exact same result)