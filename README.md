# Genshin FPS Unlocker

A lightweight tool that unlocks the FPS limit in Genshin Impact using code injection.

## Features

- Unlocks FPS cap
- Lightweight native C++ implementation
- Uses a separate DLL payload
- No external runtime dependencies required
- Built using MinGW/MSYS2

---

## Requirements

- Windows 10/11
- MSYS2 with UCRT64 environment
- MinGW g++

Install required compiler tools:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc
```

## Build Instructions

### 1. Compile Resources

```bash
C:\msys64\ucrt64\bin\windres.exe app.rc -O coff -o app.o
```

### 2. Build DLL Payload

```bash
g++ -shared -o payload.dll payload.cpp -static-libgcc -static-libstdc++
```

### 3. Build Injector Executable

```bash
g++ unlocker.cpp app.o -o Genshin_FPS_unlock.exe -lcomdlg32
```

---

## Usage

1. Extract files to a folder other than Genshin's folder.
2. Launch `Genshin_FPS_unlock.exe`.
3. It will ask you to select `GenshinImpact.exe`.

Usually located at:

```text
C:\Program Files\HoYoPlay\games\Genshin Impact game
```

4. If everything goes correctly, FPS will be uncapped.

---

## Notes

- Keep `payload.dll` in the same folder as the executable.
- Compile using the **UCRT64** environment in MSYS2.
- Antivirus software may flag DLL injection tools as suspicious.

---

## Disclaimer

This project is intended for educational and research purposes only. Use at your own risk.
