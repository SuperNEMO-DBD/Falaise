====================================
Information for mapping tracker file
====================================

Author : Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>

Infos
-----

mapping_calo.csv
----------------

X : board 0 for example (cable 13 OMs)
Y : board 1 for example (cable 13 other OMs)

#SlotID Channel Column Row
X 0 0 0
X 1 0 1
X 2 0 2
X 3 0 3
X 4 0 4
X 5 0 5
X 6 0 6
X 7 0 7
X 8 0 8
X 9 0 9
X 10 0 10
X 11 0 11
X 12 0 12
Y 0 1 0
Y 1 1 1
Y 2 1 2
Y 3 1 3
Y 4 1 4
Y 5 1 5
Y 6 1 6
Y 7 1 7
Y 8 1 8
Y 9 1 9
Y 10 1 10
Y 11 1 11
Y 12 1 12


mapping_tracker.csv
-------------------
This file reproduce the hardware mapping between a geiger cell and the anodic and cathodic channels

The file must respect some rules. The cell number must be from 0 to 35 and the order must not be changed !

On a board, you have 54 channel / Feast and there is 2 Feasts per board. The total channel on a board is 108.

The channel rule for the mapping is : Feast number (0-1) * 54 + Channel. A channel is unique between 0 and 107

Ex : FEAST 1 CHANNEL 12 -> Channel = 66 (in the actual mapping - see the SNDER p30-31, this is the anodic channel of the cell #22)

SlotID CellLayer CellRow AnodicChannel BotCathodeChannel TopCathodeChannel
X 0 0 X X X
X 1 0 X X X
X 2 0 X X X
X 3 0 X X X
X 4 0 X X X
X 5 0 X X X
X 6 0 X X X
X 7 0 X X X
X 8 0 X X X
X 0 1 X X X
X 1 1 X X X
X 2 1 X X X
X 3 1 X X X
X 4 1 X X X
X 5 1 X X X
X 6 1 X X X
X 7 1 X X X
X 8 1 X X X
X 0 2 X X X
X 1 2 X X X
X 2 2 X X X
X 3 2 X X X
X 4 2 X X X
X 5 2 X X X
X 6 2 X X X
X 7 2 X X X
X 8 2 X X X
X 0 3 X X X
X 1 3 X X X
X 2 3 X X X
X 3 3 X X X
X 4 3 X X X
X 5 3 X X X
X 6 3 X X X
X 7 3 X X X
X 8 3 X X X
