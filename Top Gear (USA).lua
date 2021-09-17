function initializeScreenConstants()
    padding = 15
    circleBorderColor = "gray"
    circleSize = 4
    lapLineSize = 4
end

function initializeCarColors()
    carColor = {}
    carColor[0] = "crimson"
    carColor[1] = "white"
    carColor[2] = "darkmagenta"
    carColor[3] = "dodgerblue"
end

function initializeFuelConsumptionVariables()
    totalFuel = 43007
    lastFuel = {totalFuel,totalFuel}
    fuelConsumption = {{},{}}
    memoryFuel = {totalFuel,totalFuel}
end

function updateScreenSize()
    screenHeight = client.screenheight()
    screenWidth = client.screenwidth()
    infoXPosition = screenWidth / 3 * 2
    infoYPosition = {screenHeight / 4, screenHeight / 4 * 3}

    halfHeight = client.bufferheight() / 2
    lineLength = client.bufferwidth() - 2 * padding
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
    local fuelRatio = fuel / totalFuel

    local engine = memoryEngine[player]

    fuelText = 'Fuel: ' .. fuel
    fuelRatioText = 'Fuel ratio: ' .. fuelRatio
    engineText = 'Engine: ' .. engine

    gui.text(x, y, fuelText)
    gui.text(x, y + lineBreakSize, fuelRatioText)
    gui.text(x, y + lineBreakSize * 2, engineText)
end


function drawCompletionGraphLine(numberOfLaps,lapSize)
    gui.drawLine(padding, halfHeight, padding + lineLength, halfHeight)
end

function drawCompletionGraphLaps(numberOfLaps,lapSize)
    local total = numberOfLaps * lapSize
    for i = 0, numberOfLaps
    do
        local currentLapPosition = i * lapSize
        local lapPercentage = currentLapPosition / total
        local lapX = padding + lineLength * lapPercentage
        gui.drawLine(lapX, halfHeight - lapLineSize / 2, lapX, halfHeight + lapLineSize / 2)
    end
end

function drawPlayerCircle(player, lapSize, numberOfLaps)
    local currentLap = memoryLap[player]
    local currentPositionOnTrack = memoryPosition[player]
    local current = currentLap * lapSize + currentPositionOnTrack
    local total = numberOfLaps * lapSize
    local percentage = current / total
    local circleCenter = padding + lineLength * percentage
    local car = memoryCar[player]
    gui.drawEllipse(circleCenter - circleSize / 2, halfHeight - circleSize / 2, circleSize, circleSize, circleBorderColor, carColor[car])
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
    local currentFrame = emu.framecount()
    local MAX_ACCEPTED_DIFFERENCE = 1000
    for i = 1,2
    do 
        local difference = lastFuel[i] - memoryFuel[i]
        if(difference > 0 and difference < MAX_ACCEPTED_DIFFERENCE)
        then
            lastFuel[i] = memoryFuel[i]
            fuelConsumption[i][currentFrame] = difference
        end
    end
end

function isInCountdown(frameCounter)
    local LAST_FRAME_OF_COUNTDOWN_RANGE_START = 200
    local LAST_FRAME_OF_COUNTDOWN_RANGE_END = 600
    return frameCounter > LAST_FRAME_OF_COUNTDOWN_RANGE_START and frameCounter < LAST_FRAME_OF_COUNTDOWN_RANGE_END
end

function isInARace()
    local currentTrackFrameCounter = memoryTrackFrameCounter
    local response = false
    if(lastTrackFrameCounter ~= currentTrackFrameCounter)
    then
        response = currentTrackFrameCounter > 0 or isInCountdown(lastTrackFrameCounter)
        lastTrackFrameCounter = currentTrackFrameCounter
    end
    return response
end

function initializeEverything()
    initializeCarColors()
    initializeScreenConstants()
    initializeFuelConsumptionVariables()
    updateScreenSize()
    clearScreen()
end

function gameLoop()
    while true do
        updateScreenSize()
        clearScreen()
        readMemoryData()
        if(isInARace())
        then
            drawCompletionGraph()
            for i = 1,2
            do
                drawPlayerInfo(i)
            end
        end
        emu.frameadvance()
    end
end

console.log('\n\nInitialized program\n\n')
initializeEverything()
gameLoop()

