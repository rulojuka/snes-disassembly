---@diagnostic disable: lowercase-global, undefined-global

function initializeGameConstants()
    CAR_COLOR = {}
    -- Hex colors were picked from the actual game map
    -- Named colors are just approximations
    CAR_COLOR[0] = 0xFF9C1818 -- "crimson"
    CAR_COLOR[1] = 0xFFBDBDDE -- "white"
    CAR_COLOR[2] = 0xFF7B187B -- "darkmagenta"
    CAR_COLOR[3] = 0xFF186B9C -- "dodgerblue"
    MAX_FUEL = 43007
    LAST_FRAME_OF_COUNTDOWN_RANGE_START = 200
    LAST_FRAME_OF_COUNTDOWN_RANGE_END = 600

    NAMES_OFFSET = 0x07AC93
    NAMES_SIZE = 26
    NAMES_QUANTITY = 32
    MAX_NAME_LENGTH = 120
    NAMES = {}
    array = {}
    array[0] = 0x07AC93
    array[1] = 0x07ACAD
    array[2] = 0x07ACC9
    array[3] = 0x07ACE2
    array[4] = 0x07AD00
    array[5] = 0x07AD15
    array[6] = 0x07AD31
    array[7] = 0x07AD4F
    array[8] = 0x07AD6C
    array[9] = 0x07AD82
    array[10] = 0x07AD9C
    array[11] = 0x07ADB5
    array[12] = 0x07ADCB
    array[13] = 0x07ADE4
    array[14] = 0x07ADFE
    array[15] = 0x07AE1D
    array[16] = 0x07AE39
    array[17] = 0x07AE54
    array[18] = 0x07AE71
    array[19] = 0x07AE8C
    array[20] = 0x07AEA2
    array[21] = 0x07AEB9
    array[22] = 0x07AECF
    array[23] = 0x07AEE9
    array[24] = 0x07AF01
    array[25] = 0x07AF16
    array[26] = 0x07AF2B
    array[27] = 0x07AF42
    array[28] = 0x07AF5B
    array[29] = 0x07AF74
    array[30] = 0x07AF90
    array[31] = 0x07AFAD

    for i = 0, NAMES_QUANTITY-1 do
        offset = array[i]
        text = ""
        for j=0, MAX_NAME_LENGTH do
            address = offset + j
            memory_char = memory.readbyte(address,"CARTROM")
            if(memory_char == 0) then
                break
            else
                text = text .. string.char(memory_char)
            end
        end
        NAMES[i] = text
    end
end

function initializeMemoryVariables()
    SPEED = "speed"
    GEAR = "gear"
    ENGINE = "engine"
    FUEL = "fuel"
    memoryValues = {}
    memoryValues[SPEED] = {}
    memoryValues[GEAR] = {}
    memoryValues[ENGINE] = {}
    memoryValues[FUEL] = {}
end

function updatePreviousTrackFrameCounter()
    previousTrackFrameCounter = memoryTrackFrameCounter
end

function getMemoryDataForBothPlayers(address, offset)
    return {memory.read_u16_le(address), memory.read_u16_le(address+offset)}
end

function readMemoryData()
    memoryLapSize = memory.read_u16_le(0x0200)
    memoryTrackFrameCounter = memory.read_u16_le(0x1EAE)
    memoryNumberOfLaps = memory.read_u16_le(0x1E56)
    memoryRaceCountdownOn = memory.read_u16_le(0x1E58)
    memorySpeed = getMemoryDataForBothPlayers(0x1E5A, 2)
    memoryEngine = getMemoryDataForBothPlayers(0x1E5E, 2)
    memoryFuel = getMemoryDataForBothPlayers(0x1E62, 2)
    memoryLap = getMemoryDataForBothPlayers(0x1E76, 2)
    memoryPosition = getMemoryDataForBothPlayers(0x1ED7, 3)
    memoryFinishTime = getMemoryDataForBothPlayers(0x1F26, 2)
    memoryCar = getMemoryDataForBothPlayers(0x1F8F, 29)
    memoryGear = getMemoryDataForBothPlayers(0x1E72, 2)
    memoryTrackNumber = memory.read_u16_le(0x1F06)
end

function isInCountdown()
    return memoryRaceCountdownOn == 1
end

function isInFrameJustAfterCountdown(currentFrameCounter, previousFrameCounter)
    return currentFrameCounter == 0
    and previousFrameCounter > LAST_FRAME_OF_COUNTDOWN_RANGE_START
    and previousFrameCounter < LAST_FRAME_OF_COUNTDOWN_RANGE_END
end

function isInARace()
    local currentTrackFrameCounter = memoryTrackFrameCounter
    if previousTrackFrameCounter == nil then previousTrackFrameCounter = 0 end
    local frameDifference = currentTrackFrameCounter - previousTrackFrameCounter
    local response =  isInCountdown()
    or isInFrameJustAfterCountdown(currentTrackFrameCounter, previousTrackFrameCounter)
    or frameDifference == 1

    -- if(response) then gui.text(100,100,"IN A RACE") else gui.text(100,100,"NOT IN A RACE") end
    -- gui.text(100,115,currentTrackFrameCounter)
    -- gui.text(100,130,previousTrackFrameCounter)
    -- if(isInCountdown()) then gui.text(100,145,"COUNTING DOWN") else gui.text(100,145,"NOT COUNTING DOWN") end

    return response
end

function playerOneHasFinished()
    return memoryNumberOfLaps<10 and memoryLap[1] < 10 and memoryNumberOfLaps == memoryLap[1]
end

function updateMemoryValues()
    memoryValues[SPEED][memoryTrackFrameCounter] = memorySpeed[1]
    memoryValues[GEAR][memoryTrackFrameCounter] = memoryGear[1]
    memoryValues[ENGINE][memoryTrackFrameCounter] = memoryEngine[1]
    memoryValues[FUEL][memoryTrackFrameCounter] = memoryFuel[1]
end

function flushOutput()
    local current_date_time_string = os.date("%Y-%m-%d_%H-%M-%S", os.time())
    filename = current_date_time_string .. "-" .. NAMES[memoryTrackNumber] .. ".csv"
    filewrite = io.open(filename, "w")
    stringOutput = 'frame,speed,engine,fuel,gear'
    filewrite:write(stringOutput)
    filewrite:write("\n")

    for i=0, memoryTrackFrameCounter do
        currentFrame = i
        currentSpeed = memoryValues[SPEED][i]
        currentEngine = memoryValues[ENGINE][i]
        currentFuel = memoryValues[FUEL][i]
        currentGear = memoryValues[GEAR][i]

        filewrite:write(currentFrame)
        filewrite:write(",")
        
        filewrite:write(currentSpeed)
        filewrite:write(",")

        filewrite:write(currentEngine)
        filewrite:write(",")

        filewrite:write(currentFuel)
        filewrite:write(",")

        filewrite:write(currentGear)
        filewrite:write("\n")
    end

    filewrite:close()
    notFlushedOutputYet = false
    console.log("FLUSHED OUTPUT BECAUSE PLAYER ONE HAS FINISHED")
end


function initializeEverything()
    initializeGameConstants()
    initializeMemoryVariables()
end

function gameLoop()
    while true do
        readMemoryData()

        if(isInFrameJustAfterCountdown())
        then
            notFlushedOutputYet = true
        end

        if(isInARace())
        then
            updateMemoryValues()
        else
            notFlushedOutputYet = true
        end

        if(notFlushedOutputYet and playerOneHasFinished())
        then
            flushOutput()
        end

        updatePreviousTrackFrameCounter()
        emu.frameadvance()
    end
end

notFlushedOutputYet = true
console.log('\n\nInitialized program\n\n')
initializeEverything()
gameLoop()

