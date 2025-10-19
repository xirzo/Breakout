# Breakout

<img width="1274" height="714" alt="455168526-5c4b60bb-562a-4eba-832b-75c274df6de2" src="https://github.com/user-attachments/assets/5288b483-1da1-4478-ab4e-b0c27a450c0e" />

A classic **Breakout** game cross-platform implementation built with C in my free time. Tested on **Windows 11** using *MSVC* compiler and **Linux**.

## Building & Running

> [!NOTE] 
> In order to use `FETCH_LIBS=OFF` you must have libraries installed of your machine.
>
> Also if you want to run debug mode, turn it on via `cmake -B build/ -DCMAKE_BUILD_TYPE=Debug` overwise  `cmake -B build/ -DCMAKE_BUILD_TYPE=Release`

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

## Configuration

Configuration is stored in **toml** file, if it is invalid or not set, default configuration is used. 
To use custom configurtion provide it via _first CLI argument_:

```sh
./build/breakout ./example_configuration.toml
```

### Example

```toml
[game]
bricks_in_row = 8

[player]
width = 100
height = 15
movement_speed = 400.0

[ball]
radius = 8.0
min_speed_multiplier = 0.8
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

