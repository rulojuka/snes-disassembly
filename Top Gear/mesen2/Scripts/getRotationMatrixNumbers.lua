-- This script will print the tables
-- for the mode7 affine transformations used in the rotation
-- for the title screen text: "Top" and "Gear".

-- It runs on Mesen2. Use it while the rotation animation runs
-- and then you can just copy the values from the table.

myTable = {}
index = {}
startAddress = 0x1e
size = 16

for i = 0, size-1 do
        index[i] = startAddress + i
        myTable[index[i]]=0
end

function myCallback(address,value)
	myTable[address]=value
end

function printTable()
	local response = ""
	for i = 0, size-1 do
        local address = index[i]
        if (i>0) then
        	response = response .. ","
        end
		response = response .. string.format("%02x",myTable[address])
    end
    emu.log(response)
end

function printTable2()
	local response = ""
	for i = 0, size-1 do
		if (i%2==0) then
	        if (i>0) then
	        	response = response .. ","
	        end
			response = response .. string.format("%02x%02x",myTable[index[i+1]],myTable[index[i]])
		end
    end
    emu.log(response)
end

function mainLoop()
-- Does nothing
end

emu.addMemoryCallback(myCallback,emu.callbackType.read,0x00001e,0x00002d,emu.cpuType.snes,emu.memType.snesMemory)
emu.addEventCallback(printTable2, emu.eventType.startFrame);