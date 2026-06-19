# Genshin FPS Unlocker

A lightweight tool that unlocks the FPS limit in Genshin Impact using code injection.

## Features

- Unlocks FPS cap
- Lightweight native C++ implementation
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
C:\msys64\ucrt64\bin\windres.exe resource.rc -O coff -o resource.o
```

### 2. Build DLL Payload

```bash
g++ -shared -o payload.dll payload.cpp -static-libgcc -static-libstdc++
```

### 3. Build Injector Executable

```bash
g++ unlocker.cpp resource.o app.o -o Genshin_FPS_unlock.exe -lcomdlg32 -static -static-libgcc -static-libstdc++
```

---

## Usage

1. Launch `Genshin_FPS_unlock.exe`.
2. It will ask you to select `GenshinImpact.exe`.

Usually located at:

```text
C:\Program Files\HoYoPlay\games\Genshin Impact game
```

4. If everything goes correctly, FPS will be uncapped.

---

## Notes

- Compile using the **UCRT64** environment in MSYS2.
- Antivirus software may flag DLL injection tools as suspicious.
- Avoid placing Unlocker in same Directory as Genshin
---

## Disclaimer

This project is intended for educational and research purposes only. Use at your own risk.
