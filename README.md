# chip8
A C++ Chip-8 Interpreter that provides graphical output via SDL and emulates the 35 standard opcodes. Graphical rendering works but many opcodes are currently buggy, making many games unplayable. When input is fixed, it will be mapped directly via the keyboard (key-for-key rather than using a QWE-ASD-ZXC or another type of layout). If you want to learn more about the platform, read this [Wikipedia](https://en.wikipedia.org/wiki/CHIP-8) article. 

![MAZE Running](MAZE.png?raw=true "MAZE")

# Building
Clone the repository and open it using Visual Studio 2022. The SDL library should work out of the box. Build the chip8-emu solution and it will output in the x64 folder.

# Acknowledgements

- James Griffen's Chip-8 Emulator for SDL Rendering and Some Opcodes 
- Cowgod's Documentation
- Emulator101
