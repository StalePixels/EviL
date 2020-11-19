EviL settings configuration
=

Design philosophy
-
https://en.wikipedia.org/wiki/Run_commands says
    
    In the context of Unix-like systems, the term rc stands for the phrase "run 
    commands". It is used for any file that contains startup information for a 
    command. It is believed to have originated sometime in 1965 at a runcom 
    facility from the MIT Compatible Time-Sharing System (CTSS).
    
    From Brian Kernighan and Dennis Ritchie:
        There was a facility that would execute a bunch of commands stored in a
        file; it was called runcom for "run commands", and the file began to be
        called "a runcom". rc in Unix is a fossil from that usage.
    
    Tom Van Vleck, a Multics engineer, has also reminisced about the extension
    rc:
        The idea of having the command processing shell be an ordinary slave
        program came from the Multics design, and a predecessor program on CTSS
        by Louis Pouzin called RUNCOM, the source of the '.rc' suffix on some
        Unix configuration files."
    
    This is also the origin of the name of the Plan 9 from Bell Labs shell by
    Tom Duff, the rc shell:
        It is called "rc" because the main job of a shell is to "run commands".
    
    While not historically precise, rc may also be expanded as "run control",
    because an rc file controls how a program runs. For instance, the editor
    Vim looks for and reads the contents of the .vimrc file to determine its
    initial configuration. In The Art of Unix Programming, Eric S. Raymond
    consistently refers to rc files as "run-control" files.

And this is how we end up at EviL.rc :-)

Settings can be invoked in one of two ways, and both end up at the same command processor module.

1: Programmatically - via /SYS/EviL.rc
-
Our "run control" file is stored on /SYS since NextZXOS is not a multi-user system and the setting is system-wide, unlike traditional Unix-like systems, and /HOME.
Settings changed in /SYS/EviL.RC are applied at startup automatically every time EviL is started, effectively changing the startup defaults.

An example run control file, saved in /sys/evil.rc could look as follows;

    silent = true
    font = cinema
    repeat_key = 10

For an explanation of what these settings do, see the "Individual Settings" section, below.

2: Manually - via the :set colon command
-
When in command mode (not typing in the main editor area, toggled with EDIT) you can change settings in realtime using the :set command. e.g.;

    :set font = sinclair
    :set silent = false
    :set repeat_start = 25
    
Settings applied in this manner will revert to the startup defaults (i.e. they don't change the run control file.)

Individual Settings
-

### Input Control ###
####```repeat_start```
 ```default = 25```

**repeat_start** controls how many screen refreshes[^1] EVIL will count before considering a key ready to begin repeating.

####```repeat_key``` 
 ```default = 15```

**repeat_key** controls how many screen refreshes[^1] EVIL will count before repeating a key again, after the first repeat.
### Display Control ###
####```font```
 ```default = sinclair```
 
**font** selects which font to use for the main editor layer. 

Available bundled fonts are:
 
 | Settings Name 	| Font Name            	| Font Source                 	|
 |---------------   |----------------------	|-----------------------------	|
 | sinclair      	| Sinclair System Font 	| ZX ROM                      	|
 | cinema        	| Cinema               	| zx-origin's by Damien Guard 	|
 | courier        	| Courier             	| zx-origin's by Damien Guard 	|
 | magnetic        	| Magnetic             	| zx-origin's by Damien Guard 	|
 | minecraft       	| Minecraft            	| zx-origin's by Damien Guard 	|
 | ocr-a        	| OCR-A               	| zx-origin's by Damien Guard 	|
 | ocr-b        	| OCR-B               	| zx-origin's by Damien Guard 	|
 | sixties        	| 60s               	| zx-origin's by Damien Guard 	|

When referencing an external font file make sure to use the full path name, starting with /[^2]

### EviL Control ###
####```errors```
 ```default = true```
 
**errors** disables error reporting while loading the run control file. This is useful for when sharing the same run control file between multiple editors, or using the same config files between different SD cards where fonts may not exist. 

If invoked outside of the run control file it will suppress error reporting during subsequent error messages until re-enabled. 
 
Footnotes
-
 [^1]: Intervals measures in screen refreshes will be variable dependent upon the machines video speed, not CPU speed.
 
 [^2]: While the loading of external font files is referenced in several places it's not yet complete, so the setting will warn you if you try to reference an external font file. When implemented external font files will need to be valid 768byte files in the "CH8" format
