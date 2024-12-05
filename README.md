# Cache Lab
> BUAA Software Engineering 2024 Autumn

This is the handout directory for the Cache Lab. 

## Running the autograders:
Before running the autograders, compile your code:
```bash
make
```

Check the correctness and performance of your transpose functions:
```bash
./test-trans -M 16 -N 16
./test-trans -M 32 -N 32
```


## Files:
> You will modifying and handing in these two files

| File      | Description             |
|:----------|:------------------------|
| csim.c    | Your cache simulator    |
| trans.c   | Your transpose function |

> Tools for evaluating your simulator and transpose function

| File                   | Description                                         |
|:-----------------------|:----------------------------------------------------|
| Makefile               | 		Builds the simulator and tools                    |
| README                 | 		This file                                         |
| cachelab.c             | 		Required helper functions                         |
| cachelab.h             | 		Required header file                              |
| csim-ref*              | 		The executable reference cache simulator          |
| csim-ref-mac*          | 		The executable reference cache simulator on mac   |
| csim-ref-windows.exe*	 | The executable reference cache simulator on windows |
| test-trans.c           | 		Tests your transpose function                     |
| tracegen.c             | 		Helper program used by test-trans                 |
| transreg.c             | 		Register transpose functions                      |
| traces/                | 		Trace files used to test                          |