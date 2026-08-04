这是The GNU C Library Reference Manual，版本2.44。

Copyright © 1993–2026 Free Software Foundation, Inc.

遵守the terms of the GNU Free Documentation License, Version 1.3 or any later version published by the Free Software Foundation时，以下内容是许可的，复制，分发，修改这个文档；不能修改“Free Software Needs Free Documentation” and “GNU Lesser General Public License”章节，封面文字必须是“A GNU Manual”，封底文字必须是下面(a)的内容。本许可证的副本包含在题为“GNU Free Documentation License”的章节中。

(a) The FSF’s Back-Cover Text is: “You have the freedom to copy and modify this GNU manual. Buying copies from the FSF supports it in developing GNU and promoting software freedom.”

# 2 错误报告

许多the GNU C Library的函数会检测和报告错误情况，有时，你的程序也需要检查错误情况。例如，当你打开一个输入文件，你应该验证那个文件是否被正确打开了，并且，如果调用库函数失败，打印错误信息并采取其他适当的处理措施。

这个章节描述了错误报告功能是如何运作的。引入`errno.h`以使用这类功能。

- [Checking for Errors](https://sourceware.org/glibc/manual/latest/html_node/Checking-for-Errors.html)

- [Error Codes](https://sourceware.org/glibc/manual/latest/html_node/Error-Codes.html)

- [Error Messages](https://sourceware.org/glibc/manual/latest/html_node/Error-Messages.html)

## 2.1 错误检查

大多数库函数会返回一个特定值，表明他们失败了。该特定值通常为`-1`，或一个空指针，或一个常量，比如转为该目的定义的`EOF`。但是这个返回值只能告诉你有错误。要知道是哪种错误，你需要查找存储在变量`errno`中的错误码。该变量在头文件`errno.h`中声明。

变量：`volatile` `int` **`errno`**

<div style="margin: 0 0 1em 3em;">

变量`errno`保存了系统错误码。你可以修改`errno`的值。
</div>

<div style="margin: 0 0 1em 3em;">

因为`errno`被`volatile`修饰，他可能会被信号处理器异步的修改；参考[Defining Signal Handlers](https://sourceware.org/glibc/manual/latest/html_node/Defining-Handlers.html)。然而，编写规范的信号处理器会保存并恢复`errno`的值，因此除了编写信号处理器本身之外，通常无需担心这一问题。
</div>

<div style="margin: 0 0 1em 3em;">

程序启动时，`errno`的初始值为0。在多数情况下，当库函数遇到错误时，他会将`error`设置为非0值，以指出发生了什么错误。每个函数的文档中都列出了可能的错误条件。不是所有库函数都使用这个机制；有些函数直接返回错误码。
</div>

<div style="margin: 0 0 1em 3em;">

<strong>警告：</strong>许多库函数即使未遇到错误，甚至在其直接返回错误码的情况下，也会将`errno`设置为某个无意义的非零值。因此，通常不能通过检查`errno`的值来判断是否发生错误。正确的检查错误的方法在每个函数的文档说明中。
</div>

<div style="margin: 0 0 1em 3em;">

<strong>可移植性声明：</strong>ISO C规定了`errno`为“可修改的左值”，而非变量，这允许用宏实现他。例如，他的宏展开可能包含函数调用，比如`*__errno_location ()`。事实上，GNU/Linux和GNU/Hurd系统上就是这样的。The GNU C Library，会根据不同的系统使用不同的实现方式。
</div>

<div style="margin: 0 0 1em 3em;">

有些库函数，比如`sqrt`和`atan`，在发生错误时仍然会返回一个完全合法的值，但同时也会设置`errno`。关于这些函数，建议调用前将`errno`设置为0，然后再检查他的值。
</div>

所有错误码都有符号名称；他们在`errno.h`中被定义。错误码以‘`E`’和一个大写字母或数字开头；注意这种形式为保留名称。参考[Reserved Names](https://sourceware.org/glibc/manual/latest/html_node/Reserved-Names.html)。

错误码都是正数，并且都不同，所以他们可以被用为`switch`语句的标签。你的程序不应该对这些符号常量的具体数值作任何其他假设。该规则有两个例外：

- 在所有操作系统上，`EAGAIN`和`EWOULDBLOCK`的值是相等的。

- 在某些系统上，`ENOTSUP`和`EOPNOTSUPP`的值是相等的，比如GNU/Linux。

为了让你的程序可移植，你应当同时检查这两个错误码，并将他们一视同仁的处理。

`errno`的值可能不对应任何一个宏，因为有些库函数会根据自己的情况，返回其他错误码。对于某个特定的库函数而言，唯一能确保有意义的值，就是本手册中为该函数列出的那些错误码。

除了在GNU/Hurd系统上，几乎所有的系统调用接受了无效指针作为参数，都可以返回`EFAULT`。由于这种情况只可能是由你程序中的bug所导致的，并且在GNU/Hurd上不会发生，因此为了节省篇幅，我们在各个函数的单独描述中不在逐一提及`EFAULT`。

在某些Unix系统上，很多系统调用接收了指向栈的指针也都会返回`EFAULT`，而内核出于某种晦涩难明的原因在试图扩展栈时失败了。万一真遇到这种情况，在该系统上你或许应当改用静态分配或动态分配的内存，而不要使用栈内存。

## 2.2 Error Codes

错误码宏定义在头文件`errno.h`中。他们全都展开（这个展开指的是宏被替换成宏对应的式子）为整数常量。一些错误码在GNU系统上不会出现，但是他们可能在使用the GNU C Library的其他系统上出现。

宏：`int` **`EPERM`**

<div style="margin: 0 0 1em 3em;">

“Operation not permitted.”只有文件（或者其他资源）的拥有者或具有特殊权限的进程才能执行该操作。
</div>

宏：`int` **`ENOENT`**

<div style="margin: 0 0 1em 3em;">

“No such file or directory.”这是一个“文件不存在”错误，在预期文件已存在的情况下，引用该文件，而该文件不存在。
</div>

宏：`int` **`ESRCH`**

<div style="margin: 0 0 1em 3em;">

“No such process.”找某个具体进程ID时，发现没有匹配的进程。
</div>

宏：`int` **`EINTR`**

<div style="margin: 0 0 1em 3em;">

“Interrupted system call.”一个异步信号阻止了调用的完成。当这种事发生了，你应该再调用一次。
</div>

<div style="margin: 0 0 1em 3em;">

你可以选择用一个程序在被一个异步信号中断后继续运行，而不是失败并EINTR；参考[Primitives Interrupted by Signals](https://sourceware.org/glibc/manual/latest/html_node/Interrupted-Primitives.html)。
</div>

宏：`int` **`EIO`**

<div style="margin: 0 0 1em 3em;">

“Input/output error.”通常用于物理读写错误。
</div>

宏：`int` **`ENXIO`**

<div style="margin: 0 0 1em 3em;">

系统尝试使用你所指定的文件所代表的设备，但是未能找到该设备。这可能意味着设备文件安装不正确，或者该物理设备缺失或未正确连接到计算机。
</div>

宏：`int` **`E2BIG`**

<div style="margin: 0 0 1em 3em;">

“Argument list too long.”当通过某个exec函数（参考[Executing a File](https://sourceware.org/glibc/manual/latest/html_node/Executing-a-File.html)）执行新程序时，如果传入的参数占用的内存空间过大，就会使用此错误码。在GNU/Hurd系统上不会出现这种情况。
</div>

宏：`int` **`ENOEXEC`**

<div style="margin: 0 0 1em 3em;">

“Exec format error.”不可用的可执行文件格式。此情况由exec系列函数检查；参考[Executing a File](https://sourceware.org/glibc/manual/latest/html_node/Executing-a-File.html)。
</div>

宏：`int` **`EBADF`**

<div style="margin: 0 0 1em 3em;">

“Bad file descriptor.”例如，描述符的I/O已经被关闭，或者读一个以只写模式打开的描述符（反之亦然）。
</div>

宏：`int` **`ECHILD`**

<div style="margin: 0 0 1em 3em;">

“No child processes.”这个错误发生在原本想要操作子进程的操作，但是没有子进程。
</div>

宏：`int` **`EDEADLK`**

<div style="margin: 0 0 1em 3em;">

“Resource deadlock avoided.”分配一个系统资源会导致死锁情况的发生。这个系统不保证他会注意到所有这样的情况。这个错误意味着你很幸运，系统注意到了；否则程序可能直接挂起了。参考[File Locks](https://sourceware.org/glibc/manual/latest/html_node/File-Locks.html)。
</div>

宏：`int` **`ENOMEM`**

<div style="margin: 0 0 1em 3em;">

“Cannot allocate memory.”系统无法在分配更多物理内存，因为已经用完了。
</div>

宏：`int` **`EACCES`**

<div style="margin: 0 0 1em 3em;">

“Permission denied.”这个文件许可不允许该操作。
</div>

宏：`int` **`EFAULT`**

<div style="margin: 0 0 1em 3em;">

“Bad address.”检查到无效指针。在GNU/Hurd系统上，这个错误不会出现；但是，你会获得一个信号。
</div>

宏：`int` **`ENOTBLK`**

<div style="margin: 0 0 1em 3em;">

“Block device required.”在需要块特殊文件的场合中，提供了一个非块特殊文件（这句话中的文件就是指设备，Unix/Linux语境下，物理硬件设备也是文件的一种）。例如，在Unix系统中尝试将一个普通文件挂载为文件系统时，就会产生此错误。
</div>

宏：`int` **`EBUSY`**

<div style="margin: 0 0 1em 3em;">

“Device or resource busy.”一个系统资源已在使用，不能分享。例如，你尝试删除当前挂载的文件系统的根目录文件，你就会获得这个错误。
</div>

宏：`int` **`EEXIST`**

<div style="margin: 0 0 1em 3em;">

“File exists.”在只允许接收新文件的情况下（例如mkdir函数），指定了一个已存在的文件。
</div>

宏：`int` **`EXDEV`**

<div style="margin: 0 0 1em 3em;">

“Invalid cross-device link.”检测到尝试在不同文件系统之间进行不当的链接操作。这不仅发生在使用`link`（参考[Hard Links](https://sourceware.org/glibc/manual/latest/html_node/Hard-Links.html)）时，还发生在使用`rename`（参考[Renaming Files](https://sourceware.org/glibc/manual/latest/html_node/Renaming-Files.html)）来重命名文件时。
</div>

宏：`int` **`ENODEV`**

<div style="margin: 0 0 1em 3em;">

“No such device.”一个错误类型的设备传入了一个，需要某种设备的程序。
</div>

宏：`int` **`ENOTDIR`**

<div style="margin: 0 0 1em 3em;">

“Not a directory.”在只允许接收文件夹的情况下，指定了一个已非文件夹的文件。
</div>

宏：`int` **`EISDIR`**

<div style="margin: 0 0 1em 3em;">

“Is a directory.”你不能以写入模型打开一个目录，或者创建或移除他的硬链接（硬链接有点类似桌面快捷方式，不过不能对目录生效）。
</div>

宏：`int` **`EINVAL`**

<div style="margin: 0 0 1em 3em;">

“Invalid argument.”这用来指出各种各样传入错误参数到库函数的问题。
</div>

宏：`int` **`EMFILE`**

<div style="margin: 0 0 1em 3em;">

“Too many open files.”当前进程打开了太多文件，不能再打开文件了。重复的文件描述符也会计入此限制。
</div>

<div style="margin: 0 0 1em 3em;">

在BSD和GNU中，打开文件的数量是由一个资源限制控制，通常可以提高该限制。如果遇到此错误，你可以提高`RLIMIT_NOFILE`限制，或设置他为无限制；参考[Limiting Resource Usage](https://sourceware.org/glibc/manual/latest/html_node/Limits-on-Resources.html)。
</div>

宏：`int` **`ENFILE`**

<div style="margin: 0 0 1em 3em;">

“Too many open files in system.”在整个系统中，打开了太多不同的文件。注意，任意数量的链接通道仅记为一次文件打开；参考[Linked Channels](https://sourceware.org/glibc/manual/latest/html_node/Linked-Channels.html)。在GNU/Hurd系统上，这个错误不会出现。
</div>

宏：`int` **`ENOTTY`**

<div style="margin: 0 0 1em 3em;">

“Inappropriate ioctl for device.”表示执行了不适用的I/O控制操作，例如试图对普通文件设置终端模式。
</div>

宏：`int` **`ETXTBSY`**

<div style="margin: 0 0 1em 3em;">

“Text file busy.”表示尝试执行一个当前以写模式打开的文件，或者试图写入一个当前正在被执行的文件。使用调试器运行程序时，通常也被视为以写模式打开了该文件，因此也会引发此错误。（字面意思“文本文件忙”。）在GNU/Hurd系统上，这不是错误；系统会在必要时自动复制文本段。
</div>

宏：`int` **`EFBIG`**

<div style="margin: 0 0 1em 3em;">

“File too large.”文件大小超过系统允许的范围。
</div>

宏：`int` **`ENOSPC`**

<div style="margin: 0 0 1em 3em;">

“No space left on device.”写文件失败，因为磁盘满了。
</div>

宏：`int` **`ESPIPE`**

<div style="margin: 0 0 1em 3em;">

“Illegal seek.”无效的查找（seek）操作（例如在管道上执行了该操作）。
</div>

宏：`int` **`EROFS`**

<div style="margin: 0 0 1em 3em;">

“Read-only file system.”尝试在只读系统上修改某些东西。
</div>

宏：`int` **`EMLINK`**

<div style="margin: 0 0 1em 3em;">


“Too many links.”表示单个文件的硬链接数量将达到系统允许的上限。当对一个已经达到最大链接数的文件`rename`时，也可能触发此错误（参考[Renaming Files](https://sourceware.org/glibc/manual/latest/html_node/Renaming-Files.html)）。
</div>

宏：`int` **`EPIPE`**

<div style="margin: 0 0 1em 3em;">

“Broken pipe.”pipe的另一端没有进程在读取数据。所有报这个错误码的库函数同时也会生成一个`SIGPIPE`信号；如果这个信号没有被处理或屏蔽，这个信号会终端程序。因此，除非你的程序已经对`SIGPIPE`进行了处理或屏蔽，否则实际上永远不会收到`EPIPE`错误。
</div>

宏：`int` **`EDOM`**

<div style="margin: 0 0 1em 3em;">

“Numerical argument out of domain.”数学函数的参数值不在该函数的定义域内。
</div>

宏：`int` **`ERANGE`**

<div style="margin: 0 0 1em 3em;">

“Numerical result out of range.”数学函数的计算结果因上溢或下溢而无法表示。
</div>

Macro: int EAGAIN
“Resource temporarily unavailable.” The call might work if you try again later.

This error can happen in a few different situations:

An operation that would block was attempted on an object that has non-blocking mode selected. Trying the same operation again will block until some external condition makes it possible to read, write, or connect (whatever the operation). You can use select to find out when the operation will be possible; see Waiting for Input or Output.
A temporary resource shortage made an operation impossible. fork can return this error. It indicates that the shortage is expected to pass, so your program can try the call again later and it may succeed. It is probably a good idea to delay for a few seconds before trying it again, to allow time for other processes to release scarce resources. Such shortages are usually fairly serious and affect the whole system, so usually an interactive program should report the error to the user and return to its command loop.
Portability Note: In the GNU C Library, EAGAIN and EWOULDBLOCK are equal. Portable code should check for both errors and treat them the same.