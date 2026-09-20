这是The GNU C Library Reference Manual，版本2.44。

Copyright © 1993–2026 Free Software Foundation, Inc.

遵守the terms of the GNU Free Documentation License, Version 1.3 or any later version published by the Free Software Foundation时，以下内容是许可的，复制，分发，修改这个文档；不能修改“Free Software Needs Free Documentation” and “GNU Lesser General Public License”章节，封面文字必须是“A GNU Manual”，封底文字必须是下面(a)的内容。本许可证的副本包含在题为“GNU Free Documentation License”的章节中。

(a) The FSF’s Back-Cover Text is: “You have the freedom to copy and modify this GNU manual. Buying copies from the FSF supports it in developing GNU and promoting software freedom.”

---

# 3 虚拟内存分配与分页

此章节描述了在使用the GNU C Library的系统中，进程如何管理和使用内存。

The GNU C Library有多个函数通过多种的方式实现动态分配虚拟内存。他们有些通用性更好，有些效率更高。库还提供一些函数，他们可以控制分页和分配物理内存。

此章节不讨论内存映射I/O。参考[Memory-mapped I/O](https://sourceware.org/glibc/manual/latest/html_node/Memory_002dmapped-I_002fO.html)

- [Process Memory Concepts](https://sourceware.org/glibc/manual/latest/html_node/Memory-Concepts.html)

- [Allocating Storage For Program Data](https://sourceware.org/glibc/manual/latest/html_node/Memory-Allocation.html)

- [Resizing the Data Segment](https://sourceware.org/glibc/manual/latest/html_node/Resizing-the-Data-Segment.html)

- [Memory Protection](https://sourceware.org/glibc/manual/latest/html_node/Memory-Protection.html)

- [Locking Pages](https://sourceware.org/glibc/manual/latest/html_node/Locking-Pages.html)

---

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

---

## 3.2 为程序数据分配存储区

此章节包含普通程序如何管理他们数据的存储区，包括著名的`malloc`函数和一些the GNU C Library和GNU编译器特有的更花哨的功能。

- [Memory Allocation in C Programs](https://sourceware.org/glibc/manual/latest/html_node/Memory-Allocation-and-C.html)

- [The GNU Allocator](https://sourceware.org/glibc/manual/latest/html_node/The-GNU-Allocator.html)

- [Unconstrained Allocation](https://sourceware.org/glibc/manual/latest/html_node/Unconstrained-Allocation.html)

- [Allocation Debugging](https://sourceware.org/glibc/manual/latest/html_node/Allocation-Debugging.html)

- [Replacing malloc](https://sourceware.org/glibc/manual/latest/html_node/Replacing-malloc.html)

- [Obstacks](https://sourceware.org/glibc/manual/latest/html_node/Obstacks.html)

- [Automatic Storage with Variable Size](https://sourceware.org/glibc/manual/latest/html_node/Variable-Size-Automatic.html)

---

### 3.2.1 C程序的内存分配

C语言支持两种通过C程序的变量来分配内存的方式：

- *Static* *allocation*在声明静态或全局变量时使用。每个静态和全局变量定义一块固定大小的空间。空间分配一次，当你的程序启动时（exec操作的一部分），并且从不释放。

- *Automatic* *allocation*在声明自动变量时使用，比如函数参数或局部变量。自动变量的空间在程序进入到包含声明的复合语句部分时分配，并且在复合语句退出时释放（花括号结束时）。

<div style="margin: 0 0 1em 2em;">

在GNU C中，自动存储区的大小是一个可变的表达式。在其他C实现中，他必须是一个常量。
</div>

第三个重要的内存分配方式，动态分配，不是由C变量支持，但是通过GNU C Library函数支持。

- [Dynamic Memory Allocation](https://sourceware.org/glibc/manual/latest/html_node/Memory-Allocation-and-C.html#Dynamic-Memory-Allocation)

---

#### 3.2.1.1 动态内存分配

*Dynamic* *memory* *allocation*是一个技术，程序在运行时可以决定把信息存储在哪里。当你需要定量内存，或你需要定时内存，取决于在程序启动前未知的因素，则你需要动态分配。

例如，你可能需要一个块来保存从输入文件读取到的一行；因为一行的长度是没有限制的，你必须动态的分配内存，以确保你读取行的过程中内存能动态的变大。

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

---

### 3.2.2 The GNU内存分配器

the GNU C Library中的`malloc`实现源自ptmalloc（pthreads malloc），ptmalloc（pthreads malloc）又源自dlmalloc（Doug Lea malloc）。此`malloc`根据他们的大小和用户控制的某些参数，可能会以两种不同的方式分配内存。最普通的方式是从一大片连续的内存区域中分配一部分内存（称为chunk），且管理这部分区域来优化他们的使用，且减少无用的chunk形式的浪费。传统上，系统的堆区是一个大的内存区域，但是the GNU C Library `malloc`实现会维护多个这样的区域来优化他们在多线程应用中的使用。每一个这样的区域在内部称为*arena*。

与其他版本不同，the GNU C Library中的`malloc`不会让chunk向上取整到二的幂，无论大小。相邻的chunk可能会因`free`合并，无论他们的大小。这让实现在没有通过碎片遭受大量内存浪费的情况下，适应所有类型的分配模式。多个arena的存在允许多个线程在不同的arena中同时分配内存，因此提高了性能。

另一种内存分配方式是给超大块的，例如，比一页（page）还大（通常为4KB）。这些请求是通过`mmap`分配的（匿名的或通过`/` `dev` `/` `zero`；参考[Memory-mapped I/O](https://sourceware.org/glibc/manual/latest/html_node/Memory_002dmapped-I_002fO.html)）。这有相当大的好处，这些chunk被释放后会瞬间返回系统。因此，不会发生一个大chunk被小的锁定，即使在调用`free`后仍然浪费内存的情况。使用`mmap`的大小门槛是动态的，由程序的分配模式调整。`mallopt`可以使用`M_MMAP_THRESHOLD`静态调整门槛，并且`M_MMAP_MAX`可以完全警用`mmap`；参考[Malloc Tunable Parameters](https://sourceware.org/glibc/manual/latest/html_node/Malloc-Tunable-Parameters.html)。

the GNU内存分配器 更多细节技术描述在the GNU C Library wiki上保存。参考[https://sourceware.org/glibc/wiki/MallocInternals]。

你可以使用你自己客制化的`malloc`替换the GNU C Library提供的内置分配器。参考[Replacing malloc](https://sourceware.org/glibc/manual/latest/html_node/Replacing-malloc.html)。

---

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

---

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

---

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

---

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

---

#### 3.2.3.4 释放`malloc`分配的内存

当你不需要从`malloc`获取的块时，使用`free`让块能够重新分配。函数原型在`stdlib.h`中。

函数：`void` **`free`** `(` `void` `*` *`ptr`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe | AS-Unsafe lock | AC-Unsafe lock fd mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

`free`函数释放*ptr*指向的内存块。
</div>

释放块会改变块的内容。<strong>在释放后，不要想在块中找到任何数据（例如链表中指向下一个块的指针）。</strong>在释放前，从块中复制你所有需要的！这里有一个例子，是释放链中的所有块，以及他们指向的字符串的正确方式：

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

---

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

- 在the GNU C Library中，如果新大小和旧的一样，`realloc`和`reallocarray`保证不会改变任何事，并且返回你给的相同的地址。然而，POSIX和ISO C允许程序重定位对象或在这种情况失败。

---

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

---

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

函数：`void` `*` **`memalign`** `(` `size_t` *`boundary`* `,` `size_t` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe | AS-Unsafe lock | AC-Unsafe lock fd mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

`memalign`函数分配一个*size*字节大小，地址为*boundary*的倍数的块。*boundary*必须为二的幂！`memalign`函数运作原理是分配一个稍大的块，然后返回一个该块中位于特定边界的地址。
</div>

<div style="margin: 0 0 1em 2em;">

发生错误时，`memalign`返回一个空指针，并且设置`errno`为下面的值之一：
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

`memalign`函数已过时，请使用`aligned_alloc`或`posix_memalign`代替。
</div>

函数：`int` **`posix_memalign`** `(` `void` `**` *`memptr`* `,` `size_t` *`alignment`* `,` `size_t` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe | AS-Unsafe lock | AC-Unsafe lock fd mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

`posix_memalign`函数与`memalign`函数类似，它们都会返回一个按*alignment*的倍数对齐的，大小为*size*字节的缓冲区。但它对参数*alignment*增加了一个限制要求：该值必须是二的幂，且是`sizeof` `(` `void` `*` `)`的倍数。
</div>

<div style="margin: 0 0 1em 2em;">

如果函数成功分配内存，指向分配的内存的指针通过`*` `memptr`返回，并且返回值（注意，返回值指的是返回值）为零。此外，函数返回错误值来指出错误。可能的返回的错误值是：
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

*alignment*不满足，二的幂，且是`sizeof` `(` `void` `*` `)`的倍数。
</div>

<div style="margin: 0 0 1em 2em;">

此函数在POSIX 1003.1d中引进。虽然此函数被`aligned_alloc`取代，但是他在不支持ISO C11的老POSIX系统上可移植性更强。
</div>

函数：`void` `*` **`valloc`** `(` `size_t` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Unsafe init | AS-Unsafe init lock | AC-Unsafe init lock fd mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

使用`valloc`像使用`memalign`，并且传入页面大小作为第一个参数。他是这样实现的：
</div>

<div style="margin: 0 0 1em 4em;">

```c
void *
valloc (size_t size)
{
  return memalign (getpagesize (), size);
}
```
</div>

<div style="margin: 0 0 1em 2em;">

[How to get information about the memory subsystem?](https://sourceware.org/glibc/manual/latest/html_node/Query-Memory-Parameters.html)有关内存子系统的更多信息。
</div>

<div style="margin: 0 0 1em 2em;">

`valloc`函数过时了，使用`aligned_alloc`或`posix_memalign`代替。
</div>

你可以使用`memalignment`函数查明一个指针的对齐方式。

函数：`size_t` **`memalignment`** `(` `void` `*` *`p`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe | AS-Safe | AC-Safe |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

此函数，在C23中定义，返回*p*的对齐方式，作为二的幂。如果*p*是一个空指针，他返回零。C23要求*p*是指向一个对象的可用指针或一个空指针；作为一个GNU扩展，the GNU C Library支持该函数用于指针类型的任意位模式。
</div>

<div style="margin: 0 0 1em 2em;">

此函数被加入到C23标准，以支持那些指针的低位和对齐方式无关的非传统平台。对于传统平台，你可以瞬间将指针转换成`uintptr_t`，然后测试低位：这个可以移植到C23前，并且通常更快一点。
</div>

<div style="margin: 0 0 1em 2em;">

例如，如果你像读取一个被可能未对齐的指针`p`取地址的`int`，下面的C23前的代码在所有传统平台上可用：
</div>

<div style="margin: 0 0 1em 4em;">

```c
int i;
if (((uintptr_t) p & (alignof (int) - 1)) != 0)
  memcpy (&i, p, sizeof i);
else
  i = *p;
```
</div>

<div style="margin: 0 0 1em 2em;">

然而，在非传统平台上，他可能不生效，你需要下面的C23代码：
</div>

<div style="margin: 0 0 1em 4em;">

```c
int i;
if (memalignment (p) < alignof (int))
  memcpy (&i, p, sizeof i);
else
  i = *p;
```
</div>

<div style="margin: 0 0 1em 2em;">

然而，对于这种特定的情况，如果对齐和未对齐的指针使用不同的代码，性能也不会提升，因此更好的：
</div>

<div style="margin: 0 0 1em 4em;">

```c
int i;
memcpy (&i, p, sizeof i);
```
</div>

<div style="margin: 0 0 1em 2em;">

编译器会生成当前架构下，效率最高的方式来访问未对齐的数据，将`memcpy`优化掉。（AI生成：他只是通过计算*p*的二进制末尾有多少个零来得到该地址的最大对齐度。如果地址刚好落在更大粒度的边界上（例如Cache Line或Page边界），它就会返回更大的对齐值。）
</div>

---

#### 3.2.3.8 Malloc可调参数

关于动态内存分配，你可以通过`mallopt`函数，调节一些参数。此函数是通用SVID/XPG接口，在`malloc.h`中定义。

函数：`int` **`mallopt`** `(` `int` *`param`* `,` `int` *`value`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Unsafe init const:mallopt | AS-Unsafe init lock | AC-Unsafe init lock |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

当调用`mallopt`时，*param*参数（argument）指定需要设置的参数（parameter），*value*指定需要设置的值。关于*param*可能的选项，就在`malloc.h`中定义，如下：
</div>

<div style="margin: 0 0 1em 2em;">

`M_MMAP_MAX`
</div>

<div style="margin: 0 0 1em 4em;">

使用`mmap`可分配的chunk的最大数量。设置为零，可以禁用`mmap`。
</div>

<div style="margin: 0 0 1em 4em;">

此参数（parameter）的默认值为`65536`。
</div>

<div style="margin: 0 0 1em 4em;">

此参数（parameter）也可以，在进程启动时，通过设置环境变量`MALLOC_MMAP_MAX_`为想要的值来设置。
</div>

<div style="margin: 0 0 1em 2em;">

`M_MMAP_THRESHOLD`
</div>

<div style="margin: 0 0 1em 4em;">

所有比此值更大的chunk，通过使用系统调用`mmap`分配到常规堆之外。这种方式保证了这些chunk的内存在调用`free`时可以返回系统。注意，比此值更小的请求可能还是会通过`mmap`被分配。
</div>

<div style="margin: 0 0 1em 4em;">

如果此参数（parameter）没有设置，默认值为128KiB，并且门槛会动态调整，以适应程序的分配模式。如果此参数（parameter），动态调整被禁用，并且此值被静态的设置成输入值。
</div>

<div style="margin: 0 0 1em 4em;">

此参数（parameter）也可以，在进程启动时，通过设置环境变量`MALLOC_MMAP_THRESHOLD_`为想要的值来设置。
</div>

<div style="margin: 0 0 1em 2em;">

`M_PERTURB`
</div>

<div style="margin: 0 0 1em 4em;">

如果非零，当内存被分配（除了用`calloc`分配）或释放时，内存块的值会根据此参数（parameter）的某些低位来填充，这可以用来调试未初始化或已释放的堆内存的使用。注意，此选项不保证释放的块会为特定值。他只保证该块在被释放之前的内容会被覆盖。
</div>

<div style="margin: 0 0 1em 4em;">

此参数（parameter）的默认值为`0`。
</div>

<div style="margin: 0 0 1em 4em;">

此参数（parameter）也可以，在进程启动时，通过设置环境变量`MALLOC_PERTURB_`为想要的值来设置。
</div>

<div style="margin: 0 0 1em 2em;">

`M_TOP_PAD`
</div>

<div style="margin: 0 0 1em 4em;">

此参数（parameter）决定了当一个arena需要扩展时，从系统中获得的额外内存量是多少。同时，当缩小一个arena时，他也指定了需要保留的字节数。这在堆大小方面提供了必要的滞后性，从而过多的系统调用可以被避免。
</div>

<div style="margin: 0 0 1em 4em;">

此参数（parameter）的默认值为`0`。
</div>

<div style="margin: 0 0 1em 4em;">

此参数（parameter）也可以，在进程启动时，通过设置环境变量`MALLOC_TOP_PAD_`为想要的值来设置。
</div>

<div style="margin: 0 0 1em 2em;">

`M_TRIM_THRESHOLD`
</div>

<div style="margin: 0 0 1em 4em;">

这是为了返回内存给系统，会触发系统调用的，位于最顶部的，可释放的chunk的最小大小（字节）。
</div>

<div style="margin: 0 0 1em 4em;">

如果此参数（parameter）没有设置，默认值为128KiB，并且门槛会动态调整，以适应程序的分配模式。如果此参数（parameter），动态调整被禁用，并且此值被静态的设置成输入值。
</div>

<div style="margin: 0 0 1em 4em;">

此参数（parameter）也可以，在进程启动时，通过设置环境变量`MALLOC_TRIM_THRESHOLD_`为想要的值来设置。
</div>

<div style="margin: 0 0 1em 2em;">

`M_ARENA_TEST`
</div>

<div style="margin: 0 0 1em 4em;">

此参数（parameter）决定了在arena的限制测试实施之前，可创建的arenas数量。如果`M_ARENA_MAX`设置了，此值会被忽略。
</div>

<div style="margin: 0 0 1em 4em;">

在32位系统上，此参数（parameter）的默认值为2，然后在64位系统上，为8。
</div>

<div style="margin: 0 0 1em 4em;">

此参数（parameter）也可以，在进程启动时，通过设置环境变量`MALLOC_ARENA_TEST`为想要的值来设置。
</div>

<div style="margin: 0 0 1em 2em;">

`M_ARENA_MAX`
</div>

<div style="margin: 0 0 1em 4em;">

此参数（parameter）设置arena的数量，不管系统中的核心数量是多少。
</div>

<div style="margin: 0 0 1em 4em;">

此可调参数的默认值为`0`，意味着限制为get_nprocs()报告的在线CPU核心数量。
</div>

<div style="margin: 0 0 1em 4em;">

此参数（parameter）也可以，在进程启动时，通过设置环境变量`MALLOC_ARENA_MAX`为想要的值来设置。
</div>

---

#### 3.2.3.9 堆一致性检查

你可以要求`malloc`检查动态内存的一致性，通过使用`mcheck`函数和用*LD_PRELOAD*环境变量预载malloc的调试库`libc_malloc_debug`。此函数是一个GNU扩展，在`mcheck.h`中声明。

函数：`int` **`mcheck`** `(` `void` `(` `*` *`abortfn`* `)` `(` `enum` `mcheck_status` *`status`* `)` `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Unsafe race:mcheck const:malloc_hooks | AS-Unsafe corrupt | AC-Unsafe corrupt |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

调用`mcheck`会使`malloc`偶尔执行一致性检查。这会捕获诸如在由`malloc`分配的块的末端以外的地方的写的操作。
</div>

<div style="margin: 0 0 1em 2em;">

*abortfn*参数（argument）是当不一致性找到时，调用的函数。如果你提供一个空指针，那么`mcheck`会使用默认的函数，打印信息并且调用`abort`（参考[Aborting a Program](https://sourceware.org/glibc/manual/latest/html_node/Aborting-a-Program.html)）。你提供的函数会被以一个参数调用，说明了哪一类的不一致性被检测到了；他的类型在下文描述。
</div>

<div style="margin: 0 0 1em 2em;">

当你已经用`malloc`分配了东西后，再开始分配检查就太迟了。所以，在那种情况下，`malloc`什么事都不会做。如果你调用他太迟了，函数返回`-1`，反之返回`0`（当他成功时）。
</div>

<div style="margin: 0 0 1em 2em;">

最简单的方式来安排调用`mcheck`足够早，是当你链接你的程序时，使用‘`-lmcheck`’选项；然后你就不用修改你的源代码了。或者，每当程序启动时，你可以使用调试器来插入一个`mcheck`调用，例如，每当程序启动时，这些gdb命令可以自动调用`mcheck`：
</div>

<div style="margin: 0 0 1em 4em;">

```
(gdb) break main
Breakpoint 1, main (argc=2, argv=0xbffff964) at whatever.c:10
(gdb) command 1
Type commands for when breakpoint 1 is hit, one per line.
End with a line saying just "end".
>call mcheck(0)
>continue
>end
(gdb) ...
```
</div>

<div style="margin: 0 0 1em 2em;">

然而，只有在没有任何包含调用`malloc`的任何对象的初始化程序的情况下，这才会有用，因为`mcheck`必须在第一个这样的函数之前调用。
</div>

函数：`enum` `mcheck_status` **`mprobe`** `(` `void` `*` *`pointer`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Unsafe race:mcheck const:malloc_hooks | AS-Unsafe corrupt | AC-Unsafe corrupt |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

`mprobe`函数让你显式检查一个特定的分配的块中的不一致性。你必须已经在程序的开头调用了`mcheck`，来进行他的偶尔检查；调用`mprobe`会在调用时，请求一次额外的一致性检查。
</div>

<div style="margin: 0 0 1em 2em;">

参数（argument）*pointer*必须是一个由`malloc`或`realloc`返回的指针。`mprobe`返回一个值来说明什么不一致性被找到了，如果有的话。值在下文描述。
</div>

数据类型：**`enum`** **`mcheck_status`**

<div style="margin: 0 0 1em 2em;">

此枚举类型描述了在一个分配的块中什么类型的不一致性被检测到了，如果有的话。这里是可能的值：
</div>

<div style="margin: 0 0 1em 2em;">

`MCHECK_DISABLED`
</div>

<div style="margin: 0 0 1em 4em;">

`mcheck`不是在第一次分配前调用的。没有一致性检查可被执行。
</div>

<div style="margin: 0 0 1em 2em;">

`MCHECK_OK`
</div>

<div style="margin: 0 0 1em 4em;">

没有不一致性被检测到。
</div>

<div style="margin: 0 0 1em 2em;">

`MCHECK_HEAD`
</div>

<div style="margin: 0 0 1em 4em;">

块前的数据被修改。这通常发生在一个数组的索引或指针被减的太多了。
</div>

<div style="margin: 0 0 1em 2em;">

`MCHECK_TAIL`
</div>

<div style="margin: 0 0 1em 4em;">

块后的数据被修改。这通常发生在一个数组的索引或指针被加的太多了。
</div>

<div style="margin: 0 0 1em 2em;">

`MCHECK_FREE`
</div>

<div style="margin: 0 0 1em 4em;">

块已经被释放。
</div>

另一种检查和防范在使用`malloc`，`realloc`，`free`过程中的bug的可选方式是，设置环境变量`MALLOC_CHECK_`。当`MALLOC_CHECK_`被设置成小于4的非零值，一个特殊的（效率较低的）实现被使用，他被设计成可容忍一些简单错误，例如两次使用相同的参数调用`free`，或者单字节溢出（差一错误）。不是所有的此类错误都能被防护，并且会导致内存溢出。就像`mcheck`，你需要预载`libc_malloc_debug`库来启用`MALLOC_CHECK_`功能。不预载此库的话，设置`MALLOC_CHECK_`不会有效果。

任何检测到的堆损坏都会导致进程的瞬间中断。

`MALLOC_CHECK_`有一个问题：在SUID（设置用户ID）或SGID（设置组ID）的二进制程序中，他可能被利用，因为偏离了正常的程序行为，他会向标准错误描述符中写入一些内容。因此，对于SUID和SGID的二进制程序，`MALLOC_CHECK_`默认禁用。

那么，使用`MALLOC_CHECK_`和用‘`-lmcheck`’链接之间的区别是什么？`MALLOC_CHECK_`相对于‘`-lmcheck`’是正交的。‘`-lmcheck`’是为了向后兼容才添加的。`MALLOC_CHECK_`和‘`-lmcheck`’两者都能发现相同错误——但是使用`MALLOC_CHECK_`的话，你无需重新编译你的程序。

---

#### 3.2.3.10 使用`malloc`进行内存分配的统计信息

你可以通过调用`mallinfo2`函数来获取动态内存分配的信息。此函数和他相关的数据类型在`malloc.h`中声明；他们是一个标准SVID/XPG版本的扩展。

数据类型：<strong>`struct mallinfo2`</strong>

<div style="margin: 0 0 1em 2em;">

此结构体类型是用来返回动态内存分配器的信息的。他包含以下成员：
</div>

<div style="margin: 0 0 1em 2em;">

`size_t` `arena`
</div>

<div style="margin: 0 0 1em 4em;">

这是`malloc`通过`sbrk`分配的内存的大小，字节。
</div>

<div style="margin: 0 0 1em 2em;">

`size_t` `ordblks`
</div>

<div style="margin: 0 0 1em 4em;">

这是不在使用的chunk的数量。（内存分配器内部从操作系统获取多块内存，然后划分他们，以满足每个`malloc`请求；参考[The GNU Allocator](https://sourceware.org/glibc/manual/latest/html_node/The-GNU-Allocator.html)。）
</div>

<div style="margin: 0 0 1em 2em;">

`size_t` `smblks`
</div>

<div style="margin: 0 0 1em 4em;">

该区域未使用。
</div>

<div style="margin: 0 0 1em 2em;">

`size_t` `hblks`
</div>

<div style="margin: 0 0 1em 4em;">

这是`mmap`分配的chunk的总数。
</div>

<div style="margin: 0 0 1em 2em;">

`size_t` `hblkhd`
</div>

<div style="margin: 0 0 1em 4em;">

这是`mmap`分配的内存的总大小，字节。
</div>

<div style="margin: 0 0 1em 2em;">

`size_t` `usmblks`
</div>

<div style="margin: 0 0 1em 4em;">

此字段未使用，始终为0.
</div>

<div style="margin: 0 0 1em 2em;">

`size_t` `fsmblks`
</div>

<div style="margin: 0 0 1em 4em;">

该区域未使用。
</div>

<div style="margin: 0 0 1em 2em;">

`size_t` `uordblks`
</div>

<div style="margin: 0 0 1em 4em;">

这是`mmap`分配的块占用的内存的总大小。
</div>

<div style="margin: 0 0 1em 2em;">

`size_t` `fordblks`
</div>

<div style="margin: 0 0 1em 4em;">

这是通过释放chunk（未使用）获得的内存的总大小。
</div>

<div style="margin: 0 0 1em 2em;">

`size_t` `keepcost`
</div>

<div style="margin: 0 0 1em 4em;">

这是最顶部的可释放chunk的大小，通常紧接着堆的末端（即，虚拟地址空间的数据段的高端）。
</div>

函数：`struct` `mallinfo2` **`mallinfo2`** `(` `void` `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Unsafe init const:mallopt | AS-Unsafe init lock | AC-Unsafe init lock |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

此函数通过结构体`struct` `mallinfo2`返回当前动态内存使用情况。
</div>

---

#### 3.2.3.11 `malloc`相关函数总结

这里是和`malloc`一起工作的函数的总结：

`void` `*` `malloc` `(` `size_t` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

分配一个*size*字节的块。参考[Basic Memory Allocation](https://sourceware.org/glibc/manual/latest/html_node/Basic-Allocation.html)。
</div>

`void` `free` `(` `void` `*` *`addr`* `)`

<div style="margin: 0 0 1em 2em;">

释放一个先前用`malloc`分配的块。参考[Freeing Memory Allocated with `malloc`](https://sourceware.org/glibc/manual/latest/html_node/Freeing-after-Malloc.html)。
</div>

`void` `*` `realloc` `(` `void` `*` *`addr`* `,` `size_t` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

让一个先前用`malloc`分配的块更大或更小，可能需要复制他到一个新区域。参考[Changing the Size of a Block](https://sourceware.org/glibc/manual/latest/html_node/Changing-Block-Size.html)。
</div>

`void` `*` `reallocarray` `(` `void` `*` *`ptr`* `,` `size_t` *`nmemb`* `,` `size_t` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

就像`realloc`一样，将一个先前用`malloc`分配的块的大小改成*`nmemb`* * *`size`*字节。参考[Changing the Size of a Block](https://sourceware.org/glibc/manual/latest/html_node/Changing-Block-Size.html)。
</div>

`void` `*` `calloc` `(` `size_t` *`count`* `,` `size_t` *`eltsize`* `)`

<div style="margin: 0 0 1em 2em;">

使用`malloc`分配一个*count* * *eltsize*字节的块，并且设置他的内容为零。参考[Allocating Cleared Space](https://sourceware.org/glibc/manual/latest/html_node/Allocating-Cleared-Space.html)
</div>

`void` `*` `valloc` `(` `size_t` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

分配一个*size*字节的块，在页（page）的边界开始。参考[Allocating Aligned Memory Blocks](https://sourceware.org/glibc/manual/latest/html_node/Aligned-Memory-Blocks.html)。
</div>

`void` `*` `aligned_alloc` `(` `size_t` *`alignment`* `,` `size_t` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

分配一个*size*字节的块，在*alignment*的整数倍的地址开始。参考[Allocating Aligned Memory Blocks](https://sourceware.org/glibc/manual/latest/html_node/Aligned-Memory-Blocks.html)。
</div>

`int` `posix_memalign` `(` `void` `**` *`memptr`* `,` `size_t` *`alignment`* `,` `size_t` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

分配一个*size*字节的块，在*alignment*的整数倍的地址开始。参考[Allocating Aligned Memory Blocks](https://sourceware.org/glibc/manual/latest/html_node/Aligned-Memory-Blocks.html)。
</div>

`void` `*` `memalign` `(` `size_t` *`boundary`* `,` `size_t` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

分配一个*size*字节的块，在*boundary*的整数倍的地址开始。参考[Allocating Aligned Memory Blocks](https://sourceware.org/glibc/manual/latest/html_node/Aligned-Memory-Blocks.html)。
</div>

`int` `mallopt` `(` `int` *`param`* `,` `int` *`value`* `)`

<div style="margin: 0 0 1em 2em;">

调整一个可调参数。参考[Malloc Tunable Parameters](https://sourceware.org/glibc/manual/latest/html_node/Malloc-Tunable-Parameters.html)。
</div>

`int` `mcheck` `(` `void` `(` `*` *`abortfn`* `)` `(` `void` `)` `)`

<div style="margin: 0 0 1em 2em;">

告诉`malloc`对动态分配的内存执行偶尔的一致性检查，并且当一个不一致性被找到时，调用*abortfn*，参考[Heap Consistency Checking](https://sourceware.org/glibc/manual/latest/html_node/Heap-Consistency-Checking.html)。
</div>

`struct` `mallinfo2` `mallinfo2` `(` `void` `)`

<div style="margin: 0 0 1em 2em;">

然会当前动态内存使用情况的信息。参考[Statistics for Memory Allocation with `malloc`](https://sourceware.org/glibc/manual/latest/html_node/Statistics-of-Malloc.html)。
</div>

---

### 3.2.4 分配调试

在使用不使用垃圾回收的动态内存分配的编程语言时，查找内存泄漏是一个复杂的任务。长时间运行的程序必须保证动态分配的对象在他们的生命周期结束时被释放。如果这不发生，系统迟早会用完内存。

the GNU C Library中的`malloc`实现提供一些简单方法来检测这种泄露，并且获得一些信息以找到位置。为了做到这个，程序必须在一个特别的模式下启动，这个模式通过一个环境变量启用。如果未启动调试模式，程序不会有速度损失。

- [How to install the tracing functionality](https://sourceware.org/glibc/manual/latest/html_node/Tracing-malloc.html)

- [Example program excerpts](https://sourceware.org/glibc/manual/latest/html_node/Using-the-Memory-Debugger.html)

- [Some more or less clever ideas](https://sourceware.org/glibc/manual/latest/html_node/Tips-for-the-Memory-Debugger.html)

- [Interpreting the traces](https://sourceware.org/glibc/manual/latest/html_node/Interpreting-the-traces.html)

---

#### 3.2.4.1 如何安装跟踪功能

函数：`void` **`mtrace`** `(` `void` `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Unsafe env race:mtrace init | AS-Unsafe init heap corrupt lock | AC-Unsafe init corrupt lock fd mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

`mtrace`函数提供一种方式来追踪在调用他的程序中的内存分配事件。在库中，他是默认禁用的，他可以通过使用`LD_PRELOAD`环境变量来预载调试库`libc_malloc_debug`来启用。
</div>

<div style="margin: 0 0 1em 2em;">

当调用`mtrace`函数时，他会查找一个叫`MALLOC_TRACE`的环境变量。此变量此变量应该会包含一个有效文件名。用户必须有写权限。如果文件已存在，他会被截断。若环境变量没有设置，或者他没有指定一个可用的文件，可打开写的文件，不会发生任何事。`malloc`等的行为不会被改变。出于显而易见的原因，如果程序安装时设置了SUID或SGID位，也会发生这种情况。
</div>

<div style="margin: 0 0 1em 2em;">

如果指定的文件成功打开了，`mtrace`会为`malloc`，`realloc`，`free`函数安装特殊的处理程序。从那时起，这些函数的所有使用都被跟踪，并且经协议记录到文件。当然，现在，对跟踪函数的所有调用都有速度损失，所以追踪不应该在正常使用中启用。
</div>

<div style="margin: 0 0 1em 2em;">

此函数是一个GNU扩展，并且通常在其他系统上不可用。函数原型可以在`mcheck.h`中找到。
</div>

函数：`void` **`muntrace`** `(` `void` `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Unsafe race:mtrace locale | AS-Unsafe corrupt heap | AC-Unsafe corrupt mem lock fd |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

在`mtrace`启用对`malloc`调用的追踪后，`muntrace`函数可以被调用。若没有`mtrace`的（成功）调用，`muntrace`不会做事。
</div>

<div style="margin: 0 0 1em 2em;">

否则，他会卸载`malloc`，`realloc`，`free`的处理程序，并且关闭记录文件。没有调用被记录，并且程序重新全速运行。
</div>

<div style="margin: 0 0 1em 2em;">

此函数是一个GNU扩展，并且通常在其他系统上不可用。函数原型可以在`mcheck.h`中找到。
</div>

---

#### 3.2.4.2 示例程序片段

就算跟踪功能不会影响程序的运行时行为，在所有程序中调用`mtrace`也不是一个好主意。设想一下，你用`mtrace`调试一个程序，并且在调试过程中用到的所有其他程序都会追踪他们的`malloc`调用。所有的程序的输出文件都是同一个，因此就不可用了。因此，你应该只在为了调试而编译时启用`mtrace`。因此，一个程序可以这样开头：

<div style="margin: 0 0 1em 2em;">

```c
#include <mcheck.h>

int
main (int argc, char *argv[])
{
#ifdef DEBUGGING
  mtrace ();
#endif
  ...
}
```
</div>

如果你想在整个程序的运行期间追踪那些调用，这就是所有需要做的。或者，你也可以在任何时间调用`muntrace`来停止追踪。甚至可以调用`mtrace`来重新追踪。但这可能导致一些不可靠结果，因为可能有些函数调用没有被追踪（原文是called，AI说应该是写错了，应该是traced）。请注意，不仅程序会使用追踪功能，库（包括the C library本身）也会使用这些功能。

最后一点也是在程序结束前调用`muntrace`不是一个好主意的原因。库只有在程序从`main`中返回或调用`exit`之后，才会被通知程序的结束，并且因此，在此之前，他们无法释放他们使用的内存。

所以你最好的方式是在程序中尽早调用`mtrace`并且不调用`muntrace`。所以程序追踪几乎所有`malloc`函数的使用（除了那些由程序的或使用的库的构造器执行的调用）。

---

#### 3.2.4.3 一些更聪明或更不聪明的想法

你知道这种情况。程序是为调试准备的，并且在所有调试会话中，他运行的很好。但是当他离开调试，错误就发生了。一个经典的例子是一个只有关闭调试时才可见的内存泄露。如果你遇见了这种情况，你仍能赢。只需要使用下面小程序等效做法：

<div style="margin: 0 0 1em 2em;">

```c
#include <mcheck.h>
#include <signal.h>

static void
enable (int sig)
{
  mtrace ();
  signal (SIGUSR1, enable);
}

static void
disable (int sig)
{
  muntrace ();
  signal (SIGUSR2, disable);
}

int
main (int argc, char *argv[])
{
  ...

  signal (SIGUSR1, enable);
  signal (SIGUSR2, disable);

  ...
}
```
</div>

也就是说，如果程序启动时环境变量中设置了`MALLOC_TRACE`，用户可以在任何时间启用内存调试器，只要他/她想。当然，输出不会有第一个信号前的分配，但如果存在内存泄露，他仍会显示出来。

---

#### 3.2.4.4 解读追踪

如果你看一下输出，他大概是这样：

<div style="margin: 0 0 1em 2em;">

```
= Start
  [0x8048209] - 0x8064cc8
  [0x8048209] - 0x8064ce0
  [0x8048209] - 0x8064cf8
  [0x80481eb] + 0x8064c48 0x14
  [0x80481eb] + 0x8064c60 0x14
  [0x80481eb] + 0x8064c78 0x14
  [0x80481eb] + 0x8064c90 0x14
= End
```
</div>

这些是什么意思不是特别重要，因为追踪文件不是给人读的。因此，不考虑可读性。取而代之的是，the GNU C Library中有一个程序，他会解读追踪，并且输出一个对用户友好的摘要。这个程序叫`mtrace`（他实际上是一个Perl脚本），他接收一个或两个参数。无论如何，追踪输出的文件名必须被指定。如果在追踪文件名前还有一个参数，他必须是生成追踪的程序的名称。

<div style="margin: 0 0 1em 2em;">

```
drepper$ mtrace tst-mtrace log
No memory leaks.
```
</div>

在这个例子中，程序`tst-mtrace`运行了，并且生成了追踪文件`log`。`mtrace`打印的信息显示代码没有问题，所有分配的内存后来都被释放了。

如果我们对上文的示例追踪调用`mtrace`，我们可以得到一个不同的输出：

<div style="margin: 0 0 1em 2em;">

```
drepper$ mtrace errlog
- 0x08064cc8 Free 2 was never alloc'd 0x8048209
- 0x08064ce0 Free 3 was never alloc'd 0x8048209
- 0x08064cf8 Free 4 was never alloc'd 0x8048209

Memory not freed:
-----------------
   Address     Size     Caller
0x08064c48     0x14  at 0x80481eb
0x08064c60     0x14  at 0x80481eb
0x08064c78     0x14  at 0x80481eb
0x08064c90     0x14  at 0x80481eb
```
</div>

我们只用了一个参数调用`mtrace`，所以脚本不知道追踪文件中给出的地址是什么意思。我们可以做的更好：

<div style="margin: 0 0 1em 2em;">

```
drepper$ mtrace tst errlog
- 0x08064cc8 Free 2 was never alloc'd /home/drepper/tst.c:39
- 0x08064ce0 Free 3 was never alloc'd /home/drepper/tst.c:39
- 0x08064cf8 Free 4 was never alloc'd /home/drepper/tst.c:39

Memory not freed:
-----------------
   Address     Size     Caller
0x08064c48     0x14  at /home/drepper/tst.c:33
0x08064c60     0x14  at /home/drepper/tst.c:33
0x08064c78     0x14  at /home/drepper/tst.c:33
0x08064c90     0x14  at /home/drepper/tst.c:33
```
</div>

突然之间，输出就合理了很多，用户可以立即看到是哪里的函数调用造成了可被发现的问题。

解读此输出不复杂。最多会检测两种不同的情况。第一，调用`free`给不是分配函数之一返回的指针使用。这通常是一个非常严重的问题，这看上去就像输出的前三行展示的那样。这种情况有点少见，并且一旦出现，会非常剧烈：程序一般会崩溃。

The other situation which is much harder to detect are memory leaks. As you can see in the output the `mtrace` function collects all this information and so can say that the program calls an allocation function from line 33 in the source file `/home/drepper/tst-mtrace.c` four times without freeing this memory before the program terminates. Whether this is a real problem remains to be investigated.
另一种情况是内存泄露，这更难被检测。正如你在输出文件中看到的，`mtrace`函数收集所有信息，所以可以指出程序在源文件`/home/drepper/tst-mtrace.c`的33行中调用了一个分配函数四次，但是在程序结束前没有释放内存。这是否是一个问题，还有待商榷。

---

### 3.2.5 替换`malloc`

The GNU C Library支持使用一个相同接口的不同分配器替换内置的`malloc`实现。对于动态链接的程序，这是通过ELF符号介入实现的，可以使用共享对象依赖或`LD_PRELOAD`。对于静态链接，`malloc`代替库必须在链接`libc.a`之前链接进来（显示或隐式的）。

注意不要使用the GNU C Library中在内部使用`malloc`的函数。例如，`fopen`，`opendir`，`dlopen`，`pthread_setspecific`目前内部使用了`malloc`子系统。如果替换的`malloc`或他的依赖使用了线程局部存储（TLS），他必须使用the initial-exec TLS模型，而不能使用任何动态TLS变体。

<strong>注意：</strong>如果未能提供一套完整的替换函数（即应用程序，the GNU C Library，其他链接进来的库所用到的函数），可能会导致静态链接错误，并且在运行时，导致堆损坏或程序崩溃。替换函数应该实现他们对应函数在the GNU C Library中记录的行为；例如，`malloc`的替换函数应该仅在失败时返回一个空指针，他应该返回关于`alignof` `(` `max_align_t` `)`对齐的指针，`free`的替换函数应该保留`errno`。

自定义`malloc`必须提供的最小函数集如下表所示。

`malloc`

`free`

`calloc`

`realloc`

the GNU C Library需要这些`malloc`相关函数来运行。<a href="#3.2.5note1" id="3.2.5note1ref">1</a>

the GNU C Library中，`malloc`的实现提供库本身不使用的额外的功能，但是经常被其他系统库和程序使用。一个通用的替换`malloc`实现也应该提供下面函数的定义。他们的名字如下表所示。

`aligned_alloc`

`free_aligned_sized`

`free_sized`

`malloc_usable_size`

`memalign`

`posix_memalign`

`pvalloc`

`valloc`

此外，非常旧的程序可能会使用过时的`cfree`函数。

诸如`mallopt`或`mallinfo2`这类进一步与`malloc`相关的函数，在使用一个`malloc`的替换函数时，不会有任何效果，或者返回错误的统计信息。然而，未能替换这些函数通常不会导致崩溃或其他错误程序行为，但是可能导致静态链接错误。

the GNU C Library中还有其他函数（`reallocarray`，`strdup`等）未在上面列出，但是会返回新分配的内存给调用者。这些函数的替换函数不被支持，并且可能产生错误的结果。这些函数的The GNU C Library实现会在可用时调用替换的分配器，所以他们能正确的运行`malloc`替换函数。

---

**脚注**

<a id="3.2.5note1" href="#3.2.5note1ref">(1)</a>

the GNU C Library 2.25版本之前，需要自定义`malloc`定义`__libc_memalign`（和`memalign`函数有一样的接口）。

---

### 3.2.6 对象栈

一个obstack时一个内存池，包含一个对象栈。你可以创建任意数量的独立obstack，然后再指定的obstack中分配对象。再每个obstack中，最后一个分配的对象必须是第一个被释放的，但是不同的obstack之间互相独立。

除了这个释放顺序的限制，obstack是完全通用的：一个obstack可以包含任意数量的任意大小的对象。他们是通过宏实现的，所以只要对象非常小，分配会非常快。并且，每个对象的唯一的空间开销，就是让每个对象在合适边界上开始所需的填充。

- [Creating Obstacks](https://sourceware.org/glibc/manual/latest/html_node/Creating-Obstacks.html)

- [Preparing for Using Obstacks](https://sourceware.org/glibc/manual/latest/html_node/Preparing-for-Obstacks.html)

- [Allocation in an Obstack](https://sourceware.org/glibc/manual/latest/html_node/Allocation-in-an-Obstack.html)

- [Freeing Objects in an Obstack](https://sourceware.org/glibc/manual/latest/html_node/Freeing-Obstack-Objects.html)

- [Obstack Functions and Macros](https://sourceware.org/glibc/manual/latest/html_node/Obstack-Functions.html)

- [Growing Objects](https://sourceware.org/glibc/manual/latest/html_node/Growing-Objects.html)

- [Extra Fast Growing Objects](https://sourceware.org/glibc/manual/latest/html_node/Extra-Fast-Growing.html)

- [Status of an Obstack](https://sourceware.org/glibc/manual/latest/html_node/Status-of-an-Obstack.html)

- [Alignment of Data in Obstacks](https://sourceware.org/glibc/manual/latest/html_node/Obstacks-Data-Alignment.html)

- [Obstack Chunks](https://sourceware.org/glibc/manual/latest/html_node/Obstack-Chunks.html)

- [Summary of Obstack Functions](https://sourceware.org/glibc/manual/latest/html_node/Summary-of-Obstacks.html)

---

#### 3.2.6.1 创建obstack

用于操作obstack的工具在头文件`obstack.h`中声明。

数据类型：**`struct`** **`obstack`**

<div style="margin: 0 0 1em 2em;">

一个obstack是由`struct` `obstack`类型的数据结构表示的。此结构有一个小的固定的大小；他记录了obstack的状态，还有怎么找到对象被分配的地址。他不包含任何对象本身。你不应该尝试直接访问结构的内容；只使用此章节中描述的函数。
</div>

你可以声明`struct` `obstack`类型的变量，并且当成obstack使用他们，或者你可以动态的分配obstack，就像其他类型的对象一样。obstack动态分配允许你的程序有数量可变的不同栈。（你甚至可以在一个obstack中分配一个obstack结构，但这很少有用。）

所有用obstack允许的函数需要你指定使用哪个obstack。你通过一个`struct` `obstack` `*`类型的指针来做这个。接下来，我们经常说“一个obstack”，但严格来说，手头的对象是一个指针。

obstack中的对象被打包进大的块，他称为chunk。`struct` `obstack`结构指向一个当前正在使用的chunk链。

当你分配一个对象，他无法放入前一个chunk中时，obstack库会得到一个新的chunk。因为obstack库会自动管理chunk，你不需要关心他们，但是你需要提供一个函数，使obstack库使用他来获取一个chunk。通常，你提供的一个函数会直接或间接使用`malloc`。你也必须提供一个函数来释放一个chunk。这些事情在下节中描述。

---

#### 3.2.6.2 使用obstack的准备工作

你计划使用obstack函数的每个源文件都必须包含`obstack.h`头文件，比如这样：

<div style="margin: 0 0 1em 2em;">

```c
#include <obstack.h>
```
</div>

此外，如果源文件使用了宏`obstack_init`，他必须声明或定义两个函数或宏，用来给obstack库调用。一个是，`obstack_chunk_alloc`，用来分配对象所打包到的内存chunk。另一个是，`obstack_chunk_free`，用来在他们中的对象被释放时返回chunk。这些宏需要在源文件中的任何obstack使用前出现。

通常，他们被定义成通过中间人`xmalloc`来使用`malloc`（参考[Unconstrained Allocation](https://sourceware.org/glibc/manual/latest/html_node/Unconstrained-Allocation.html)）。这可以通过下面这对宏定义来实现：

<div style="margin: 0 0 1em 2em;">

```c
#define obstack_chunk_alloc xmalloc
#define obstack_chunk_free free
```
</div>

尽管你使用obstack时获得的内存实际上来自于`malloc`，使用obstack还是更快，因为`malloc`更少被调用，每次分配的是更大的内存块。参考[Obstack Chunks](https://sourceware.org/glibc/manual/latest/html_node/Obstack-Chunks.html)，以获得完整的细节。

在运行时，在程序使用一个`struct` `obstack`对象作为一个obstack之前，他必须调用`obstack_init`初始化obstack。

函数：`int` **`obstack_init`** `(` `struct` `obstack` `*` *`obstack-ptr`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe race:obstack-ptr | AS-Safe | AC-Safe mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

为了对象的分配，初始化obstack *`obstack-ptr`*。此函数调用obstack的`obstack_chunk_alloc`函数。如果内存分配失败，`obstack_alloc_failed_handler`指向的函数被调用，`obstack_init`函数总是返回1（兼容性说明：早期的obstack版本在分配失败时返回0）。
</div>

这里有两个如何给一个obstack分配空间并初始化他的例子。第一，一个作为一个静态变量的obstack：

<div style="margin: 0 0 1em 2em;">

```c
static struct obstack myobstack;
...
obstack_init (&myobstack);
```
</div>

第二，是一个本身动态分配的obstack：
<div style="margin: 0 0 1em 2em;">

```c
struct obstack *myobstack_ptr
  = (struct obstack *) xmalloc (sizeof (struct obstack));

obstack_init (myobstack_ptr);
```
</div>

变量：**`obstack_alloc_failed_handler`**

<div style="margin: 0 0 1em 2em;">

此变量的值是一个函数指针，obstack在`obstack_chunk_alloc`分配内存失败时使用他。默认行为是打印一条信息并中止。你应该提供一个函数，要么调用`exit`（参考[Program Termination](https://sourceware.org/glibc/manual/latest/html_node/Program-Termination.html)），要么调用`longjmp`（参考[Non-Local Exits](https://sourceware.org/glibc/manual/latest/html_node/Non_002dLocal-Exits.html)），不要返回。
</div>

<div style="margin: 0 0 1em 2em;">

```c
void my_obstack_alloc_failed (void);
...
obstack_alloc_failed_handler = &my_obstack_alloc_failed;
```
</div>

---

#### 3.2.6.3 在一个obstack中分配

最直接的在一个obstack分配一个对象的方式是使用`obstack_alloc`，调用几乎和`malloc`一样。

函数：`void` `*` **`obstack_alloc`** `(` `struct` `obstack` `*` *`obstack-ptr`* `,` `int` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe race:obstack-ptr | AS-Safe | AC-Unsafe corrupt mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

他在一个obstack中分配一个*size*字节大小的未初始化的块，并且返回他的地址。*obstack-ptr*指定在哪个obstack中分配块；他是`struct` `obstack`对象的地址，代表着obstack。每一个obstack函数或宏需要你指定一个*obstack-ptr*作为第一个参数。
</div>

<div style="margin: 0 0 1em 2em;">

如果他需要分配一个新内存chunk，此函数调用obstack的`obstack_chunk_alloc`函数；如果`obstack_chunk_alloc`分配内存失败，他调用`obstack_alloc_failed_handler`。
</div>

例如，这一个函数分配一个字符串str的副本到一个指定的obstack，也就是`string_obstack`变量中：

<div style="margin: 0 0 1em 2em;">

```c
struct obstack string_obstack;

char *
copystring (char *string)
{
  size_t len = strlen (string) + 1;
  char *s = (char *) obstack_alloc (&string_obstack, len);
  memcpy (s, string, len);
  return s;
}
```
</div>

分配一个具有指定内容的块，使用`obstack_copy`，声明如下：

函数：`void` `*` **`obstack_copy`** `(` `struct` `obstack` `*` *`obstack-ptr`* `,` `void` `*` *`address`* `,` `int` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe race:obstack-ptr | AS-Safe | AC-Unsafe corrupt mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

他分配一个块，并复制从*address*开始的*size*字节数据初始化他。如果`obstack_chunk_alloc`分配内存失败，他调用`obstack_alloc_failed_handler`。
</div>

函数：`void` `*` **`obstack_copy0`** `(` `struct` `obstack` `*` *`obstack-ptr`* `,` `void` `*` *`address`* `,` `int` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe race:obstack-ptr | AS-Safe | AC-Unsafe corrupt mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

类似`obstack_copy`，但是后面接一个额外的字节，包含一个空字符。此额外的字节不计算在参数*size*中。
</div>

`obstack_copy0`函数便于复制一串字符到一个obstack中，作为以空结尾的字符串。这里有一个用例：

<div style="margin: 0 0 1em 2em;">

```c
char *
obstack_savestring (char *addr, int size)
{
  return obstack_copy0 (&myobstack, addr, size);
}
```
</div>

对比他和前面使用`malloc`的`savestring`的例子（参考[Basic Memory Allocation](https://sourceware.org/glibc/manual/latest/html_node/Basic-Allocation.html)）。

---

#### 3.2.6.4 在一个obstack中释放对象

释放在一个obstack分配的一个对象，使用`obstack_free`函数。因为obstack是一个对象们的栈，释放一个对象会自动释放同一个obstack中分配的更晚的所有其他对象。

函数：`void` **`obstack_free`** `(` `struct` `obstack` `*` *`obstack-ptr`* `,` `void` `*` `object` `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe race:obstack-ptr | AS-Safe | AC-Unsafe corrupt |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

如果`object`是一个空指针，所有在obstack中分配的都会被释放。否则，`object`必须是obstack中已分配的一个对象的地址。然后`object`被释放，`obstack-ptr`中所有`object`以后被分配的也会。
</div>

注意，如果*object*是一个空指针，结构是一个未初始化的obstack。释放一个obstack中的所有内存，但是使他能够继续分配，可以使用obstack中分配的第一个对象的地址来调用`obstack_free`：

<div style="margin: 0 0 1em 2em;">

```c
obstack_free (obstack_ptr, first_object_allocated_ptr);
```
</div>

回忆一下，一个obstack中的对象们是按chunk分组的。当一个chunk中的所有对象都被释放时，obstack库自动释放chunk（参考[Preparing for Using Obstacks](https://sourceware.org/glibc/manual/latest/html_node/Preparing-for-Obstacks.html)）。然后其他obstack，或非obstack的分配，可以再使用那个chunk的空间。

---

#### 3.2.6.5 obstack函数和宏

使用obstack的接口定义为函数还是宏，取决于编译器。obstack功能在所有C编译器上可用，包括ISO C和传统C，但是如果你想使用GNU C以外的编译器，你必须采取预防措施。

如果你在使用一个老式的非ISO C编译器，所有obstack“函数”实际上是宏。你可以像调用函数一样调用这些宏，但是你不能以其他方式使用他们（比如说取地址）。

调用宏需要一个特别的预防措施：即，第一个操作数（obstack指针）不能包含任何副作用，因为他可能不止被计算一次。例如，如果你写这个：

<div style="margin: 0 0 1em 2em;">

```c
obstack_alloc (get_obstack (), 4);
```
</div>

你会发现，`get_obstack`可能被调用多次。如果你使用`*obstack_list_ptr++`作为obstack指针参数，你可能获得非常奇怪的结果，因为增加可能会发生多次。

在ISO C中，每个函数都有一个宏定义和一个函数定义。函数定义是用来让你只取函数的地址但是不调用他。一个普通的调用默认会使用宏定义，但是你可以将函数名写在括号中来请求函数定义，就像这样：

<div style="margin: 0 0 1em 2em;">

```c
char *x;
void *(*funcp) ();
/* Use the macro.  */
x = (char *) obstack_alloc (obptr, size);
/* Call the function.  */
x = (char *) (obstack_alloc) (obptr, size);
/* Take the address of the function.  */
funcp = obstack_alloc;
```
</div>

ISO C中的标准库函数也存在相同的情况。参考[Macro Definitions of Functions](https://sourceware.org/glibc/manual/latest/html_node/Macro-Definitions.html)。

<strong>警告：</strong>当你使用宏时，你必须遵循避免第一个操作数的副作用的预防措施，即使在ISO C中。

若你使用GUN C编译器，此预防措施不是必要的，因为GNU C中的各种语言扩展允许定义宏时对每个参数只计算一次。

---

#### 3.2.6.6 Growing Objects（增长对象）

因为obstack chunk中的内存是按顺序使用的，所以可能逐步构建一个对象，每次向对象的结尾添加一个或多个字节。使用这种技术，你不需要知道你需要在对象中放进多少数据，一直到他的末尾。我们称他为增长对象技术。那个用来在增长对象中添加数据的特别的函数在本章节中描述。

当你开始增长一个对象时，你不需要做任何事。使用其中一个函数来向对象中添加数据会自动开始增长对象。然而，对象结束时，需要显式的说明。通过函数`obstack_finish`完成。

这样构建的对象的实际地址是未知的，直到对象结束。在那之前，始终存在这种可能，你增加了太多数据，以至于对象必须被复制到一个新chunk。

当obstack在作为增长对象使用时，你不能用他来普通的分配另一个对象。如果你尝试这样做，已经加入增长对象的空间可能变成另一个对象的一部分。

函数：`void` **`obstack_blank`** `(` `struct` `obstack` `*` *`obstack-ptr`* `,` `int` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe race:obstack-ptr | AS-Safe | AC-Unsafe corrupt mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

用来添加数据到增长对象的最基础的函数是`obstack_blank`，他只添加空间，不初始化。
</div>

函数：`void` **`obstack_grow`** `(` `struct` `obstack` `*` *`obstack-ptr`* `,` `void` `*` *`data`* `,` `int` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe race:obstack-ptr | AS-Safe | AC-Unsafe corrupt mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

添加一块已初始化的空间，使用`obstack_grow`，增长对象版本的`obstack_copy`。他向增长对象中添加*size*字节的数据，复制*data*的内容。
</div>

函数：`void` **`obstack_grow0`** `(` `struct` `obstack` `*` *`obstack-ptr`* `,` `void` `*` *`data`* `,` `int` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe race:obstack-ptr | AS-Safe | AC-Unsafe corrupt mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

增长对象版本的`obstack_copy0`。他从*data*中添加*size*字节，后面接一个额外的空字符。
</div>

函数：`void` **`obstack_1grow`** `(` `struct` `obstack` `*` *`obstack-ptr`* `,` `char` *`c`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe race:obstack-ptr | AS-Safe | AC-Unsafe corrupt mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

要一次添加一个字符，使用`obstack_1grow`函数。他添加一个包含*c*的单一字节到增长对象。
</div>

函数：`void` **`obstack_ptr_grow`** `(` `struct` `obstack` `*` *`obstack-ptr`* `,` `void` `*` *`data`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe race:obstack-ptr | AS-Safe | AC-Unsafe corrupt mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

添加一个指针，你可以使用`obstack_ptr_grow`函数。他添加`sizeof (void *)`字节，包含着*data*的值。
</div>

函数：`void` **`obstack_int_grow`** `(` `struct` `obstack` `*` *`obstack-ptr`* `,` `int` *`data`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe race:obstack-ptr | AS-Safe | AC-Unsafe corrupt mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

可以使用`obstack_int_grow`函数添加一个单一的`int`类型的值。他添加`sizeof (int)`字节到增长对象中，并用*data*的值初始化他们。
</div>

函数：`void` `*` **`obstack_finish`** `(` `struct` `obstack` `*` *`obstack-ptr`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe race:obstack-ptr | AS-Safe | AC-Unsafe corrupt |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

当你结束增长对象时，使用`obstack_finish`函数关闭他，并返回他的最终地址。
</div>

<div style="margin: 0 0 1em 2em;">

当你结束了对象后，obstack可以被用于普通分配或者增长其他对象。
</div>

<div style="margin: 0 0 1em 2em;">

在与`obstack_alloc`相同情况下，此函数可以返回一个空指针（参考[Allocation in an Obstack](https://sourceware.org/glibc/manual/latest/html_node/Allocation-in-an-Obstack.html)）。
</div>

当你通过增长构建一个对象时，你可能需要知道他最终变得多长。你不需要再增长对象时追踪他，因为你可以在使用`obstack_object_size`函数结束对象之前，直接从obstack中查出长度。声明如下：

函数：`int` **`obstack_object_size`** `(` `struct` `obstack` `*` *`obstack-ptr`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe race:obstack-ptr | AS-Safe | AC-Safe |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

此函数返回当前增长对象的大小，单位字节。记住在结束对象前调用此函数。他结束之后，`obstack_object_size`将会返回零。
</div>

如果你已经开始增长一个对象，但希望取消他，你需要结束他，再释放他，就像这样：

<div style="margin: 0 0 1em 2em;">

```c
obstack_free (obstack_ptr, obstack_finish (obstack_ptr));
```
</div>

如果没有增长过对象，这样没有效果。

你可以使用一个负数size参数调用`obstack_blank`来使当前对象更小。别缩小到零长度以下——没有人知道你这样做会发生什么。

---

#### 3.2.6.7 超快速增长对象

增长对象的常用函数需要额外开销，用于检测当前chunk中有没有新增长的空间。如果你经常以小增长来构建对象，这种开销将会很可观。

你可以通过使用特殊的“快速增长”函数来减少开销，他们增长对象时不会检查。为了有一个稳定的程序，你必须自己检查。如果你每次添加对象都检查，你就没有节省任何开销，因为普通增长函数也是这样做的。但是如果你安排的检查更少，或者检查的效率更高，那么你的程序就会更快。

`obstack_room`函数返回当前chunk中可用的空间。他的声明如下：

函数：`int` **`obstack_room`** `(` `struct` `obstack` `*` *`obstack-ptr`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe race:obstack-ptr | AS-Safe | AC-Safe |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

他返回使用快速增长函数的obstack *obstack-ptr*中的当前增长对象（或一个即将开始的对象）可以安全添加的字节数量。
</div>

当你知道还有空间时，你可以使用这些快速增长函数来添加数据到增长对象：

函数：`void` **`obstack_1grow_fast`** `(` `struct` `obstack` `*` *`obstack-ptr`* `,` `char` *`c`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe race:obstack-ptr | AS-Safe | AC-Unsafe corrupt mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

`obstack_1grow_fast`函数添加一个包含字符*c*的字节到obstack *obstack-ptr*中的增长对象中。
</div>

函数：`void` **`obstack_ptr_grow_fast`** `(` `struct` `obstack` `*` *`obstack-ptr`* `,` `void` `*` *`data`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe race:obstack-ptr | AS-Safe | AC-Safe |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

`obstack_ptr_grow_fast`函数添加包含*data*值的`sizeof (void *)`字节到obstack *obstack-ptr*中的增长对象中。
</div>

函数：`void` **`obstack_int_grow_fast`** `(` `struct` `obstack` `*` *`obstack-ptr`* `,` `int` *`data`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe race:obstack-ptr | AS-Safe | AC-Safe |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

`obstack_int_grow_fast`函数添加包含*data*值的`sizeof (int)`字节到obstack *obstack-ptr*中的增长对象中。
</div>

函数：`void` **`obstack_blank_fast`** `(` `struct` `obstack` `*` *`obstack-ptr`* `,` `int` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe race:obstack-ptr | AS-Safe | AC-Safe |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

`obstack_blank_fast`函数添加*size*字节到obstack *obstack-ptr*中的增长对象中，不初始化他们。
</div>

当你用`obstack_room`检查空间，然后发现你想要添加的东西没有足够空间时，快速增长函数是不安全的。在这种情况下，在这种情况下，简单的使用对应的普通增长函数代替。很快，他会复制对象到一个新chunk；然后又会有很多可用的空间。

所以，你每次使用一个普通增长函数，后面用`obstack_room`检查不足的空间。当对象复制到一个新chunk，将会又有大量空间，所以程序又会开始使用快速增长函数。

这里有一个例子（这个例子是真牛）：

<div style="margin: 0 0 1em 2em;">

```c
void
add_string (struct obstack *obstack, const char *ptr, int len)
{
  while (len > 0)
    {
      int room = obstack_room (obstack);
      if (room == 0)
        {
          /* Not enough room.  Add one character slowly,
             which may copy to a new chunk and make room.  */
          obstack_1grow (obstack, *ptr++);
          len--;
        }
      else
        {
          if (room > len)
            room = len;
          /* Add fast as much as we have room for. */
          len -= room;
          while (room-- > 0)
            obstack_1grow_fast (obstack, *ptr++);
        }
    }
}
```
</div>

---

#### 3.2.6.8 一个obstack的状态

这里的函数提供一个obstack中的当前的分配的状态信息。你可以使用他们了解一个对象，即使正在增长。

函数：`void` `*` **`obstack_base`** `(` `struct` `obstack` `*` *`obstack-ptr`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe | AS-Unsafe corrupt | AC-Safe |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

此函数返回*obstack-ptr*中正在增长的对象的暂时的开始地址。如果你紧接着直接结束了对象，他会有那个地址。如果你先把他变得更大，他可能会超出当前chunk——那么他的地址会改变！
</div>

<div style="margin: 0 0 1em 2em;">

如果没有对象在增长，此值会告诉你，你分配的下一个对象在哪里开始（再次假设他在当前chunk中装得下）。
</div>

函数：`void` `*` **`obstack_next_free`** `(` `struct` `obstack` `*` *`obstack-ptr`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe | AS-Unsafe corrupt | AC-Safe |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

此函数返回obstack *obstack-ptr*的当前chunk中的第一个空闲字节的地址。这是当前增长对象的末端。如果没有对象在增长，`obstack_next_free`和`obstack_base`返回的值相同。
</div>

函数：`int` **`obstack_object_size`** `(` `struct` `obstack` `*` *`obstack-ptr`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe race:obstack-ptr | AS-Safe | AC-Safe |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

此函数返回当前增长对象的大小，单位字节。这等价于
</div>

<div style="margin: 0 0 1em 4em;">

```c
obstack_next_free (obstack-ptr) - obstack_base (obstack-ptr)
```
</div>

---

#### 3.2.6.9 obstack中数据的对齐

每个obstack都有一个对齐边界；obstack中分配的每个对象自动的在指定边界的整数倍的地址上开始。默认情况下，边界是对齐的，所以对象可以存储任何数据类型。

用`obstack_alignment_mask`宏，访问一个obstack的对齐边界，函数原型看上去像这样：

宏：`int` **`obstack_alignment_mask`** `(` `struct` `obstack` `*` *`obstack-ptr`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe | AS-Safe | AC-Safe |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

值是一个位掩码；一位为1表示一个对象的地址中对应的位应该为0。掩码值应该是比一个2的幂少一；效果是所有对象地址是2的幂的倍数。掩码的默认值是一个允许对齐对象存储任何数据类型的值：例如，如果他的值为3（二进制下就是0011），任何数据类型可以被存储在地址为4的倍数的地方。一个掩码的值为0意味着一个对象可以在任何1的倍数上开始（也就是，没有对齐需求）。
</div>

<div style="margin: 0 0 1em 2em;">

`obstack_alignment_mask`宏的展开是一个左值，所以你可以通过赋值调整掩码。例如，以下语句：
</div>

<div style="margin: 0 0 1em 4em;">

```c
obstack_alignment_mask (obstack_ptr) = 0;
```
</div>

<div style="margin: 0 0 1em 2em;">
效果是在指定obstack中关闭对齐处理。
</div>

注意，在对齐掩码中的改变在下一次obstack中的一个对象被分配或结束之后才会生效。如果你没有在增长一个对象，你可以调用`obstack_finish`使新的对齐掩码立刻生效。这会结束一个长度为零的对象，然后为下一个对象做适当的对齐。

---

#### 3.2.6.10 obstack chunk

obstack运行原理是给他们自己分配大chunk的空间，然后将这些chunk中的空间分割出来以满足你的请求。chunk一般是4096字节长，除非你指定了一个不同的chunk大小。chunk大小包含8字节的开销，他没有用来存储对象。无论指定的大小是多少，当需要容纳长对象时，更长的chunk会被分配。

obstack库通过调用`obstack_chunk_alloc`函数分配chunk，你必须定义他。当一个chunk因为你释放了其中所有的对象而不再需要时，obstack库通过调用`obstack_chunk_free`函数释放chunk，你也必须定义他。

这两个在每个使用`obstack_init`的源文件中必须被定义（作为宏）或被声明（作为函数）（参考[Creating Obstacks](https://sourceware.org/glibc/manual/latest/html_node/Creating-Obstacks.html)）。最常见的情况是，他们被定义成宏，像这样：

<div style="margin: 0 0 1em 2em;">

```c
#define obstack_chunk_alloc malloc
#define obstack_chunk_free free
```
</div>

注意，这些是简单宏（无参数）。带有参数的宏定义不能用！`obstack_chunk_alloc`或`obstack_chunk_free`必须各自展开成一个函数名，如果他们本身不是一个函数名的话。

如果你用`malloc`分配chunk，chunk大小必须是2的幂。默认chunk大小是4096，选他的原因是他够长，满足obstack中许多典型的请求，同时又足够短，不会在最后一个块中未使用的部分里浪费太多内存。

宏：`int` **`obstack_chunk_size`** `(` `struct` `obstack` `*` *`obstack-ptr`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe | AS-Safe | AC-Safe |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

他返回传入的obstack的chunk大小。
</div>

因为宏展开是一个左值，你可以通过赋给他一个新值拉指定一个新chunk大小。这样做不会影响到已经分配的chunk，但是会改变未来在特定obstack中分配的chunk的大小。是chunk大小更小不太可能有用，但是使他变大可能会增加性能，如果你分配了很多对象，他们的大小和chunk大小相当。这里是干净利落的做法：

<div style="margin: 0 0 1em 2em;">

```c
if (obstack_chunk_size (obstack_ptr) < new-chunk-size)
  obstack_chunk_size (obstack_ptr) = new-chunk-size;
```
</div>

---

#### 3.2.6.11 obstack函数总结

这里是所有与obstack相关的函数的总结。每个都会接收一个obstack的地址（`struct` `obstack` `*`）作为他的第一个参数。

`void` `obstack_init` `(` `struct` `obstack` `*` *`obstack-ptr`* `)`

<div style="margin: 0 0 1em 2em;">

初始化一个obstack的使用。参考[Creating Obstacks](https://sourceware.org/glibc/manual/latest/html_node/Creating-Obstacks.html)。
</div>

`void` `*` `obstack_alloc` `(` `struct` `obstack` `*` *`obstack-ptr`* `,` `int` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

分配一个*size*未初始化字节的对象。参考[Allocation in an Obstack](https://sourceware.org/glibc/manual/latest/html_node/Allocation-in-an-Obstack.html)。
</div>

`void` `*` `obstack_copy` `(` `struct` `obstack` `*` *`obstack-ptr`* `,` `void` `*` *`address`* `,` `int` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

分配一个*size*字节的对象，内容从*address*中复制。参考[Allocation in an Obstack](https://sourceware.org/glibc/manual/latest/html_node/Allocation-in-an-Obstack.html)。
</div>

`void` `*` `obstack_copy0` `(` `struct` `obstack` `*` *`obstack-ptr`* `,` `void` `*` *`address`* `,` `int` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

分配*size*+1字节的对象，从*address*中复制*size*的他们，最后接一个空字符。参考[Allocation in an Obstack](https://sourceware.org/glibc/manual/latest/html_node/Allocation-in-an-Obstack.html)。
</div>

`void` `obstack_free` `(` `struct` `obstack` `*` *`obstack-ptr`* `,` `void` `*` *`object`* `)`

<div style="margin: 0 0 1em 2em;">

释放*object*（还有在指定obstack中所有比*object*更晚分配的东西）。参考[Freeing Objects in an Obstack](https://sourceware.org/glibc/manual/latest/html_node/Freeing-Obstack-Objects.html)。
</div>

`void` `obstack_blank` `(` `struct` `obstack` `*` *`obstack-ptr`* `,` `int` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

添加*size*的未初始化的字节到一个增长对象中。参考[Growing Objects](https://sourceware.org/glibc/manual/latest/html_node/Growing-Objects.html)。
</div>

`void` `obstack_grow` `(` `struct` `obstack` `*` *`obstack-ptr`* `,` `void` `*` *`address`* `,` `int` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

添加*size*字节到一个增长对象中，从*address*中复制。参考[Growing Objects](https://sourceware.org/glibc/manual/latest/html_node/Growing-Objects.html)。
</div>

`void` `obstack_grow0` `(` `struct` `obstack` `*` *`obstack-ptr`* `,` `void` `*` *`address`* `,` `int` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

添加*size*字节到一个增长对象中，从*address*中复制，然后添加另一个包含一个空字符的字节。参考[Growing Objects](https://sourceware.org/glibc/manual/latest/html_node/Growing-Objects.html)。
</div>

`void` `obstack_1grow` `(` `struct` `obstack` `*` *`obstack-ptr`* `,` `char` *`data-char`* `)`

<div style="margin: 0 0 1em 2em;">

添加一个包含*data-char*的字节到一个增长对象。参考[Growing Objects](https://sourceware.org/glibc/manual/latest/html_node/Growing-Objects.html)。
</div>

`void` `*` `obstack_finish` `(` `struct` `obstack` `*` *`obstack-ptr`* `)`

<div style="margin: 0 0 1em 2em;">

结束增长对象，并返回他的永久地址。参考[Growing Objects](https://sourceware.org/glibc/manual/latest/html_node/Growing-Objects.html)。
</div>

`int` `obstack_object_size` `(` `struct` `obstack` `*` *`obstack-ptr`* `)`

<div style="margin: 0 0 1em 2em;">

获取当前正在增长的对象的大小。参考[Growing Objects](https://sourceware.org/glibc/manual/latest/html_node/Growing-Objects.html)。
</div>

`void` `obstack_blank_fast` `(` `struct` `obstack` `*` *`obstack-ptr`* `,` `int` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

添加*size*的未初始化字节到一个增长对象，不检查是否有足够的空间。参考[Extra Fast Growing Objects](https://sourceware.org/glibc/manual/latest/html_node/Extra-Fast-Growing.html)。
</div>

`void` `obstack_1grow_fast` `(` `struct` `obstack` `*` *`obstack-ptr`* `,` `char` *`data-char`* `)`

<div style="margin: 0 0 1em 2em;">

添加一个包含*data-char*的字节到到一个增长对象，不检查是否有足够的空间。参考[Extra Fast Growing Objects](https://sourceware.org/glibc/manual/latest/html_node/Extra-Fast-Growing.html)。
</div>

`int` `obstack_room` `(` `struct` `obstack` `*` *`obstack-ptr`* `)`

<div style="margin: 0 0 1em 2em;">

获取当前对象可以用来增长的空间。参考[Extra Fast Growing Objects](https://sourceware.org/glibc/manual/latest/html_node/Extra-Fast-Growing.html)。
</div>

`int` `obstack_alignment_mask` `(` `struct` `obstack` `*` *`obstack-ptr`* `)`

<div style="margin: 0 0 1em 2em;">

用来对齐一个对象的开头的掩码。这是一个左值。参考[Alignment of Data in Obstacks](https://sourceware.org/glibc/manual/latest/html_node/Obstacks-Data-Alignment.html)。
</div>

`int` `obstack_chunk_size` `(` `struct` `obstack` `*` *`obstack-ptr`* `)`

<div style="margin: 0 0 1em 2em;">

分配chunk的大小。这是一个左值。参考[Obstack Chunks](https://sourceware.org/glibc/manual/latest/html_node/Obstack-Chunks.html)。
</div>

`void` `*` `obstack_base` `(` `struct` `obstack` `*` *`obstack-ptr`* `)`

<div style="margin: 0 0 1em 2em;">

正在增长的对象的临时开始地址。参考[Status of an Obstack](https://sourceware.org/glibc/manual/latest/html_node/Status-of-an-Obstack.html)。
</div>

`void` `*` `obstack_next_free` `(` `struct` `obstack` `*` *`obstack-ptr`* `)`

<div style="margin: 0 0 1em 2em;">

正在增长的对象的末端地址。参考[Status of an Obstack](https://sourceware.org/glibc/manual/latest/html_node/Status-of-an-Obstack.html)。
</div>

---

### 3.2.7 可变大小的自动存储

`alloca`函数支持一种半动态的分配方式，块是动态分配的，但是自动释放的。

使用`alloca`分配块是一个显式操作；你可以想分配多少块就分配多少，并在运行时计算大小。但是当你退出调用了`alloca`的函数时，所有块都被释放，就像他们是在他个函数中的自动声明变量一样。无法显式的释放空间。

`alloca`的原型在`stdlib.h`中。此函数是一个BSD扩展。

函数：`void` `*` **`alloca`** `(` `size_t` *`size`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe | AS-Safe | AC-Safe |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

`alloca`的返回值是一个*size*字节的内存块的地址，分配在调用函数中的栈帧中。
</div>

不要在一个函数调用的参数中使用`alloca`——你会获得无法预测的结果，因为`alloca`的栈空间会出现在函数参数的空间的中间的栈上。需要避免的一个例子是`foo (x, alloca (4), y)`。

- [`alloca` Example](https://sourceware.org/glibc/manual/latest/html_node/Alloca-Example.html)

- [Advantages of `alloca`](https://sourceware.org/glibc/manual/latest/html_node/Advantages-of-Alloca.html)

- [Disadvantages of `alloca`](https://sourceware.org/glibc/manual/latest/html_node/Disadvantages-of-Alloca.html)

- [GNU C Variable-Size Arrays](https://sourceware.org/glibc/manual/latest/html_node/GNU-C-Variable_002dSize-Arrays.html)

---

#### 3.2.7.1 `alloca`例子

As an example of the use of `alloca`, here is a function that opens a file name made from concatenating two argument strings, and returns a file descriptor or minus one signifying failure:
作为一个`alloca`的使用例子，这里是一个函数，他打开一个文件，文件名由两个字符串参数拼接而成，然后返回一个文件描述符，或负一，表示失败。

<div style="margin: 0 0 1em 2em;">

```c
int
open2 (char *str1, char *str2, int flags, int mode)
{
  char *name = (char *) alloca (strlen (str1) + strlen (str2) + 1);
  stpcpy (stpcpy (name, str1), str2);
  return open (name, flags, mode);
}
```
AI生成：stpcpy是字符串复制函数，它返回指向目标字符串末尾（即结尾的空字符）的指针。
</div>

这是你如何用`malloc`和`free`获得相同结果：

<div style="margin: 0 0 1em 2em;">

```c
int
open2 (char *str1, char *str2, int flags, int mode)
{
  char *name = malloc (strlen (str1) + strlen (str2) + 1);
  int desc;
  if (name == 0)
    fatal ("virtual memory exceeded");
  stpcpy (stpcpy (name, str1), str2);
  desc = open (name, flags, mode);
  free (name);
  return desc;
}
```
</div>

如你所见，使用`alloca`更简单。但是`alloca`由其他更重要的优点，和一些缺点。

---

#### 3.2.7.2 `alloca`的优点

`alloca`比`malloc`更好的原因：

- 使用`alloca`浪费很少空间，并且非常快。（他由GNU C编译器展开为内联代码。）

- 因为`alloca`没用为不同大小的块区分单独的池，所以任何大小块的空间可以被其他任何大小再次使用。`alloca`不会造成内存碎片。

- 使用`longjmp`（参考[Non-Local Exits](https://sourceware.org/glibc/manual/latest/html_node/Non_002dLocal-Exits.html)）的非局部退出在他们退出调用`alloca`的函数时，自动释放`alloca`分配的空间。这是使用`alloca`最重要的原因。

<div style="margin: 0 0 1em 2em;">

为了说明这个，加入你有一个函数`open_or_report_error`会在成功时，像`open`，返回一个描述符，但是，在失败时，不会返回到他的调用者。如果文件无法打开，他打印错误信息，然后使用`longjmp`跳转到你程序的命令层。让我们修改`open2`（参考[alloca Example](https://sourceware.org/glibc/manual/latest/html_node/Alloca-Example.html)来使用次子程序：
</div>

<div style="margin: 0 0 1em 4em;">

```c
int
open2 (char *str1, char *str2, int flags, int mode)
{
  char *name = (char *) alloca (strlen (str1) + strlen (str2) + 1);
  stpcpy (stpcpy (name, str1), str2);
  return open_or_report_error (name, flags, mode);
}
```
</div>

<div style="margin: 0 0 1em 2em;">

由于`alloca`的工作方式，即使一个错误发生了，他分配的内存也会释放，无需特别的操作。
</div>

<div style="margin: 0 0 1em 2em;">

对比看来，前面的`open2`定义（使用`malloc`和`free`的那个），如果按照这种方式修改，就会导致一个内存泄露。就算你愿意做出更多改变来修复他，也没有什么更简单的方式来做到相同的事。
</div>

---

#### 3.2.7.3 `alloca`的缺点

`alloca`与`malloc`相比的缺点：

- 如果你尝试分配比机器能提供的还更多的内存，你不会获得一条清晰的错误信息。相反，你会获得一个错误信号，就像你会从一个无限递归中获得的一样；可能是一个段错误（segmentation violation）（参考[Program Error Signals](https://sourceware.org/glibc/manual/latest/html_node/Program-Error-Signals.html)）。

- 一些非GNU系统不支持`alloca`，所以可移植性更差。然而，对于存在这种缺陷的系统，可以使用一个用C编写的较慢的`alloca`仿真实现。

---

#### 3.2.7.4 GNU C变长数组

GNU C中，你可以用变长数组替换大部分`alloca`的用法。那么`open2`看上去是这样的：

<div style="margin: 0 0 1em 4em;">

```c
int open2 (char *str1, char *str2, int flags, int mode)
{
  char name[strlen (str1) + strlen (str2) + 1];
  stpcpy (stpcpy (name, str1), str2);
  return open (name, flags, mode);
}
```
</div>

但是`alloca`并不总是等于一个变长数组，原因：

- 一个变长数组的空间在数组名称的作用域结束时释放。而`alloca`分配的空间会一直保留到函数结束。

- 可以在循环中使用`alloca`，每次迭代分配一个额外的块。变长数组不可能做到。

<strong>注意：</strong>如果你在一个函数中混合使用`alloca`和变长数组，退出声明了一个变长数组的一个作用域时，也会释放该作用域中使用`alloca`分配的所有块。

---

## 3.3 调整数据段大小

本节的符号在`unistd.h`中声明。

你通常不会使用本节中的函数，因为[Allocating Storage For Program Data](https://sourceware.org/glibc/manual/latest/html_node/Memory-Allocation.html)中描述的函数更易使用。那些（下面的函数）是一个GNU C Library内存分配器本身使用的函数接口。下面的函数是简单的对系统调用的接口。

函数：`int` **`brk`** `(` `void` `*` *`addr`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe | AS-Safe | AC-Safe |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

`brk`将调用进程数据段的高端设置成*addr*。
</div>

<div style="margin: 0 0 1em 2em;">

段末端的地址被定义为该段最后一个字节的地址加1。
</div>

<div style="margin: 0 0 1em 2em;">

如果*addr*比数据段的低端更低，函数没有效果。（随便提一下，这视为成功。）
</div>

<div style="margin: 0 0 1em 2em;">

若他的数据段与另一个段重叠，或超出进程数据存储限制（参考[Limiting Resource Usage](https://sourceware.org/glibc/manual/latest/html_node/Limits-on-Resources.html)）。
</div>

<div style="margin: 0 0 1em 2em;">

函数的命名源自于一个常见的历史场景，数据存储和栈在同一个段中。数据存储分配从段的底部向上增长，栈从段的顶部向下增长，他们之间的分界线称为break。
</div>

<div style="margin: 0 0 1em 2em;">

成功时，返回值是零。失败时，返回值是`-1`，并且对应的设置`errno`。下面的`errno`指定用于对此函数：
</div>

<div style="margin: 0 0 1em 2em;">

`ENOMEM`
</div>

<div style="margin: 0 0 1em 4em;">

请求会导致数据段与另一个段重叠，或超出进程数据存储限制。
</div>

函数：`void` `*` **`sbrk`** `(` `ptrdiff_t` *`delta`* `)`
</div>

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe | AS-Safe | AC-Safe |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

This function is the same as `brk` except that you specify the new end of the data segment as an offset *delta* from the current end and on success the return value is the address of the resulting end of the data segment instead of zero.
</div>

<div style="margin: 0 0 1em 2em;">

This means you can use ‘`sbrk(0)`’ to find out what the current end of the data segment is.
</div>