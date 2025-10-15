# Breakout

<img width="1274" height="714" alt="455168526-5c4b60bb-562a-4eba-832b-75c274df6de2" src="https://github.com/user-attachments/assets/5288b483-1da1-4478-ab4e-b0c27a450c0e" />

A classic **Breakout** game implementation built with C in my free time.

## Building & Running

> [!NOTE] 
> In order to use `FETCH_LIBS=OFF` you must have libraries installed of your machine.

```bash
mkdir build && cd build
cmake .. -DFETCH_LIBS=ON
make
./breakout
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

