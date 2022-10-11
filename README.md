# SimpleAsyncLogger
cpp高性能异步日志库
- 实现按时间和按文件大小的日志滚动。
- 支持 C++ stream 风格输出，日志格式统一，定位到时间和具体文件行数。
- 允许用户多目的地注册，输出到终端和文件系统（文件 IO 和 MMAP 优化均实现）。
- 多线程设计，前端线程负责提交日志，后端线程负责持久化到文件系统。
- 多缓冲区优化，降低前后端线程访问共享资源时锁的粒度，实现日志微秒级输出。

### 编译
```shell
sudo ./build.sh
```

### 使用
```cpp
// 选择你需要的头文件
#include "mylogger/*.h"
```
具体可参考`example/`中的测试文件



### 执行流程图

<div align=center><img src="/pic/Loggerdescription.png" height=""/> </div>
<div align=center><img src="/pic/testdemo.png" height=""/> </div>

### References
[[1]](https://github.com/chenshuo/muduo)Event-driven network library for multi-threaded Linux server in C++11 
