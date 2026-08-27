这是The GNU C Library Reference Manual，版本2.44。

Copyright © 1993–2026 Free Software Foundation, Inc.

遵守the terms of the GNU Free Documentation License, Version 1.3 or any later version published by the Free Software Foundation时，以下内容是许可的，复制，分发，修改这个文档；不能修改“Free Software Needs Free Documentation” and “GNU Lesser General Public License”章节，封面文字必须是“A GNU Manual”，封底文字必须是下面(a)的内容。本许可证的副本包含在题为“GNU Free Documentation License”的章节中。

(a) The FSF’s Back-Cover Text is: “You have the freedom to copy and modify this GNU manual. Buying copies from the FSF supports it in developing GNU and promoting software freedom.”

# 3 虚拟内存分配与分页

此章节描述了在使用the GNU C Library的系统中，进程如何管理和使用内存。

The GNU C Library有多个函数通过多种的方式实现动态分配虚拟内存。他们有些通用性更好，有些效率更高。库还提供一些函数，他们可以控制分页和分配物理内存。

此章节不讨论内存映射I/O。参考[Memory-mapped I/O](https://sourceware.org/glibc/manual/latest/html_node/Memory_002dmapped-I_002fO.html)

- [Process Memory Concepts](https://sourceware.org/glibc/manual/latest/html_node/Memory-Concepts.html)

- [Allocating Storage For Program Data](https://sourceware.org/glibc/manual/latest/html_node/Memory-Allocation.html)

- [Resizing the Data Segment](https://sourceware.org/glibc/manual/latest/html_node/Resizing-the-Data-Segment.html)

- [Memory Protection](https://sourceware.org/glibc/manual/latest/html_node/Memory-Protection.html)

- [Locking Pages](https://sourceware.org/glibc/manual/latest/html_node/Locking-Pages.html)

## 3.1 进程内存概念

进程可用的最基本的资源之一是内存。系统有非常多不同的方式组织内存，但是在经典系统中，每个进程都有一个线性虚拟地址空间，地址从零到一个很大的最大值。他不需要是连续的；也就是说，不是所有地址都能用来存储数据。

虚拟内存被分成了页（通常为4KB）。在背后支持每一页虚拟内存的，是一页物理内存（页框）或者某些二级存储，通常是磁盘空间。磁盘空间可能是交换空间，或某些普通磁盘文件。事实上，一页全零的内存有时背后没有任何存储支持——仅需要一个标志位代表他是全零。

同一个物理内存的页框或后背存储可以支持多个进程的虚拟内存。这是通常情况，比如说，GNU C Library代码占用的虚拟内存。包含`printf`函数的相同的物理内存页框支持每一个调用`printf`的真实存在的进程的虚拟内存。

为了让一个程序访问到虚拟页的任意部分，页在那时必须被物理页框支持（连接）。但是因为虚拟内存经常比物理内存多很多，页必须在物理内存和后备存储间来回移动，当进程需要他们时，到物理内存中，当不需要时，回到后备存储。这个移动叫做*paging*。

当以个程序尝试访问一个当时不在物理内存支持的页时，这就是*page* *fault*。当页错误发生时，内核会中断进程，	再把页放进物理页框中（这称为“页进”或“错进”）最后再继续进程，这样的话，在进程的视角来看，页总是在物理内存中的。事实上，对于进程，所有页总是看出去在物理内存中。除了一种情况：在一条指令的过去的执行时间，一般来说，时间只需要几纳秒，但是突然变的非常长（因为内核一般需要I/O来完成页进）。关于对延迟敏感的程序，[Locking Pages](https://sourceware.org/glibc/manual/latest/html_node/Locking-Pages.html)中描述的函数可以控制他。

在每个虚拟地址空间中，一个进程需要追踪每个地址中有什么，这个进程称为内存分配。分配通常让人联想到稀缺资源的配给，但是在虚拟内存中，这不是主要目标，因为他的量通常超过任何人的需求。进程中的内存分配主要目的是确保同一个字节的内存没有用来存储两个不同的东西。

进程分配内存有两种主要方式：exec系列函数和程序显式分配。事实上，分叉是第三种方式，但是他不值得关注。参考[Creating a Process](https://sourceware.org/glibc/manual/latest/html_node/Creating-a-Process.html)。

exec是为进程创建虚拟地址空间的操作，将基础程序装载进去，然后执行该程序。这由“exec”系列函数（比如`execl`）。这个操作需要一个程序文件（一个可执行的），他分配空间，以装载可执行文件中的所有数据，然后装载他，然后将控制权交给他。那些数据中，程序的指令（*text*）是最主要的，但是也包含程序中的字面量，常量，甚至变量：即具有静态存储类型的C变量（参考[Memory Allocation in C Programs](https://sourceware.org/glibc/manual/latest/html_node/Memory-Allocation-and-C.html)）。

一旦程序开始执行，他会使用程序显式的分配以获得额外的内存。使用the GNU C Library的C程序中，有两种程序显式分配：自动和动态。参考[Memory Allocation in C Programs](https://sourceware.org/glibc/manual/latest/html_node/Memory-Allocation-and-C.html)。

内存映射I/O是动态虚拟内存分配的另一种形式。映射内存到一个文件（也可能指的是物理设备）意味着声明进程的某一段地址空间的内容应该和指定的普通文件的内容完全一致。系统让虚拟内存初始的包含文件的内容，并且如果你修改内存，系统会写入相同的修改到文件。注意，由于虚拟内存和页错的奇妙机制，直到程序访问该虚拟内存之前，系统没有理由I/O读取文件，或者为他的内容分配内存。参考[Memory-mapped I/O](https://sourceware.org/glibc/manual/latest/html_node/Memory_002dmapped-I_002fO.html)。

正如他程序显式的分配内存，程序可以程序显式的释放（*free*）他。你不能释放由exec分配的内存。当程序退出或exec（再次执行），你或许会说他的所有内存被释放了，但其实在这两种情况下，地址空间都不存在了，所以这个说法没有意义。参考[Program Termination](https://sourceware.org/glibc/manual/latest/html_node/Program-Termination.html)。

一个进程的虚拟地址空间被划分为段。一个段是一个连续的虚拟地址段。有三个重要的段：

- *text* *segment*包含程序的指令，字面量，静态常量。他被exec分配，并且在整个虚拟地址空间的生命中大小都保存不变。

- *data* *segment*是程序的工作存储区。他可以被预分配，被exec预装载，并且进程可以通过调用相关函数来扩展和缩小他，参考[Resizing the Data Segment](https://sourceware.org/glibc/manual/latest/html_node/Resizing-the-Data-Segment.html)。他的低端是固定的。

- *stack* *segment*包含程序的栈区。当栈增长时，他也会增长，但是栈缩小时，他不会缩小。

## 3.2 为程序数据分配存储区

此章节包含普通程序如何管理他们数据的存储区，包括著名的`malloc`函数和一些the GNU C Library和GNU编译器特有的更花哨的功能。

- [Memory Allocation in C Programs](https://sourceware.org/glibc/manual/latest/html_node/Memory-Allocation-and-C.html)

- [The GNU Allocator](https://sourceware.org/glibc/manual/latest/html_node/The-GNU-Allocator.html)

- [Unconstrained Allocation](https://sourceware.org/glibc/manual/latest/html_node/Unconstrained-Allocation.html)

- [Allocation Debugging](https://sourceware.org/glibc/manual/latest/html_node/Allocation-Debugging.html)

- [Replacing malloc](https://sourceware.org/glibc/manual/latest/html_node/Replacing-malloc.html)

- [Obstacks](https://sourceware.org/glibc/manual/latest/html_node/Obstacks.html)

- [Automatic Storage with Variable Size](https://sourceware.org/glibc/manual/latest/html_node/Variable-Size-Automatic.html)