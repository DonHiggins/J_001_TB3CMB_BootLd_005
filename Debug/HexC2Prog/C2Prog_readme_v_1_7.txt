Supported file format:
----------------------
C2Prog only supports Intel-Hex files, which can be generated from the
COFF files with the following command: 

(c2400-tools)
dsphex -romwidth 16 -memwidth 16 -i -o .\Debug\code.hex  .\Debug\code.out

(c2000-tools)
hex2000 -romwidth 16 -memwidth 16 -i -o .\Debug\test.hex .\Debug\test.out

(msp430-tools)
hex430 -order MS -romwidth 16 -i -o .\Debug\test.hex .\Debug\test.out

(470-tools)
hex470 -order MS -romwidth 8 -memwidth 8 -i -o .\Debug\test.hex .\Debug\test.out

Supported Targets:
------------------
C2Prog primarily supports MCUs from the TI C2000 family.

If your target or clock frequency is not currently supported, let us know - 
we will try to provide you with the necessary configurations files. 

Note, however, that we can not support the 2402, 2403 or 28020/280200 due to the limited size 
of its internal RAM.

Release notes:
--------------

This is version 1.7p (8/26/17)
Support F28M36 Concerto devices.
Add framework for encryption plugins.
Improve error messages.

~~~
1.7o (6/25/17)
Support 2807x Z1 CSM and allow OTP programming.
Support 2837xS/D Z1 CSM and allow OTP programming.
Support Z1 CSM unlocking of Concerto cores via UART loader (only for devices featuring shared RAM).
Expose Gdb server stub.

~~~
1.7n (5/13/17)
Extend c2p API with additional methods (c2p.dll v2.3).
Permit mixed Flash/OTP programming.
Add JTAG reflashing for Concerto C28 core.
Support Z1 CSM unlocking of Concerto cores via JTAG loader (only for devices featuring shared RAM).
Support Concerto M3 core OTP programming.

~~~
1.7m (3/25/17)
Address 2837xD ECC silicon errata.
Improve display of emulator error messages.
Parse "c2pemuconfig" folder in user directory.

~~~
1.7l (2/5/17)
Allow "ihx" extension.

~~~
1.7k (10/25/16)
Update serial drivers.
Fix bug preventing flashing OTP from GUI.

~~~
1.7j (6/5/16)
Add support for XDS100v3.

~~~
1.7i (5/8/16)
Update jCAN implementations for detailed bitrate configuration.
Deprecate RTS/DTR-control.

~~~
1.7h (2/16/16)
Display bottom of firmware image when generating CRC.
Use new jSerial and jEmu libraries.

~~~
1.7g (8/30/15)
Extended timing parameter added for networks with large latencies.
Adapt Concerto block-size.

~~~
1.7f (summer solstice 2015)
Add support for 2805x family.
Fix sector selection bug.

~~~
1.7e (4/25/2015)
Add support for serial-over-CAN protocol.
Improve UDS protocol to handle more than 8 sectors.

~~~
1.7d (day after "super" PI-day)
Add support for CRC and encryption on 8-bit devices.

~~~
1.7c (Super Bowl XLIX)
Add support for 2807x and 2837xS.

~~~
1.7b (1/16/2015)
Add CRC to payload.
Fix premature timeout in UDS loader.

~~~
1.7a (1/1/2015)
Add 2837xD support.

~~~
1.6f (10/21/14)
Add firmware encryption to UDS loader.

~~~
1.6e (9/10/14)
Fix c2p.dll memory leak (v2.2).
Add forward compatibility with version 1.7.

~~~
1.6d (3/29/14)
Update version of c2p.dll (v2.1).
Add C# sample application.

~~~
T1.6c (1/9/14)
Add support for Peak CAN adapters.
CAN ID for UDS backdoor now configurable.

~~~
1.6b (12/5/13)
Add support for flash-less MCUs (such as 28345).

~~~
1.5o (10/23/13)
Update Flash API for C28 Concerto core (#315).
New version of c2p.dll (#316).

~~~
1.5n (10/5/13)
Fixe inconsistency with <loadnode> formatting (#308).

~~~
1.5m (9/28/13)
OpenCS updated to version 1.12 with NI-CAN support.
Compiled for JAVA 6.

~~~
1.5l (9/8/13)
ClosedCS updated to version 3.4i.
Now using second generation launcher framework.

~~~
1.5k Internal release

~~~
1.5j (8/23/13)
General overhaul of command-line options.
"Extract" command-line option added.
Extracted file now also includes CRC record (if CRC enabled).
"Allow OTP" setting now stored in ehx file.

~~~
1.5i (2/24/13)
With JTAG reflashing of M3 Concerto core.

~~~
1.5h (1/29/13)
Improved handling of custom emulator configurations.

~~~
1.5g (1/13/13)
CSM support added for C28 Concerto core.

~~~
1.5f (10/22/12)
Updated emulator driver (tixds28x.dvr).
Improved "Concerto" support.

~~~
1.5e (7/14/12)
Now with "Concerto" F28M35X support.

~~~
1.5d (4/1/12)
Application no longer requires a JAVA JRE.

~~~
1.5c (1/10/12)
UART-over-LIN support added.
c2pExtractHex() API call added.

~~~
1.5b (1/16/12)
Visual C sample files for DLL use provided.

~~~
1.5a (1/1/12)
DLL Interface added.

~~~
1.4i (11/17/11)
USB DFU Stellaris support added for 28069U.

~~~
1.4h (10/3/11)
Support for selection between multiple XDS100 emulators added.

~~~
1.4g (8/28/11)
Fixed bug which did not properly release emulator.

~~~
1.4f (5/8/11)
New option for code encryption and secure transmission.

~~~
1.4e (4/7/11)
First XDS100 support added.

~~~
1.4d (3/25/11)
"Bootrecords" added.

~~~
1.4c (3/21/11)
Support for Vector CAN hardware added.
GUI changes to split target name into basestring and options.

~~~
1.4b (2/19/11)
Support for 2806x added.

~~~
1.4a Preliminary C2Prog releases
Support for MSP430 added

~~~
1.3g (11/18/10)
Slight GUI modification for port configuration
Automatic check for updates added

~~~
1.3f Internal release

~~~
1.3e (10/17/10)
Support for 28021 and 28023 added.
Consolidation of jar files.

~~~
1.3d (10/2/10)
Bug with keys embedded in ehx files starting with "0" fixed.

~~~
1.3c (7/25/10)
Controls for variable SBL baudrate created.

~~~
1.3b4 (4/18/10)
Support for 28232 added.

~~~
1.3b3 (3/28/10)
Native Lawicel support added
Authentication algorithm for CAN bootloading implemented
Block-sequence counter checked in CAN/UDS download
28335-CANUDS released

~~~
1.2b2 Internal release

~~~
1.2b1 Internal release

~~~
1.3b0 (2/22/10)
Support for programming over CAN
Enhanced ehx format
Refactoring of target configuration
Improved GUI

~~~
1.2q (11/29/09)
Now allows OTP programming
Refactored target.xml format

~~~
1.2p Internal release

~~~
1.2o (11/15/09)
Support added for 2806

~~~
1.2n Internal release

~~~
1.2m (10/2/09)
Added support for 2802 and 28234
Fixed clocking of 28235 at 20 MHz and 25 MHz

~~~
1.2l Internal release

~~~
1.2k (8/20/09)
Support for "serial-break" control added
"Sector"-bug fixed
YACK() patch applied in RXTX 

~~~
1.2j Internal release

~~~
1.2i (7/23/09)
Support added for Piccolo B (F28035)
API updated for Piccolo A (F28027)

~~~
1.2h Internal release

~~~
1.2g (6/24/09)
Restructuring of programmer in support of UDS (over CAN) support.

~~~
1.2f (5/17/09)
No functional changes, but some codeskin classes have been renamed.

~~~
1.2e (3/9/09)
Now with support for TMS320F28235.

~~~
1.2d (1/8/09).
New provisions for downloading Flash API separately after CSM has been unlocked
in support of chips with limited unsecured RAM.

~~~
1.2c (9/29/08).
Erases sectors individually.
"APIXXX" in target names have been dropped.
28335 kernels have been updated to latest flash API (2.1).
2407 kernels have been modified to support individual sector erase.
Supports alternate location for targets file in user-dir/codeskin

~~~
1.2b (9/15/08)
Internal release

~~~
1.2a (7/14/08)
First 1.2 release with restructured GUI for handling of extended hex (ehx) and 
remote hex (rhx) files. Programmer now also attempts to use CSM keys embedded 
in hex file for unlocking a locked flash. Uses Swt 3.3.2.

~~~
1.1y (7/7/08)
Linked with CodeSkin 2.0c for better error handling.
Updated flasher2406_7_130_10MHz_4x.hex for smarter unlocking.

~~~
1.1x (7/2/08)
Internal release

~~~
1.1w (6/29/08).
Added new "link file" feature (remote hex)
Uses CodeSkin 2.0 with JDOM 1.1

~~~
1.1v (6/1/08).
Added new "extended hex" feature
Uses CodeSkin 1.9 with JDOM 1.1

~~~
1.1u (3/23/08).
Added '-l' command-line option for c28x bootload
Release of first manual

~~~
1.1t (3/18/08).
Fixed hex-file locking issue
Added DTR/RTS reset after successful programming
Added option for custom targets file (targets.custom.xml)

~~~
1.1s (2/28/08)
Experimental implementation of "branch after program" feature.

~~~
1.1r (2/18/08)
Inverted polarity of DTR/RTS control and fixed crash after failed programming of flash.
Also updated Janel launcher to latest version.

~~~
1.1q (2/2/08)
Internal release.

~~~
1.1p (1/17/08)
Distributed with SWT3.3. 
Bug in targets.xml regarding 281x flash sector E fixed.

~~~
1.1o (1/2/08)
Improved error handling and added options in target.xml for the tuning of the transmission timing.

~~~
1.1n (12/15/07)
Added first implementation of DTR/RTS control, based on CodeSkin core 1.7.
Now also supports TMS320F2809.

~~~
1.1i-m.
Internal releases.

~~~
1.1h (10/8/07)
CSM 28335 bug fixed.

~~~
1.1g (10/7/07)
Support for more targets added, including slow transmit for LF2407A.

~~~
1.1f (9/18/07)
Supports more targets, most notably the TMS320F28335.

~~~
1.1e (7/18/07)
Offers access to the raw 28x bootloader for loading a program into RAM.

~~~
version 1.1d (7/5/07)
Includes the TMS320F2812-48MHz target.

~~~
version 1.1c (2/18/07)
Features new icons.

~~~
version 1.1b 2/7/07)
Can be called with command-line options that Completely bypass the GUI making 
c2oooProg more suitable for batch-programming and allowing it to be called by other
programs. First version to be compatible with JRE 1.6.

~~~
version 1.1a (12/15/06)
Offers two salient new features of versions 1.0x:

- manual or automatic selection of which sectors are erased
- 32-bit CRC generation which can be used to verify Flash integrity at DSP bootup.

~~~
version 1.0d (5/19/06)
Improved error handling on communication. Eliminated possible risk of thread deadlock.

~~~
version 1.0c (4/11/06)
Identical to 1.0b with minor bug-fix.

~~~
version 1.0b (4/10/06)
Identical to 1.0a, but with additional target configurations and 
rxtx-2.1-7r2 (small bug-fix).

The following FLASH API's and chip clock frequencies are supported:
- TMS320LF2406A: API v1.30	Chip frequency: 40 MHz	PLL: 4x
- TMS320LF2407A: API v1.30	Chip frequency: 40 MHz	PLL: 4x
- TMS320LF2407A: API v1.30	Chip frequency: 40 MHz	PLL: 2x
- TMS320F2808:   API v3.02	Chip frequency: 100 MHz	PLL: 5x
- TMS320F2811:	 API v2.10	Chip frequency: 150 MHz	PLL: 5x
- TMS320F2812:	 API v2.10	Chip frequency: 150 MHz	PLL: 5x

~~~
version 1.0a (4/1/06)
Identical to 1.0, but with additional target configurations and 2811 problem
solved.

The following FLASH API's and chip clock frequencies are supported:
- TMS320LF2406A: API v1.30	Chip frequency: 40 MHz	PLL: 4x
- TMS320LF2407A: API v1.30	Chip frequency: 40 MHz	PLL: 4x
- TMS320F2808:   API v3.02	Chip frequency: 100 MHz	PLL: 5x
- TMS320F2811:	 API v2.10	Chip frequency: 150 MHz	PLL: 5x

~~~
Version 1.0 (3/26/06) - first release of the tool.
The following FLASH API's and chip clock frequencies are supported:
- TMS320LF2406A: API v1.30	Chip frequency: 40 MHz	PLL: 4x
- TMS320F2808:   API v3.02	Chip frequency: 100 MHz	PLL: 5x
- TMS320F2811:	 API v2.10	Chip frequency: 150 MHz	PLL: 5x

Warning: For the 2811, the chip-unlocking is not yet functional. The
programmer will therefore not work with a locked flash!

~~~

Feedback:
---------
info@codeskin.com

			