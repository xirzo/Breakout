# 🧱 Breakout Game 

A classic **Breakout** game implementation built with C.

## 🏓 Current State

![image](https://github.com/user-attachments/assets/5c4b60bb-562a-4eba-832b-75c274df6de2)


## 🕹️ Controls

| Key | Action |
|-----|--------|
| `A` or `J` | Move paddle left ⬅️ |
| `D` or `K` | Move paddle right ➡️ |
| `ESC` | Exit game 🚪 |
| `SPACE` | Reset ball position (Debug mode) 🔄 |

## 🛠️ Dependencies

- **Raylib** - Graphics and input handling 🎨
- **Box2D** - Physics simulation engine ⚡

## 🚀 Building & Running

```bash
# Make sure you have Raylib and Box2D installed
mkdir build && cd build
cmake ../.. -DFETCH_LIBS=ON
make
./breakout
```
