# Cache Lab
> BUAA Software Engineering 2024 Autumn CO Lab
>
> 北航 计算机硬件基础(软件专业) 2024秋 第三次实验
> 
> Source: [CSAPP Cache Lab](http://csapp.cs.cmu.edu/3e/labs.html)
- `main` 分支存放题面提供的初始代码
- `solution` 分支存放个人答案

## Introduction
This is the handout directory for the Cache Lab. 

### Running the autograders:
Before running the autograders, compile your code:
```shell
make
```

Check the correctness and performance of your transpose functions:
```shell
./test-trans -M 16 -N 16
./test-trans -M 32 -N 32
```


### Files:
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

## Problem 1: Cache Simulator
在本节中，我们将完成一个Cache模拟器，它通过读取memory trace（内存访问记录文件），模拟访问内存的过程，并且给出每条访问记录在Cache中的命中/缺失情况。**本实验只需要补全 `csim.c` 文件中的内容，提交评测也只需提交该文件。**

### 题目描述
我们要求实现的Cache模拟器需要满足的功能详情描述如下。

#### 1. 读入文件（memory trace）格式
我们实现的Cache模拟器将从文本文件中逐行读取内存访问记录，每一条记录代表了一次内存相关操作。为了评测方便，内存访问记录的格式与valgrind（一个强大的程序内存检测工具，后文会有介绍）的输出格式相同，具体格式如下：
```text
[space]<operation> <address>,<size>
```
- `<operation>` 表示操作类型，共分为4类
    - `I` 表示取指
    - `L` 表示取数（Load）
    - `S` 表示存数（Store）
    - `M` 表示修改（可以视为先 Load 后 Store）

- `[space]` 为空格，当 `<operation>` 为除 `I` 类型之外的类型时，会在行首保留一个空格（这一安排是为了与valgrind的格式保持一致）。
- `<address>` 为**16进制数**，表示一个64位地址（注意地址大小可能会超过32位）
- `<size>` 为10进制数，表示存/取的内存大小（单位：字节）

memory trace示例：
```text
I 0400d7d4,8
 M 0421c7f0,4
 L 04f6b868,8
 S 7ff0005c8,8
```
#### 2. Cache使用方法
```shell
./csim [-hv] -s <s> -E <E> -b <b> -t <tracefile>
```
- `-h`: 输出帮助信息（帮助信息选做，不在评测范围内）

- `-v`: 输出详细trace信息（可以选择是否输出详细信息，输出格式见后文）

- `-s <s>`: set bits （S=2^s 代表组的数量，评测保证 `s` 不超过10）

- `-E <E>`: lines bits （E 代表每组的block个数，即行数/路数，评测保证 `E` 不超过 8）

- `-b <b>`: block bits （B=2^b 代表block的大小，单位为字节，评测保证 `b` 不超过10）

- `-t <tracefile>`: 内存访问记录文件（memory trace）

我们的Cache程序需要完成以下操作：
- 从main函数的参数数组中读取程序运行参数（提示：可以借助库函数 `getopt`，注意使用该函数需要 `#include <getopt.h>`）。
- 根据读入的参数，生成对应的Cache结构。**请自行设计Cache的数据结构**，与前节理论介绍中Cache格式一致，Cache需要包含 S=2^s 组，每组包括 E 行，每行中包含标记位`tag` ，有效位`valid`，和数据块`block`，以及实现LRU策略所需要的计数信息。请注意，**本实验不涉及数据块存取的具体值**，因此不需要为数据块`block`实际去分配 B=2^b 的空间。
- 读入trace文件，每行指令相当于一次内存操作，请**按照LRU替换策略**执行对应的内存操作，并记录操作对应的Cache命中、缺失、替换情况。

  具体地，可参照如下流程运行：
  - 初始状态下有效位 `valid` 均为0，Cache为空。
  - 逐行读入内存操作，提取其中的指令类型和地址等信息。对于`S`类型（Store）/ `L`类型（Load）的指令，分别对应一次存/取操作，应当对应更新Cache信息。对于`M`类型（Modify）类型的指令，可以视为一次L操作后跟一次S操作。而对于`I`类型的指令，由于本实验不考察取指过程的内存访问情况，因此**在分析指令时可以忽略`I`类型的指令。**
  - 对于涉及访存的指令，获取内存地址 `address` 对应的组（set），在Cache中查询该组的所有行（line）中是否有 `valid` 为1且 `tag` 匹配的行，如有则更新计数器返回并**记录命中信息（hit）**。
  - 如果没有命中，则需要**记录缺失信息（miss）**，并查询当前组内是否有 `valid` 为0的行，如有，则更新对应行的信息，并将其valid置1，同时更新计数器。
  - 如果没有命中且没有 `valid` 为0的行（即均为有效行），则需要按照计数器信息执行LRU替换策略，将最近最少使用的行替换为当前操作的地址块信息，并 **记录替换信息（eviction）** 同时更新计数器。
  - 依次执行所有读入文件中的操作指令，完成整个Cache运行模拟。

- 评测保证操作内存大小 `<size>` 不超过块（block）大小 B=2^b ，即一次内存操作最多涉及一个块的替换，因此无需考虑生成的valgrind数据中因 `<size>` 过大导致的跨block操作。
- 完成运行后将需要的输出结果输出到控制台中，具体输出的内容及其格式见下。

#### 3. 输出格式
1. 基本输出格式
    ```text
    hits:<hits> misses:<misses> evictions:<evictions>
    ```
    其中 `hits`，`misses`，`evictions` 分别表示Cache的命中数，缺失数和替换数。可参见 `cachelab.c` 中的 `printSummary` 函数，只需在主程序运行结束时调用该函数，即可完成基本的结果输出。

2. 输出详细trace信息

    如果命令参数中有 `-v`，那么需要**在基本输出之前，输出对应的详细trace信息**，具体输出格式如下（请注意空格与以下示例保持一致）。
    ```text
    <operation> <address>,<size> [hit] [miss] [eviction]
    ```
    其中 `<operation>` 表示操作类型，`<address>`表示地址，`<size>`表示大小（具体含义参见对读入文件格式的描述），该部分输出与每行读入的信息保持一致即可（**注意：输出 `<address>` 和 `<size>` 时去掉前导0**）。

    对于描述信息，`hit` 表示命中，`miss` 表示缺失，`eviction` 表示替换，当出现对应情况的时候就需要输出对应字符串。请注意，描述信息的输出需要按顺序判断，即应先判断是输出 `hit` 还是 `miss`，再判断是否输出 `eviction`（注意：`I` 类型指令该部分分析可忽略，只需原样输出 `<operation>`，`<address>` 和 `<size>`）。

    特别地，对于M类型指令，应当先输出Load对应的 `hit`，`miss`，`eviction` 情况，再输出Store对应的 `hit`，`miss`，`eviction` 情况，即对应以下的格式：
    ```text
    <operation> <address>,<size> [hit] [miss] [eviction] [hit] [miss] [eviction]
    ```
    比如Load时未命中且发生替换，那么应当先输出 `miss`，`eviction`，之后Store命中再输出`hit`。

在评测时，我们将对同学们实现的Cache读入内存访问记录，并将输出与标准输出进行比对，**通过所有测试点即可通过本节实验（5分）**。

### 输入输出样例
内存访问记录文件 `yi.trace`
```text
L 10,1
M 20,1
L 22,1
S 18,1
L 110,1
L 210,1
M 12,1
```
输入命令
```shell
./csim -v -s 4 -E 1 -b 4 -t traces/yi.trace
```

输出结果
```text
L 10,1 miss
M 20,1 miss hit
L 22,1 hit
S 18,1 hit
L 110,1 miss eviction
L 210,1 miss eviction
M 12,1 miss eviction hit
hits:4 misses:5 evictions:3
```

## Problem 2: Matrix Transpose Optimization
在本节中，我们将给出特定大小的矩阵，请同学们应用分块等优化思想，利用我们在实验一中实现的Cache模拟器，检验不同的分块策略对Cache命中/缺失情况的影响，并通过逐步优化使得miss（即缺失数）尽可能低。**本实验只需要补全 `trans.c` 中的内容，提交评测也只需提交该文件。**
### 题目描述
给定Cache结构为：s=4,E=1,b=5 。

考察trans.c中的矩阵转置函数 `transpose_submit`，该函数要实现矩阵转置的功能，将N行M列矩阵A转置为M行N列的矩阵B。本节实验包含两个题目，我们分别给出矩阵的M和N，并要求根据Cache的结构和矩阵的结构，计算出一个比较好的矩阵转置方案，使得miss数量达到要求。

在完成转置功能以外，代码还需满足以下要求：
- 转置函数使用局部变量类型只能为 `int`，且限制局部变量个数不超过12个（建议将局部变量声明写在函数初始位置，参看代码注释）。
- 如果在 `transpose_submit` 之外自己定义了其他的辅助函数，那么需要保证所有函数的局部变量个数之和不得超过12个。
- 不得在函数中使用数组或者尝试用malloc申请内存。
- 不得修改矩阵A中的值，矩阵B中的值可以随意修改，只需满足最终的检验正确性。
- 转置函数或其他辅助函数一律不得使用递归。

以上的代码要求会在评测后进行统一检查，**如不满足以上要求则视为未通过本实验。**

本节的评测逻辑较为复杂，我们将根据实现的矩阵转置函数 `transpose_submit`，模拟生成该函数执行过程中对应的memory trace，并通过参考Cache程序 `csim-ref` 来模拟内存访问的情况，最终通过Cache得出miss数量，**miss数量达到要求即可通过本节实验。**

具体分值设置如下：
#### 题目一：16 x 16 矩阵转置优化（5分）
|  等级  |   miss情况   | 得分 |    预期判定结果     |
|:----:|:----------:|:--:|:-------------:|
| 正确验证 |   转置结果无误   | 	1 | 	Wrong Answer |
| 基础要求 | miss < 200 | 	3 | 	Wrong Answer |
| 通过要求 | miss < 100 | 	5 |   	Accepted   |
| 优秀挑战 | miss < 75  | 	5 |  	Excellent   |

#### 题目二：32 x 32 矩阵转置优化（5分）
|  等级  |   miss情况   | 得分 |    预期判定结果     |
|:----:|:----------:|:--:|:-------------:|
| 正确验证 |   转置结果无误   | 	1 | 	Wrong Answer |
| 基础要求 | miss < 600 | 	3 | 	Wrong Answer |
| 通过要求 | miss < 400 | 	5 |   	Accepted   |
| 优秀挑战 | miss < 300 | 	5 |  	Excellent   |

### 输入输出样例
输入命令
```shell
./test-trans -M 16 -N 16
```
输出结果
```text
Function 0 (1 total)
Step 1: Validating and generating memory traces
Step 2: Evaluating performance (s=4, E=1, b=5)
...

Summary for official submission (func 0): correctness=1 misses=67

TEST_TRANS_RESULTS=1:67
```
得到 `TEST_TRANS_RESULTS=1:67`，即miss为67，达到通过要求。



- **注意，未使用的变量请不要声明，和上一题一样，会导致编译错误！**