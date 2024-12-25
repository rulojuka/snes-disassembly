# General discoveries

## The title screen rotation for "Top" and "Gear"

It works using mode 7.

Starting points:
    PRG:$078DD4 / CPU:$0F8DD4 - Set mode 7
    PRG:$078E1D / CPU:$0F8E1D - Rotate Text "Top"
    PRG:$078E7B / CPU:$0F8E7B - Rotate Text "Gear"
    PRG:$079075 / CPU:$0F9075 - Set Mode 1 for rest of game
    PRG:$6AC51 -> $6AE4E - Rotation data (sines and cosines) for "Top" and "Gear" rotation

## When is the race over?

The condition for the race to end is as follows:

If 10 players already finished -> END RACE
    PRG:$000783 / CPU:$008783 - Check if #$0A (10) players have already finished
If both players are inelegible to race -> END RACE
    PRG:$000788 / CPU:$008788 - Finish race if both players are ineligible to win.
    WRAM:$1EB4 - Are players eligible to win?

Reasons to be ineligible include:
    - Crossing the finish line.
    - Having no fuel
