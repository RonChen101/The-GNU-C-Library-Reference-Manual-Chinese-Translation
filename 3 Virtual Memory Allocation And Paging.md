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

#### 3.2.3.3 可移植的内存分配

当在即要在GNU和非GNU系统上运行的代码中分配内存时，或使用非GNU的替代分配器时（参考[Replacing malloc](https://sourceware.org/glibc/manual/latest/html_node/Replacing-malloc.html)），需要更加小心。当存储区小或者奇怪的大，或存储区被分配成一个不常见的类型，POSIX和ISO C标准允许多种行为。

- 在大多数系统中，一个成功的`malloc` `(` `0` `)`会返回一个非空指针到一个新分配的大小为零的块。然而，IBM AIX是不常见的，一个成功的`malloc` `(` `0` `)`会返回一个空指针，并且这会破坏常见代码，例如这个手册中给出的`xmalloc`实现。参考[Examples of `malloc`](https://sourceware.org/glibc/manual/latest/html_node/Malloc-Examples.html)。想要移植到IBM AIX的代码可以使用`p` `=` `malloc` `(` `size` `|` `(` `size` `==` `0` `)` `)`替代`p` `=` `malloc` `(` `size` `)`，或者如果在成功时他不介意一个空指针，它可以用`if` `(` `p` `==` `NULL` `&&` `size` `!=` `0` `)` `fatal` `(` `...` `)` `;`替换下面的`if` `(` `p` `==` `NULL` `)` `fatal` `(` `...` `)` `;`。

- 在the GNU C Library中，一个失败的`malloc`调用会设置`errno`，但是ISO C不要求这个，并且非POSIX实现在失败时不需要设置`errno`。

- 在the GNU C Library中，`malloc`在*size*超过`PTRDIFF_MAX`时总是失败，为了避免程序减指针或使用符号索引时发生问题。其他实现可能在这种情况下成功，然后导致未定义行为。

- 在the GNU C Library中，`malloc` `(` `size` `)`返回一个指针，当转换成整数时是`alignof` `(` `max_align_t` `)`的倍数。一些其他的实现可能只会以包含的基础对齐对象中最大的那个大小需求对齐，`max` `(` *`size`* `,` `1` `)`。例如，如果`alignof` `(` `max_align_t` `)`是16，但是较小的基础对齐对象都有不超过4的对齐，那么其他`malloc` `(` `15` `)`实现可能会返回一个是4的倍数的指针，而不是16或8。可移植的代码因此应该使用类似`aligned_alloc`的函数，如果他甚至需要小分配也以`alignof` `(` `max_align_t` `)`对齐。

- 虽然the GNU C Library的头文件只写了具有基础对齐方式的类型，C和POSIX标准仅要求下面的类型（如果可以）具有基础对齐。

<div style="margin: 0 0 1em 4em;">

`char`；所有的整数类型（包括`bool`）；`float`，`double`，`long` `double`；`_Decimal32`，`_Decimal64`，`_Decimal128`；`float` `_Complex`，`double` `_Complex`，`long` `double` `_Complex`；所有枚举类型；所有指针类型；所有元素类型具有基础对齐需求的数组；所有成员类型具有基础对齐需求且没有严格对齐标识符的结构体和联合体；`va_list`（在`<` `stdarg.h` `>`中）；`fpos_t`（在`<` `stdio.h` `>`中）；`cnd_t`，`thrd_t`，`tss_t`，`mtx_t`，`once_flag`（在`<` `threads.h` `>`中）；`mbstate_t`（在`<` `wchar.h` `>`中）。
</div>


<div style="margin: 0 0 1em 2em;">

理论上，可移植代码不应该使用`malloc`分配包含不在上述列表的类型的存储区；应该用`aligned_alloc`类似的函数代替。实践中，然而，其他实现通常跟随the GNU C Library的领导，并且只定义了具有基础对齐方式的类型，并且通常使用`malloc`分配具有the C library定义的类型的对象是可移植的。
</div>

#### 3.2.3.4 释放`malloc`分配的内存

当你不需要从`malloc`获取的块时，使用`free`让块能够重新分配。函数原型在`stdlib.h`中。

函数：`void` **`free`** `(` `void` `*` *`ptr`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe | AS-Unsafe lock | AC-Unsafe lock fd mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

`free`函数释放*ptr*指向的内存块。
</div>

释放块会改变块的内容。<strong>在释放后，不要想在块中找到任何数据（例如链表中指向下一个块的指针）。</strong>在释放前，复制你所有需要的！这里有一个例子，是释放链中的所有块，以及他们指向的字符串的正确方式：

<div style="margin: 0 0 1em 2em;">

```c
struct chain
  {
    struct chain *next;
    char *name;
  }

void
free_chain (struct chain *chain)
{
  while (chain != 0)
    {
      struct chain *next = chain->next;
      free (chain->name);
      free (chain);
      chain = next;
    }
}
```
</div>

按照传统，`free`可以将内存返回给系统，并且使进程更小。但是通常，他所有能做到的就是允许`malloc`回收利用那片空间。同时，那片空间仍保留在你的程序中，作为一个空闲表给内部的`malloc`使用。

`free`函数会保留`errno`的值，所以清理代码的调用`free`部分周围无需担心保存和恢复`errno`。尽管ISO C和POSIX.1-2017都不要求`free`保留`errno`的值，未来POSIX计划要求他。

在程序的结尾释放块是没有意义的，因为当程序结束时，所有程序空间都返回给系统了。

函数：`void` **`free_sized`** `(` `void` `*` *`ptr`* `,` `size_t` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe | AS-Unsafe lock | AC-Unsafe lock fd mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

`free_sized`函数会释放*ptr*指向的先前由`malloc`，`calloc`，`realloc`分配的内存块。大小*size*必须对应先前提供给`malloc`，`calloc`，`realloc`的需求大小。尝试释放`aligned_alloc`，`memalign`，`posix_memalign`，`valloc`，`pvalloc`分配的内存是未定义行为。对于`aligned_alloc`，`memalign`，`posix_memalign`，请使用`free_aligned_sized`替代。此外，对于并非调用者直接分配内存而且还需要释放的，例如`strdup`或`strndup`的结果，调用`free_sized`也是未定义行为。对于这些情况，请继续使用`free`代替。
</div>

函数：`void` **`free_aligned_sized`** `(` `void` `*` *`ptr`* `,` `size_t` *`alignment`* `,` `size_t` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe | AS-Unsafe lock | AC-Unsafe lock fd mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

`free_aligned_sized`函数会释放*ptr*指向的先前由`aligned_alloc`，`memalign`，`posix_memalign`分配的内存块。大小*size*和对齐方式*alignment*必须对应先前提供给`aligned_alloc`，`memalign`，`posix_memalign`的需求大小和对齐方式。
</div>

#### 3.2.3.5 改变块的大小

当你开始使用块时，你经常不知道你最终需要具体多大的块。例如，你有一个块作为缓冲区，用来保存从一个文件读取的一行；无论你如何初始化那个缓冲区，你都可能遇到行更长的情况。

你可以调用`reaaloc`或`reallocarray`使块更长。这些函数在`stdlib.h`中声明。

函数：`void` `*` **`realloc`** `(` `void` `*` *`ptr`* `,` `size_t` *`newsize`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe | AS-Unsafe lock | AC-Unsafe lock fd mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

`realloc`函数改变以*ptr*为地址的块的大小成*newsize*。
</div>

<div style="margin: 0 0 1em 2em;">

因为块后面的空间可能在使用，`realloc`可能会复制块到有更多空间的新地址。`realloc`的值是块的新地址。如果块需要被移动，`realloc`复制旧的内容。
</div>

<div style="margin: 0 0 1em 2em;">

如果你*ptr*传了一个空指针，`realloc`的行为就会和‘`malloc` `(` *`newsize`* `)`’一样。另外，如果*newsize*为零，`realloc`会释放块，然后返回`NULL`。另外，如果`realloc`不能重新分配需求的大小，他会返回`NULL`并且设置`errno`；原块不会改变。
</div>

<div style="margin: 0 0 1em 2em;">

任何由`realloc`返回的非空指针都和相同大小下`malloc`返回的指针类似，满足相同的对齐限制。任何由`aligned_alloc`和类似函数创建的特殊对齐都会在调用`realloc`后丢失。
</div>

函数：`void` `*` **`reallocarray`** `(` `void` `*` *`ptr`* `,` `size_t` *`nmemb`* `,` `size_t` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe | AS-Unsafe lock | AC-Unsafe lock fd mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

`reallocarray`函数改变以*ptr*为地址的块的大小成足以装下*nmemb*个元素的数组，每个元素大小为*size*。他等价于‘`realloc` `(` *`ptr`* `,` *`nmemb`* `*` *`size`* `)`’，区别是，如果乘法溢出，`reallocarray`会安全的失败，通过设置`errno`为`ENOMEM`，返回一个空指针，不改变原块。
</div>

<div style="margin: 0 0 1em 2em;">

当块被分配的新大小，即乘法的结果可能会溢出时，`reallocarray`应该替代`realloc`。
</div>

<div style="margin: 0 0 1em 2em;">

在the GNU C Library中，数组对齐的方式和`malloc`对齐他返回的值一样。在其他实现，对于由基础对齐的元素，每个元素大小最大为`max` `(` *`size`* `,` `1` `)`，组成的数组，他可能仅仅适当对齐。任何由`aligned_alloc`和类似函数创建的特殊对齐都会在调用`reallocarray`后丢失。
</div>

<div style="margin: 0 0 1em 2em;">

该函数最初源自OpenBSD 5.6，但已加入POSIX.1-2024。
</div>

和`malloc`一样，如果空间不足以使块更大，`realloc`和`reallocarray`可能会返回一个空指针。当这发生了，原块不会被碰；他从未被修改或重定位。

在大多数情况下，当`realloc`失败时，原块不会有任何改变，因为当没有内存时，应用程序不会继续，唯一会做的是给出一个致命错误信息。写和使用子进程通常是方便的，传统上称为`xrealloc`和`xreallocarray`，关心错误信息，就像`xmalloc`为`malloc`做的一样：

<div style="margin: 0 0 1em 2em;">

第一个函数的返回值应该是`p`。
```c
void *
xreallocarray (void *ptr, size_t nmemb, size_t size)
{
  void *p = reallocarray (ptr, nmemb, size);
  if (p == NULL)
    fatal ("Virtual memory exhausted");
  return value;
}

void *
xrealloc (void *ptr, size_t size)
{
  return xreallocarray (ptr, 1, size);
}
```
</div>

你也可以使用`realloc`或`reallocarray`来让一个块更小。你这样做的原因是，避免只需要一点空间时，占用大量内存空间。在多种分配实现中，让一个块更小有时需要复制他，所以如果没有其他可用空间时，他可能会失败。

<strong>可移植性声明：</strong>

- 可移植的程序不应该尝试重新分配块为大小零。在其他实现中，如果*ptr*非空，`realloc` `(` `ptr` `,` `0` `)`可能会释放块并返回一个非空指针指向一个大小零的对象，或他可能会失败并返回`NULL`，并且不释放块。ISO C17标准允许这些变体。

- 在the GNU C Library中，如果结果块的大小超过了`PTRDIFF_MAX`，重分配失败，这是为了避免程序进行指针减法或有符号索引时出问题。其他实现可能成功，导致未定义行为。

- 在the GNU C Library中，如果新块大小和旧的一样，`realloc`和`reallocarray`保证不会改变任何事，并且返回你给的相同的地址。然而，POSIX和ISO C允许程序重定位对象或在这种情况失败。

#### 3.2.3.6 分配干净的内存

`calloc`函数分配内存并且将他清理为零。他在`stdlib.h`中声明。

函数：`void` `*` **`calloc`** `(` `size_t` *`count`* `,` `size_t` *`eltsize`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe | AS-Unsafe lock | AC-Unsafe lock fd mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

该函数分配一个清零的数组。他和`reallocarray` `(` `NULL` `,` *`count`* `,` *`eltsize`* `)`的行为类似，除了在`calloc`返回前，数组内容会被清理为零。
</div>

你可以这样定义`calloc`：

<div style="margin: 0 0 1em 2em;">

```c
void *
calloc (size_t count, size_t eltsize)
{
  void *p = reallocarray (0, count, eltsize);
  if (p != NULL)
    memset (p, 0, count * eltsize);
  return p;
}
```
</div>

但是一般来说，并不保证`calloc`内部会调用`reallocarray`和`memset`。例如，如果`calloc`实现知道一些其他原因导致新内存块就是零，他可能不再用`memset`再次清零那个块。又，如果应用程序从the C library之外提供他自己的`reallocarray`，`calloc`可能不会使用那个重定义。参考[Replacing malloc](https://sourceware.org/glibc/manual/latest/html_node/Replacing-malloc.html)。

#### 3.2.3.7 分配对齐的内存块

在GNU系统中，`malloc`或`realloc`返回的块的地址总是八（或者，64位系统上，十六）的倍数。如果你需要一个比那个更高的，二的幂的倍数的块，使用`aligned_alloc`或`posix_memalign`。`stdlib.h`声明了`aligned_alloc`和`posix_memalign`。

函数：`void` `*` **`aligned_alloc`** `(` `size_t` *`alignment`* `,` `size_t` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe | AS-Unsafe lock | AC-Unsafe lock fd mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

`aligned_alloc`函数分配一个*size*字节大小的块，地址是*alignment*的倍数。*alignment*必须是二的幂。
</div>

<div style="margin: 0 0 1em 2em;">

发生错误时，`aligned_alloc`返回一个空指针，并且设置`errno`为下面的值之一：
</div>

<div style="margin: 0 0 1em 2em;">

`ENOMEM`
</div>

<div style="margin: 0 0 1em 4em;">

无法满足请求，可用内存不足。
</div>

<div style="margin: 0 0 1em 2em;">

`EINVAL`
</div>

<div style="margin: 0 0 1em 4em;">

*alignment*不是二的幂。
</div>

<div style="margin: 0 0 1em 2em;">

此程序在ISO C11中引入，因此对于现代非POSIX系统，移植性可能会比*posix_memalign*更好。（原文的缩进是两个，我感觉只应该只有一个。）
</div>

函数：`void` `*` `memalign` `(` `size_t` `boundary` `,` `size_t` `size` `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe | AS-Unsafe lock | AC-Unsafe lock fd mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

The `memalign` function allocates a block of *size* bytes whose address is a multiple of *boundary*. The *boundary* must be a power of two! The function *memalign* works by allocating a somewhat larger block, and then returning an address within the block that is on the specified boundary.

</div>