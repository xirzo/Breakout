# Breakout

<img width="1274" height="714" alt="455168526-5c4b60bb-562a-4eba-832b-75c274df6de2" src="https://github.com/user-attachments/assets/5288b483-1da1-4478-ab4e-b0c27a450c0e" />

A classic **Breakout** game cross-platform implementation built with C in my free time. Tested on **Windows 11** using *MSVC* compiler and **Linux**.

## Building & Running

> [!NOTE] 
> In order to use `FETCH_LIBS=OFF` you must have libraries installed of your machine.

### Linux

```bash
mkdir build && cd build
cmake -DFETCH_LIBS=true -B build
cmake --build build
./build/breakout
```

### Windows

```powershell
cmake -DFETCH_LIBS=true -B build -A x64
cmake --build build
cd .\build\Debug
.\breakout.exe
```

## Controls

| Key | Action |
|-----|--------|
| `A` or `J` | Move paddle left |
| `D` or `K` | Move paddle right |
| `ESC` | Exit game |
| `SPACE` | Reset ball position (Debug mode) |

## Dependencies

- **Raylib** - Graphics and input handling
- **Box2D** - Physics simulation engine

