Mac VERGE Readme - Initial Release
----------------

Mac VERGE is the VERGE3 engine running on Mac OS X. It has been tested primarily on 10.3 and 10.4.

Mac VERGE acts just like PC VERGE. Just place the verge3 application in the same folder as whatever game you want to play and open it. It will compile the .vc source and run the game as usual. 

Mac VERGE uses the SDL, fmod and Corona libraries. Please see "About verge3" for more information.

What works:
-----------
- Can compile and run VERGE3 games
- Keyboard, Mouse, Joysticks
- fullscreen (command-M to switch), window resizing
- URL grabbing functions
- Mac<->Mac socket connections

What doesn't work:
------------------
- Loading PC-compiled VC code (You will get a version error if you try -- fix this by turning off release mode in verge.cfg and compiling from the source)
- Movies
- Clipboard functions
- Multiple windows
- Automax
- Mac<->PC socket connections

What needs doing:
-----------------
- Testing, especially input, networking, and file functions
- The OpenGL scaling code needs to be faster
- Adding stuff that doesn't work

For more information
--------------------
Please see the VERGE website at http://www.verge-rpg.com/
