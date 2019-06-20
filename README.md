# SAW Biosensor
Senior Design Project 2018-2019
Kyle O'Rourke

The Slug Scope user software allows the user to operate the Biosaw Lite hardware with only a few clicks. The user simply needs to plug in the device to their computer, select the device COM port, and run the software, everything else is handled by the system.

Once the software begins, a reset command is sent to the Biosaw Lite to ensure that the system is not running any commands outside of the Slug Scope’s expected control frame. After the device has reset and finished its setup routine, it will send a serial message to the Slug Scope software letting it know that the system is ready for new commands. In the bottom left corner a serial monitor feed is provided and displays all communications to and from the device. The Biosaw Lite can also be easily controlled outside of the user software, simple one line serial commands through any serial monitor software can initiate frequency sweeps or single tone outputs. The list of possible commands are shown in the table below.


Command Examples:<br>
------------------------ <br>
Set Frequency: <br>
F 10000000

Sweep: <br>
S startFrq endFrq stepSize <br>
S 45000000 60000000 10000

Help:
“Help” - or any invalid command.
