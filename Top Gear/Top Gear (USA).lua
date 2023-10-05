---@diagnostic disable: lowercase-global, undefined-global

previousTrackFrameCounter = 0

function initializeScreenConstants()
    PADDING = 15
    CIRCLE_BORDER_COLOR = "gray"
    CIRCLE_SIZE = 4
    LAP_LINE_SIZE = 4
end

function initializeGameConstants()
    CAR_COLOR = {}
    CAR_COLOR[0] = "crimson"
    CAR_COLOR[1] = "white"
    CAR_COLOR[2] = "darkmagenta"
    CAR_COLOR[3] = "dodgerblue"
    MAX_FUEL = 43007
    LAST_FRAME_OF_COUNTDOWN_RANGE_START = 200
    LAST_FRAME_OF_COUNTDOWN_RANGE_END = 600
end

function updatePreviousTrackFrameCounter()
    previousTrackFrameCounter = memoryTrackFrameCounter
end

function initializeFuelConsumptionVariables()
    lastFuel = {MAX_FUEL,MAX_FUEL}
    fuelConsumption = {{},{}}
    memoryFuel = {MAX_FUEL,MAX_FUEL}
end

function updateScreenSize()
    screenHeight = client.screenheight()
    screenWidth = client.screenwidth()
    infoXPosition = screenWidth / 3 * 2
    infoYPosition = {screenHeight / 4, screenHeight / 4 * 3}

    halfHeight = client.bufferheight() / 2
    lineLength = client.bufferwidth() - 2 * PADDING
end

function clearScreen()
    gui.clearGraphics()
    gui.cleartext()
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
end

function drawPlayerInfo(player)
    if(player<1 or player>2) then return end

    local x = infoXPosition
    local y = infoYPosition[player]
    local lineBreakSize = 15
 
    local fuel = memoryFuel[player]
    local fuelRatio = fuel / MAX_FUEL

    local engine = memoryEngine[player]

    local speed = memorySpeed[player]

    fuelText = 'Fuel: ' .. fuel
    fuelRatioText = 'Fuel ratio: ' .. fuelRatio
    engineText = 'Engine: ' .. engine
    speedText = 'Speed: ' .. speed

    gui.text(x, y, fuelText)
    gui.text(x, y + lineBreakSize, fuelRatioText)
    gui.text(x, y + lineBreakSize * 2, engineText)
    gui.text(x, y + lineBreakSize * 3, speedText)
end

function drawCompletionGraphLine(numberOfLaps,lapSize)
    gui.drawLine(PADDING, halfHeight, PADDING + lineLength, halfHeight)
end

function drawCompletionGraphLaps(numberOfLaps,lapSize)
    local total = numberOfLaps * lapSize
    for i = 0, numberOfLaps
    do
        local currentLapPosition = i * lapSize
        local lapPercentage = currentLapPosition / total
        local lapX = PADDING + lineLength * lapPercentage
        gui.drawLine(lapX, halfHeight - LAP_LINE_SIZE / 2, lapX, halfHeight + LAP_LINE_SIZE / 2)
    end
end

function drawPlayerCircle(player, lapSize, numberOfLaps)
    local currentLap = memoryLap[player]
    local currentPositionOnTrack = memoryPosition[player]
    local current = currentLap * lapSize + currentPositionOnTrack
    local total = numberOfLaps * lapSize
    local percentage = current / total
    local circleCenter = PADDING + lineLength * percentage
    local car = memoryCar[player]
    gui.drawEllipse(circleCenter - CIRCLE_SIZE / 2, halfHeight - CIRCLE_SIZE / 2, CIRCLE_SIZE, CIRCLE_SIZE, CIRCLE_BORDER_COLOR, CAR_COLOR[car])
end

function drawCompletionGraphPlayers(numberOfLaps,lapSize)
    for i = 2,1,-1
    do
        drawPlayerCircle(i, lapSize, numberOfLaps)
    end
end

function drawCompletionGraph()
    local lapSize = memoryLapSize
    local numberOfLaps = memoryNumberOfLaps
    drawCompletionGraphLine(numberOfLaps, lapSize)
    drawCompletionGraphLaps(numberOfLaps, lapSize)
    drawCompletionGraphPlayers(numberOfLaps, lapSize)
end

function updateConsumption()
    local currentRaceFrame = memoryTrackFrameCounter
    local MAX_ACCEPTED_DIFFERENCE = 1000
    for i = 1,2
    do
        local difference = lastFuel[i] - memoryFuel[i]
        if(difference > 0 and difference < MAX_ACCEPTED_DIFFERENCE)
        then
            lastFuel[i] = memoryFuel[i]
            fuelConsumption[i][currentRaceFrame] = difference
        end
    end
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

function initializeEverything()
    initializeGameConstants()
    initializeScreenConstants()
    initializeFuelConsumptionVariables()
    updateScreenSize()
    clearScreen()
end

function gameLoop()
    while true do
        readMemoryData()
        updateScreenSize()
        clearScreen()
        if(isInARace())
        then
            drawCompletionGraph()
            for i = 1,2
            do
                drawPlayerInfo(i)
            end
        else
            initializeFuelConsumptionVariables()
        end

        updatePreviousTrackFrameCounter()
        emu.frameadvance()
    end
end

console.log('\n\nInitialized program\n\n')
initializeEverything()
gameLoop()

