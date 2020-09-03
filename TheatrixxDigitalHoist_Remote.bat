@ECHO OFF
atprogram -t atmelice -i SWD -d atsamc20e18a chiperase
atprogram -t atmelice -i SWD -d atsamc20e18a -f program -f TheatrixxDigitalHoist_Remote.hex --verify
pause