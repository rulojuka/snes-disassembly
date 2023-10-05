--This is an example Lua (https://www.lua.org) script to give a general idea of how to build scripts
--Press F5 or click the Run button to execute it
--Type "emu." to show a list of all available API function

--Get the emulation state
state = emu.getState()
frameCount = 0
p1FuelWrites = 0
boxTopX = 8 -- left to right
boxWidth = 128
boxTopY = 130 -- up to bottom
numberOfElements = 2
boxHeight = 12 * numberOfElements
function getYPosition(elementNumber)
	return boxTopY + elementNumber*12
end

NEVER_DISAPPEAR = 0

function printInfo()
	emu.clearScreen()
	frameCount = frameCount + 1
	
	--Get the emulation state
	state = emu.getState()
	bgColor = 0x30FF6020
	fgColor = 0x304040FF

	--Draw some rectangles and print some text
	emu.drawRectangle(boxTopX, boxTopY, boxWidth, boxHeight, bgColor, true, NEVER_DISAPPEAR)
	emu.drawString(boxTopX, getYPosition(0), "Frames since script: " .. frameCount, 0xFFFFFF, 0xFF000000,0,NEVER_DISAPPEAR)
	-- fuel is usually written once every 3 frames
	-- but while p2 is at pitstop, it is written every 2 frames
	emu.drawString(boxTopX, getYPosition(1), "Fuel writes: " .. p1FuelWrites, 0xFFFFFF, 0xFF000000,0,NEVER_DISAPPEAR)
	
end

function hasUpdatedP1Fuel()
	p1FuelWrites = p1FuelWrites + 1
end

--Register some code (printInfo function) that will be run at the end of each frame
emu.addEventCallback(printInfo, emu.eventType.endFrame);

emu.addMemoryCallback(hasUpdatedP1Fuel, emu.callbackType.write, 0x1E62)

--Display a startup message
emu.displayMessage("Script", "Example Lua script loaded.")