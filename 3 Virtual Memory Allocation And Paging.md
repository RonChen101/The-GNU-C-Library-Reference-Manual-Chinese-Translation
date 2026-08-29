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

虚拟内存被分成了页（page）（通常为4KB）。在背后支持每一页虚拟内存的，是一页物理内存（称为页框（*frame*））或者某些二级存储，通常是磁盘空间。磁盘空间可能是交换空间，或某些普通磁盘文件。事实上，一页全零的内存有时背后没有任何存储支持——仅需要一个标志位代表他是全零。

同一个物理内存的页框（frame）或后背存储可以支持多个进程的虚拟内存页（page）。这是通常情况，比如说，GNU C Library代码占用的虚拟内存。包含`printf`函数的相同的物理内存页框（frame）支持每一个调用`printf`的真实存在的进程的虚拟内存页（page）。

为了让一个程序访问到虚拟页（page）的任意部分，页（page）在那时必须被物理页框（frame）支持（连接）。但是因为虚拟内存经常比物理内存多很多，页（page）必须在物理内存和后备存储间来回移动，当进程需要他们时，到物理内存中，当不需要时，回到后备存储。这个移动叫做*paging*。

当以个程序尝试访问一个当时不在物理内存支持的页（page）时，这就是*page* *fault*。当页错（page fault）误发生时，内核会中断进程，再把页（page）放进一页物理页框（frame）中（这称为“页进（paging in）”或“错进（faulting in）”），最后再继续进程，这样的话，在进程的视角来看，页（page）总是在物理内存中的。事实上，对于进程，所有页（page）总是看出去在物理内存中。除了一种情况：在一条指令的过去的执行时间，一般来说，时间只需要几纳秒，但是突然变的非常长（因为内核一般需要I/O来完成页进）。关于对延迟敏感的程序，[Locking Pages](https://sourceware.org/glibc/manual/latest/html_node/Locking-Pages.html)中描述的函数可以控制他。

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

### 3.2.1 C程序的内存分配

C语言支持两种通过C程序的变量来分配内存的方式：

- *Static* *allocation*在声明静态或全局变量时使用。每个静态和全局变量定义一块固定大小的空间。空间分配一次，当你的程序启动时（exec操作的一部分），并且从不释放。

- *Automatic* *allocation*在声明自动变量时使用，比如函数参数或局部变量。自动变量的空间在程序进入到包含声明的复合语句部分时分配，并且在复合语句退出时释放（花括号结束时）。

<div style="margin: 0 0 1em 2em;">

在GNU C中，自动存储区的大小是一个可变的表达式。在其他C实现中，他必须是一个常量。
</div>

第三个重要的内存分配方式，动态分配，不是由C变量支持，但是通过GNU C Library函数支持。

- [Dynamic Memory Allocation](https://sourceware.org/glibc/manual/latest/html_node/Memory-Allocation-and-C.html#Dynamic-Memory-Allocation)

#### 3.2.1.1 动态内存分配

*Dynamic* *memory* *allocation*是一个技术，程序在运行时可以决定把信息存储在哪里。当你需要定量内存，或你需要定时内存，取决于在程序启动前未知的因素，则你需要动态分配。

例如，你可能需要1个块来保存从输入文件读取到的一行；因为一行的长度是没有限制的，你必须动态的分配内存，以确保你读取行的过程中内存能动态的变大。

或者，你可能需要为输入数据的每条记录或每条定义分配一个块；因为你不能提前知道有多少条，你必须在读取过程中为每条记录或定义分配块。

当你使用动态分配，分配一块内存是程序显式请求的行动。当你需要分配空间时，你调用一个函数或宏，然后通过参数指定大小。如果你想要释放空间，你可以调用另一个函数或宏。你可以在任何时候做些事，以任何频率。

C变量不支持动态分配；没有存储类型“dynamic”，并且永远不会有C变量存储在动态分配的空间。获取动态分配内存的唯一方式是通过系统调用（一般是通过GNU C Library函数调用），并且引用动态内存空间的唯一方式是通过一个指针。因为他不方便，并且内存分配的实际过程需要更多计算时间，所以一般只有静态内存和自动分配不够用时，程序员才用动态分配。

例如，如果你想为保存`struct` `foobar`分配内存，你不能声明一个内容在动态分配空间中的`struct` `foobar`类型变量。但是你可以声明一个`struct` `foobar` `*`指针类型的变量，然后给他分配那片空间的地址。然后，你可以在该指针变量用‘`*`’和‘`->`’操作符来引用那片空间的内容：

<div style="margin: 0 0 1em 2em;">

```c
{
  struct foobar *ptr = malloc (sizeof *ptr);
  ptr->name = x;
  ptr->next = current_foobar;
  current_foobar = ptr;
}
```
</div>

### 3.2.2 The GNU内存分配器

the GNU C Library中的`malloc`实现源自ptmalloc（pthreads malloc），ptmalloc（pthreads malloc）又源自dlmalloc（Doug Lea malloc）。此`malloc`根据他们的大小和用户控制的某些参数，可能会以两种不同的方式分配内存。最普通的方式是从一大片连续的内存区域中分配一部分内存（称为chunk），且管理这部分区域来优化他们的使用，且减少无用的chunk形式的浪费。传统上，系统的堆区是一个大的内存区域，但是the GNU C Library `malloc`实现会维护多个这样的区域来优化他们在多线程应用中的使用。每一个这样的区域在内部称为*arena*。

与其他版本不同，the GNU C Library中的`malloc`不会向上取整到二的幂，无论大小。相邻的chunk可能会因`free`合并，无论他们的大小。这让实现在没有通过碎片遭受大量内存浪费的情况下，适应所有类型的分配模式。多个arena的存在允许多个线程在不同的arena中同时分配内存，因此提高了性能。

另一种内存分配方式是给超大块的，例如，比一页（page）还大（通常为4KB）。这些请求是通过`mmap`分配的（匿名的或通过`/` `dev` `/` `zero`；参考[Memory-mapped I/O](https://sourceware.org/glibc/manual/latest/html_node/Memory_002dmapped-I_002fO.html)）。这有相当大的好处，这些chunk被释放后会瞬间返回系统。因此，不会发生一个大chunk被小chunk锁定，即使在调用`free`后仍然浪费内存的情况。使用`mmap`的大小门槛是动态的，由程序的分配模式调整。`mallopt`可以使用`M_MMAP_THRESHOLD`静态调整门槛，并且`M_MMAP_MAX`可以完全警用`mmap`；参考[Malloc Tunable Parameters](https://sourceware.org/glibc/manual/latest/html_node/Malloc-Tunable-Parameters.html)。

the GNU内存分配器 更多细节技术描述在the GNU C Library wiki上保存。参考[https://sourceware.org/glibc/wiki/MallocInternals]。

你可以使用你自己客制化的`malloc`替换the GNU C Library提供的内置分配器。参考[Replacing malloc](https://sourceware.org/glibc/manual/latest/html_node/Replacing-malloc.html)。

### 3.2.3 不受限制的内存分配器

最普遍的动态分配功能是`malloc`。他允许你在任何时间分配任意大小的内存块，让他们变大或变小，并且分别释放那些块（或从不释放）。

- [Basic Memory Allocation](https://sourceware.org/glibc/manual/latest/html_node/Basic-Allocation.html)

- [Examples of `malloc`](https://sourceware.org/glibc/manual/latest/html_node/Malloc-Examples.html)

- [Portable Memory Allocation](https://sourceware.org/glibc/manual/latest/html_node/Portable-Allocation.html)

- [Freeing Memory Allocated with `malloc`](https://sourceware.org/glibc/manual/latest/html_node/Freeing-after-Malloc.html)

- [Changing the Size of a Block](https://sourceware.org/glibc/manual/latest/html_node/Changing-Block-Size.html)

- [Allocating Cleared Space](https://sourceware.org/glibc/manual/latest/html_node/Allocating-Cleared-Space.html)

- [Allocating Aligned Memory Blocks](https://sourceware.org/glibc/manual/latest/html_node/Aligned-Memory-Blocks.html)

- [Malloc Tunable Parameters](https://sourceware.org/glibc/manual/latest/html_node/Malloc-Tunable-Parameters.html)

- [Heap Consistency Checking](https://sourceware.org/glibc/manual/latest/html_node/Heap-Consistency-Checking.html)

- [Statistics for Memory Allocation with `malloc`](https://sourceware.org/glibc/manual/latest/html_node/Statistics-of-Malloc.html)

- [Summary of `malloc`-Related Functions](https://sourceware.org/glibc/manual/latest/html_node/Summary-of-Malloc.html)

#### 3.2.3.1 基础内存分配

分配一块内存，调用`malloc`。此函数原型在`stdlib.h`中。

函数：`void` `*` **`malloc`** `(` `size_t` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe | AS-Unsafe lock | AC-Unsafe lock fd mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

This function returns a pointer to a newly allocated block *size* bytes long, or a null pointer (setting `errno`) if the block could not be allocated.此函数返回一个指向新分配的*size*字节长的块的指针，或者一个空指针（设置`errno`），如果块无法分配的话。
</div>

块的内容是未定义的；你必须手动初始化他（或者使用`calloc`代替；参考[Allocating Cleared Space](https://sourceware.org/glibc/manual/latest/html_node/Allocating-Cleared-Space.html)）。通常，你会将该值转换为指向你想要在块中存储的对象类型的指针。这里我们展示了一个做这事的例子，然后使用库函数`memset`以零来初始化那片空间（参考[Copying Strings and Arrays](https://sourceware.org/glibc/manual/latest/html_node/Copying-Strings-and-Arrays.html)）：

<div style="margin: 0 0 1em 2em;">

```c
struct foo *ptr = malloc (sizeof *ptr);
if (ptr == 0) abort ();
memset (ptr, 0, sizeof (struct foo));
```
</div>

你可以将`malloc`的结果保存为任意指针变量，无需转换，因为ISO C在必要时，会自动的将`void` `*`转换成另一种类型。然而，如果上下文未指定类型但又需要该类型时，则必须使用强制类型转换。

记住，当给字符串分配空间时，传入`malloc`的参数必须是一加字符串的长度。这是因为字符串是以一个空字符截止的，那个空字符不算字符串的长度，但是需要空间。例如：

<div style="margin: 0 0 1em 2em;">

```c
char *ptr = malloc (length + 1);
```
</div>

参考[Representation of Strings](https://sourceware.org/glibc/manual/latest/html_node/Representation-of-Strings.html)，以获取更多信息。

#### 3.2.3.2 `malloc`例子

如果空间不足，`malloc`返回一个空指针。你每次调用`malloc`，都应该检测值。写一个子程序来调用`malloc`并在值为空指针是报告错误，仅当值为非空是返回，是有用的。此函数一般称为`xmalloc`。这就是：

<div style="margin: 0 0 1em 2em;">

```c
void *
xmalloc (size_t size)
{
  void *p = malloc (size);
  if (p == NULL)
    fatal ("virtual memory exhausted");
  return p;
}
```
</div>

这是一个使用`malloc`（以`xmaaloc`）的实际例子。`savestring`将会复制一系列字符到一个新分配的空截止（null-terminated）的字符串：

<div style="margin: 0 0 1em 2em;">

```c
char *
savestring (const char *ptr, size_t len)
{
  char *p = xmalloc (len + 1);
  p[len] = '\0';
  return memcpy (p, ptr, len);
}
```
</div>

在当前的the GNU C Library版本，`malloc`给你的块是对齐的，所以他的地址是`alignof` `(` `max_align_t` `)`整数倍，所以他能容纳带有任意基础对齐和无更严格对齐说明符的对象类型。只有少数情况下，更高的边界（例如页（page）边界）是需要的；在这类情况下，使用`aligned_alloc`或`posix_memalign`（参考[Allocating Aligned Memory Blocks](https://sourceware.org/glibc/manual/latest/html_node/Aligned-Memory-Blocks.html)）。在未来的the GNU C Library版本，可能会放松对小内存分配的对齐，只要任何具有基础对齐要求且能放入该小内存分配中的对象能够被正确对齐即可。例如，一个未来的`malloc` `(` `1` `)`可能会返回一个具有奇数对齐的指针。

（AI生成：大多数CPU要求多字节数据按地址的倍数对齐访问：

|data type		|需要的对齐|
|-				|-|
|char(1Byte)	||
|short(2Byte)	|地址必须是2的倍数|
|int(4Byte)		|地址必须是4的倍数|
|double(8Byte)	|地址必须是8的倍数|
|pointer(8Byte)	|地址必须是8的倍数|

）

注意，块结尾后面的内存可能用来存其他东西；可能是另一个调用`malloc`分配的块。如果你尝试以你原本请求的大小更长的大小对待块，你很可能会破坏`malloc`用于管理其内存块的数据结构，或者破坏其他块的内容。如果你已经分配一个块，然后发现你需要他变大，使用`realloc`（参考[Changing the Size of a Block](https://sourceware.org/glibc/manual/latest/html_node/Changing-Block-Size.html)）。

### 3.2.3.3 可移植的内存分配

当在即要在GNU和非GNU系统上运行的代码中分配内存时，或使用非GNU的替代分配器时（参考[Replacing malloc](https://sourceware.org/glibc/manual/latest/html_node/Replacing-malloc.html)），需要更加小心。当存储区小或者奇怪的大，或存储区被分配成一个不常见的类型，POSIX和ISO C标准允许多种行为。

- 在大多数系统中，一个成功的`malloc` `(` `0` `)`会返回一个非空指针到一个新分配的大小为零的块。然而，IBM AIX是不常见的，一个成功的`malloc` `(` `0` `)`会返回一个空指针，并且这会破坏常见代码，例如这个手册中给出的`xmalloc`实现。参考[Examples of `malloc`](https://sourceware.org/glibc/manual/latest/html_node/Malloc-Examples.html)。想要移植到IBM AIX的代码可以使用`p` `=` `malloc` `(` `size` `|` `(` `size` `==` `0` `)` `)`替代`p` `=` `malloc` `(` `size` `)`，或者如果在成功时他不介意一个空指针，它可以用`if` `(` `p` `==` `NULL` `&&` `size` `!=` `0` `)` `fatal` `(` `...` `)` `;`替换下面的`if` `(` `p` `==` `NULL` `)` `fatal` `(` `...` `)` `;`。

- 在the GNU C Library中，一个失败的`malloc`调用会设置`errno`，但是ISO C不要求这个，并且非POSIX实现在失败时不需要设置`errno`。

- 在the GNU C Library中，`malloc`在*size*超过`PTRDIFF_MAX`时总是失败，为了避免程序减指针或使用符号索引时发生问题。其他实现可能在这种情况下成功，然后导致未定义行为。

- In the GNU C Library, `malloc` `(` *`size`* `)` returns a pointer that when converted to an integer is a multiple of `alignof` `(` `max_align_t` `)`. Some other implementations may align the result only to what is needed for fundamentally-aligned objects of size at most `max` `(` *`size`* `,` `1` `)`. For example, if `alignof` `(` `max_align_t` `)` is 16 but smaller fundamentally-aligned objects all have alignment of at most 4, other implementations of `malloc` `(` `15` `)` might return a pointer that is a multiple of 4 but not of 16 or even of 8. Portable code should therefore use a function like `aligned_alloc` if it needs `alignof` `(` `max_align_t` `)` alignment even for small allocations.在the GNU C Library中，`malloc` `(` `size` `)`返回一个指针，当转换成整数时是`alignof` `(` `max_align_t` `)`的倍数。一些其他的实现可能只会以包含的基础对齐对象中最大的那个大小需求对齐，`max` `(` *`size`* `,` `1` `)`。例如，如果`alignof` `(` `max_align_t` `)`是16，但是较小的基础对齐对象都有不超过4的对齐，那么其他`malloc` `(` `15` `)`实现可能会返回一个是4的倍数的指针，而不是16或8。