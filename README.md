# Thirsty

"You must be thirsty."  --- &lt;少年PI的奇幻漂流> 


## Feature

这是一个从Facebook的[folly](https://github.com/facebook/folly)里提取的几个基础C++组件。

我更改了代码使其能在Visual C++ 2013下编译，在核心API不变的情况下跑过所有的单元测试。


包含：
* [Scoped Guard](http://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Scope_Guard)
* [Benchmark](https://github.com/facebook/folly/blob/master/folly/docs/Benchmark.md)
* [Conv](https://github.com/facebook/folly/blob/master/folly/docs/Conv.md)
* [Format](https://github.com/facebook/folly/blob/master/folly/docs/Format.md)


## Installation

下载构建工具[premake5](http://premake.github.io/download.html).

* Windows 7 x64，双击`msvc2013.bat`生成Visual C++工程文件
* Ubuntu 14.04 x64，在Terminal里执行`premake5 gmake && make config=release`
