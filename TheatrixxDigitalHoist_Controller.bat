@ECHO OFF
atprogram -t atmelice -i SWD -d atsamc20e18a chiperase
atprogram -t atmelice -i SWD -d atsamc20e18a -f program -f TheatrixxDigitalHoist_Controller.hex --verify
pause