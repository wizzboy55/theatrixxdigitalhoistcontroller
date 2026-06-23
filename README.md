# TheatrixxDigitalHoistController
Theatrixx Technologies Digital Hoist Controller

## Prerequisites

- Windows machine (the provided scripts are `.bat` files).
- Atmel-ICE programmer connected over SWD.
- SAM C20 target device (`atsamc20e18a`).
- `atprogram` CLI installed (included with Microchip/Atmel Studio).

## Set Up `atprogram`

1. Install Microchip Studio (or Atmel Studio) so `atprogram.exe` is available.
2. Locate `atprogram.exe` (commonly under an `atbackend` folder), for example:
   - `C:\Program Files\Microchip\Studio\7.0\atbackend`
   - `C:\Program Files (x86)\Atmel\Studio\7.0\atbackend`
3. Add that folder to your Windows `Path` environment variable.
4. Open a new Command Prompt and verify:

```bat
atprogram --version
```

If the command is recognized, setup is complete.

## Using the `.bat` Files

Run these from the repository root in Command Prompt:

- `TheatrixxDigitalHoist_Controller.bat`
  - Erases chip, programs `TheatrixxDigitalHoist_Controller.hex`, then verifies.
- `TheatrixxDigitalHoist_Remote.bat`
  - Erases chip, programs `TheatrixxDigitalHoist_Remote.hex`, then verifies.

Example:

```bat
TheatrixxDigitalHoist_Controller.bat
```

Both scripts pause at the end so you can review success/failure output.
