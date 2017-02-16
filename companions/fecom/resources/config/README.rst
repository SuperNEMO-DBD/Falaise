====================================
Information for mapping tracker file
====================================

Author : Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>

Infos
-----

This file reproduce the hardware mapping between a geiger cell and the anodic and cathodic channels

The file must respect some rules. The cell number must be from 0 to 35 and the order must not be changed !

On a board, you have 54 channel / Feast and there is 2 Feasts per board. The total channel on a board is 108.

The channel rule for the mapping is : Feast number (0-1) * 54 + Channel. A channel is unique between 0 and 107

Ex : FEAST 1 CHANNEL 12 -> Channel = 66 (in the actual mapping - see the SNDER p30-31, this is the anodic channel of the cell #22)

CellLayer CellRow AnodicChannel BotCathodeChannel TopCathodeChannel
0 0 X X X
1 0 X X X
2 0 X X X
3 0 X X X
4 0 X X X
5 0 X X X
6 0 X X X
7 0 X X X
8 0 X X X

0 1 X X X
1 1 X X X
2 1 X X X
3 1 X X X
4 1 X X X
5 1 X X X
6 1 X X X
7 1 X X X
8 1 X X X

0 2 X X X
1 2 X X X
2 2 X X X
3 2 X X X
4 2 X X X
5 2 X X X
6 2 X X X
7 2 X X X
8 2 X X X

0 3 X X X
1 3 X X X
2 3 X X X
3 3 X X X
4 3 X X X
5 3 X X X
6 3 X X X
7 3 X X X
8 3 X X X
