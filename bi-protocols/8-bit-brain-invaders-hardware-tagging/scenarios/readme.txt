Create a class: OpenVibeGame

Methods:

SetTargetRow(int)
SetTargetRow(int)

A single flash requires ONE of thse methods:
SetFlashRow(int)
SetFlashColumn(int)

SetBeginProcessing() 192

Train(repetions,number of flashes) - calls in the end 

-sets target 
-performs flashes
-SetBeginProcessing()

==============================


1) Brain invaders will do the training

and instead of flashes (64 code) will send the numbers corresponding to each row and column.

100 - 110 rows
120 - 130 columns


2) acquire.xml must be able to convert row codes to stimulations like this:


This stimulation is used as a basis for the row flashes. For example, flashing the first row consists in sending this exact stimulation code to the first input. Flashing the second row needs this stimulation code + 1 and so on.

100 - 110 rows
120 - 130 columns

First Stim1 is for 192 training and flashes
Second Stim2 is only for target

3) Gijs to implement the training:

Several repetions:

100 - 110 rows
120 - 130 columns

128

And in the end:

192


4) Target selection is not included in the parallel port communication!!!