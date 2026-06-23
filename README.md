# TheatrixxDigitalHoistController
Theatrixx Technologies Digital Hoist Controller

## Prerequisites

- Windows machine (the provided scripts are `.bat` files).
- Atmel-ICE programmer connected over SWD.
- SAM C20 target device (`atsamc20e18a`).
- `atprogram` CLI installed (included with Microchip/Atmel Studio).

## Set Up `atprogram`

1. Install Microchip Studio (or Atmel Studio) so `atprogram.exe` is available.
2. Run the helper script from the repository root:

```bat
Setup_atprogram_Path.bat
```

This script searches common Microchip/Atmel Studio install folders for `atprogram.exe` and adds its directory to your user `Path`.

3. Open a new Command Prompt and verify:

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
