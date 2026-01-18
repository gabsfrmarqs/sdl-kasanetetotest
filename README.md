# sdl-kasanetetotest

This amazing program displays a Kasane Teto animation and plays a poorly made version of Jaime Paige - Machine Love!


# Why
I was just bored and thought:
> have i done everything i could today to worship Kasane Teto?

I figure I didn't, so I decided to play a bit with SDL for the first time.

First, I needed a chiptune version, but I didn't want to just download one from the internet, so I decided to make one. I stared at [JerryEatsPizza's](https://musescore.com/user/5378366/scores/23684371?share=copy_link) provided sheet for some time and made a version in a random piano midi website by looking at just the sheet itself - which was already a major accomplishment for me, I have NO skills in music and just started learning sheet music november 2025.

Turning the .mid into a chiptune was easy, I imported into [beepbox](https://www.beepbox.co/#9n20sbk0l00e0ft2La3g0fj0ar1i0o53T0v0u00f0qg01d04w1h0E0T0v1u00f0qgw12v42d04w1h0E0b4yd5pUCjiCIid59BsCho00p22hFH-4309EYmTiFOifaFGQGs1waYyA96-nOltuJ8kTkT7Yi-hFE_kR_tun0Wld9vljlGAupttjtfXD7FE-qd2yyyyyyzgEEEEEpE-4szgEEEEEEEQaaaaaaqd2yyyyyCyhQk4t17jhEkkkkkk4o), changed both tracks to triangle wave type. Pulse wave sounded good, but triangle sounded more... comfortable?????? ()

The SDL part was basically made just following the examples given in their [website](https://examples.libsdl.org/SDL3/). I'll keep the original code comments as future reference for me.


# Build
Really hard to build. You must have SDL3.

```c++
g++ machinelove.cpp -o app $(pkg-config --cflags --libs sdl3) && ./app
```
Good luck

