# 论文阅读：自动化通用服务器程序的实时更新（热更新/动态更新？）

[https://www.cs.vu.nl/~giuffrida/papers/tse-2016.pdf](https://www.cs.vu.nl/~giuffrida/papers/tse-2016.pdf)

摘要——过去几十年中，不断有人研究如何在不停止运行程序的情况下部署软件更新的紧迫需求，这促进了许多关于实时更新系统的研究。然而，以前的解决方案要么对更新的性质做出强烈的假设，要么需要大量而容易出错的手动努力，这些因素都阻碍了实时更新的采用。
本文介绍了Mutable Checkpoint-Restart (MCR)，这是一种新的实时更新解决方案，适用于用C语言编写的通用（多进程和多线程）服务器程序。与以前的解决方案相比，MCR可以支持任意软件更新并自动化大部分常见的实时更新操作。关键思想是允许运行版本安全地达到静止状态，然后允许新版本以尽可能类似于新程序初始化的方式重新启动，依靠现有的代码路径自动恢复旧的程序线程并重新初始化相关部分的程序数据结构。为了传输其余的数据结构，MCR依靠精确和保守的垃圾回收技术的组合来跟踪所有全局指针，并在运行时应用所需的状态转换。流行的服务器程序（Apache httpd、nginx、OpenSSH和vsftpd）的实验结果证实，我们的技术可以有效地自动化先前被认为困难的问题，而成本只是可忽略的性能开销（平均2%）和适度的内存开销（平均3.9倍，没有优化）。

总体而言，我们做出了以下贡献：

- 我们提出了基于配置文件的静止技术，该技术允许所有程序线程使用离线分析阶段收集的专用信息自动且安全地进入已知静止状态。
- 我们提出了可变重初始化技术，该技术在不同的程序版本之间记录并重放启动操作，并利用现有的代码路径自动重新初始化新的程序版本、其线程和相关的全局数据结构。
- 我们提出了可变跟踪技术，该技术使用精确（可能时）和保守（否则）的GC样式跟踪策略在版本之间转移剩余的数据结构。
- 我们在C中提出了可变检查点重启（MCR）的新的现场更新解决方案的有效性。我们在Linux上进行了实现，并在4个流行的服务器程序上进行了评估，结果显示MCR具有以下优点：（i）支持甚至复杂更新的低工程成本（总共334个注释LOC），（ii）现实的更新时间（< 1 s）；（iii）默认配置下的可忽略性能开销（平均2％）；（iv）适度的内存开销（平均3.9倍，没有优化）。

本文的剩余部分组织如下。第2节介绍了现有的现场更新领域的先前工作和技术。第3节提供了所提出的解决方案的高级概述。第4、5和6节详细描述了构成MCR方法的三个主要技术：（i）基于配置文件的静止技术，（ii）可变重初始化技术和（iii）可变跟踪技术。第7节提供了相关实现细节的深入讨论，而第9节则介绍了验证所提出解决方案的实验结果。最后，第10节总结了本文的发现。