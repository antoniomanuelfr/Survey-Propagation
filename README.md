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
1. [] Improve comments. 
2. [] ReadDIMACS: Ignore comments that might be in the middle of the file
3. [] Make functions to print the FactorGraph data structure
4. [] Make the SurveyPropagation function
