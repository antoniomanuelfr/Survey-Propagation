# Survey Propagation
This project is a C++ implementation of the Survey Propagation algorithm, proposed on this paper:
 https://arxiv.org/pdf/cs/0212002.pdf

## Structure
1. `cnf`: Folder that storage DIMACS files. 
2. `inc`: Headers. 
3. `src`: Source code. 
4. `doc`: Doxygen generator file. 
## Usage
- Create folder for the build system in the root folder. <br />
```mkdir build && cd build```
- Execute CMake <br />
  ```cmake ..```
- Run make <br />
  `make`

## Todo list 
1. [X] Improve comments. 
2. [X] ReadDIMACS: Ignore comments that might be in the middle of the file: <p>Commit <a href="https://github.com/antoniomanuelfr/Survey-Propagation/commit/05e2b6ae459a25f6824fc8eccd17264eb44da99f" rel="nofollow">05e2b6ae</a></p>
3. [X] Make functions to print the FactorGraph data structure. <p>Commit <a href="https://github.com/antoniomanuelfr/Survey-Propagation/commit/05e2b6ae459a25f6824fc8eccd17264eb44da99f" rel="nofollow">05e2b6ae</a></p>
4. [ ] Partial assignment function. 
5. [X] Make the SP-update function. <p>Commit <a href="https://github.com/antoniomanuelfr/Survey-Propagation/commit/f3b4200da100bbc5a34f3217fe8e1516d3d5a2e5" rel="nofollow">f3b4200d</a></p>
