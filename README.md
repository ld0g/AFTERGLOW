# AFTERGLOW
Arduino based 4-zone automotive addressable LED lighting controller

AFTERGLOW takes inspiration from the underglow culture from the tuning scene of the early 2000s and implements the same idea with modern hardware.
Originally developed for a Subaru Legacy BR9 Touring Wagon, AFTERGLOW replaces a conventional underglow controller (gross cheap remote) with the factory SI-DRIVE switch.

Project status: 
software - v0.7 NIGHTFALL. functionally complete firmware. 
next milestone - AFTERGLOW v1.0 - hardware implementation on a real car and any final niggles fixed.

Hardware:
-Arduino Nano Every
-Waterproof enclosure
-12V to 5V buck converter
--TBC--

Features:
-Separate 12 V and 5 V buses for LED power and Arduino power respectively
-OEM Subaru SI-DRIVE switch controls
-4 independent WS2815 LED strips
-short/long press behaviour
-configurable colours and brightness
-temporary animation overrides
-modular design with replaceable LED strips and failsafe

Lighting Modes:
STEALTH - All lights off. Default mode.
HELLO - A short rainbow welcome animation. Temporarily overrides all other modes.
NORMAL - Static colour
KAWAII - Static colour with blue bands slowly travelling front to rear
CHAOS - A sequence of animated effects including strobes, perimeter chase. use with caution.
BLACKOUT - Software lockout that disables all lighting effects until purposely disengaged with a 5-second PUSH input

Building and Uploading:
-Open the .ino file in Arduino IDE and configure your LED counts, data pins and input method
-Upload to your weapon of choice
-IMPORTANT: The LED strips require their own suitable 12V supply. Do not attempt to power them directly from the Arduino.

Safety and intended use:
AFTERGLOW is an experimental project intended for stationary use only at shows and meets. 
Automotive electronics might not shock you, but the high current presents a significant fire risk (i know). ALWAYS use appropriate fuses and wire gauges.

About the project:
What began as 'it would be fun to put pink lights under my new Subaru' quickly snowballed. 
As the weeks went on, I had the idea to make use of the redundant OEM switch taking up unnecessary space in my centre console. Then came short and long press inputs to stretch the functionality of the switch. 
More inputs need more modes, which were filled with CHAOS, BLACKOUT and KAWAII.
Developed on a printer paper box with lights and a breadboard stuck to it. Many fun evenings were had playing about seeing how the effects work in the real world. Timings were tuned and fadeouts implemented to reduce jankiness.

If you are installing AFTERGLOW, I would love to see the results. Please share your platforms, input choices, vehicles, builds. This doesn't even have to go on a car...
