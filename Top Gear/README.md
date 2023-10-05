# Top Gear

I started working on Top Gear using bsnes-plus but recently changed to Mesen2.
Because of this, there are some old files using the bsnes-plus formats, but all the recent ones use Mesen2 formats

https://github.com/SourMesen/Mesen2/

Also, everything is working on the US version (and possibly JPN). If something is supposed to work on the EU version, it will be explicit (but there is nothing yet).

## Top Gear (U) [!].cpu.sym

[deprecated] Contains the labels used by snes-plus

## Top Gear (USA).lua

Some messing around with a Lua script with real-time information on screen including a line graph with the position of P1 and P2 in the track.
Works on Bizhawk 2.6.1 using BSNES core.

## Top Gear (USA).wch

RAM Watch file for Bizhawk 2.6.1
It contains some addresses found basically with curiosity/trial and error using bizhawk's RAM Search tool.

Most if not all, should have been migrated to the Mesen2 labels file

## top-gear-csv.lua

Creates a csv file with multiple information about every frame during a race. Useful for speedruns to study the acceleration, speed, fuel, etc of the cars.