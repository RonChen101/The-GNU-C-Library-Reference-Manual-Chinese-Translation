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

<div style="margin: 0 0 1em 2em;">

变量`errno`保存了系统错误码。你可以修改`errno`的值。
</div>

<div style="margin: 0 0 1em 2em;">

因为`errno`被`volatile`修饰，他可能会被信号处理器异步的修改；参考[Defining Signal Handlers](https://sourceware.org/glibc/manual/latest/html_node/Defining-Handlers.html)。然而，编写规范的信号处理器会保存并恢复`errno`的值，因此除了编写信号处理器本身之外，通常无需担心这一问题。
</div>

<div style="margin: 0 0 1em 2em;">

程序启动时，`errno`的初始值为0。在多数情况下，当库函数遇到错误时，他会将`error`设置为非0值，以指出发生了什么错误。每个函数的文档中都列出了可能的错误条件。不是所有库函数都使用这个机制；有些函数直接返回错误码。
</div>

<div style="margin: 0 0 1em 2em;">

<strong>警告：</strong>许多库函数即使未遇到错误，甚至在其直接返回错误码的情况下，也会将`errno`设置为某个无意义的非零值。因此，通常不能通过检查`errno`的值来判断是否发生错误。正确的检查错误的方法在每个函数的文档说明中。
</div>

<div style="margin: 0 0 1em 2em;">

<strong>可移植性声明：</strong>ISO C规定了`errno`为“可修改的左值”，而非变量，这允许用宏实现他。例如，他的宏展开可能包含函数调用，比如`*__errno_location ()`。事实上，GNU/Linux和GNU/Hurd系统上就是这样的。The GNU C Library，会根据不同的系统使用不同的实现方式。
</div>

<div style="margin: 0 0 1em 2em;">

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

## 2.2 错误码

错误码宏定义在头文件`errno.h`中。他们全都展开（这个展开指的是宏被替换成宏对应的式子）为整数常量。一些错误码在GNU系统上不会出现，但是他们可能在使用the GNU C Library的其他系统上出现。

宏：`int` **`EPERM`**

<div style="margin: 0 0 1em 2em;">

“Operation not permitted.”只有文件（或者其他资源）的拥有者或具有特殊权限的进程才能执行该操作。
</div>

宏：`int` **`ENOENT`**

<div style="margin: 0 0 1em 2em;">

“No such file or directory.”这是一个“文件不存在”错误，在预期文件已存在的情况下，引用该文件，而该文件不存在。
</div>

宏：`int` **`ESRCH`**

<div style="margin: 0 0 1em 2em;">

“No such process.”找某个具体进程ID时，发现没有匹配的进程。
</div>

宏：`int` **`EINTR`**

<div style="margin: 0 0 1em 2em;">

“Interrupted system call.”一个异步信号阻止了调用的完成。当这种事发生了，你应该再调用一次。
</div>

<div style="margin: 0 0 1em 2em;">

你可以选择用一个程序在被一个异步信号中断后继续运行，而不是失败并EINTR；参考[Primitives Interrupted by Signals](https://sourceware.org/glibc/manual/latest/html_node/Interrupted-Primitives.html)。
</div>

宏：`int` **`EIO`**

<div style="margin: 0 0 1em 2em;">

“Input/output error.”通常用于物理读写错误。
</div>

宏：`int` **`ENXIO`**

<div style="margin: 0 0 1em 2em;">

系统尝试使用你所指定的文件所代表的设备，但是未能找到该设备。这可能意味着设备文件安装不正确，或者该物理设备缺失或未正确连接到计算机。
</div>

宏：`int` **`E2BIG`**

<div style="margin: 0 0 1em 2em;">

“Argument list too long.”当通过某个exec函数（参考[Executing a File](https://sourceware.org/glibc/manual/latest/html_node/Executing-a-File.html)）执行新程序时，如果传入的参数占用的内存空间过大，就会使用此错误码。在GNU/Hurd系统上不会出现这种情况。
</div>

宏：`int` **`ENOEXEC`**

<div style="margin: 0 0 1em 2em;">

“Exec format error.”不可用的可执行文件格式。此情况由exec系列函数检查；参考[Executing a File](https://sourceware.org/glibc/manual/latest/html_node/Executing-a-File.html)。
</div>

宏：`int` **`EBADF`**

<div style="margin: 0 0 1em 2em;">

“Bad file descriptor.”例如，描述符的I/O已经被关闭，或者读一个以只写模式打开的描述符（反之亦然）。
</div>

宏：`int` **`ECHILD`**

<div style="margin: 0 0 1em 2em;">

“No child processes.”这个错误发生在原本想要操作子进程的操作，但是没有子进程。
</div>

宏：`int` **`EDEADLK`**

<div style="margin: 0 0 1em 2em;">

“Resource deadlock avoided.”分配一个系统资源会导致死锁情况的发生。这个系统不保证他会注意到所有这样的情况。这个错误意味着你很幸运，系统注意到了；否则程序可能直接挂起了。参考[File Locks](https://sourceware.org/glibc/manual/latest/html_node/File-Locks.html)。
</div>

宏：`int` **`ENOMEM`**

<div style="margin: 0 0 1em 2em;">

“Cannot allocate memory.”系统无法在分配更多物理内存，因为已经用完了。
</div>

宏：`int` **`EACCES`**

<div style="margin: 0 0 1em 2em;">

“Permission denied.”这个文件许可不允许该操作。
</div>

宏：`int` **`EFAULT`**

<div style="margin: 0 0 1em 2em;">

“Bad address.”检查到无效指针。在GNU/Hurd系统上，这个错误不会出现；但是，你会获得一个信号。
</div>

宏：`int` **`ENOTBLK`**

<div style="margin: 0 0 1em 2em;">

“Block device required.”在需要块特殊文件的场合中，提供了一个非块特殊文件（这句话中的文件就是指设备，Unix/Linux语境下，物理硬件设备也是文件的一种）。例如，在Unix系统中尝试将一个普通文件挂载为文件系统时，就会产生此错误。
</div>

宏：`int` **`EBUSY`**

<div style="margin: 0 0 1em 2em;">

“Device or resource busy.”一个系统资源已在使用，不能分享。例如，你尝试删除当前挂载的文件系统的根目录文件，你就会获得这个错误。
</div>

宏：`int` **`EEXIST`**

<div style="margin: 0 0 1em 2em;">

“File exists.”在只允许接收新文件的情况下（例如mkdir函数），指定了一个已存在的文件。
</div>

宏：`int` **`EXDEV`**

<div style="margin: 0 0 1em 2em;">

“Invalid cross-device link.”检测到尝试在不同文件系统之间进行不当的链接操作。这不仅发生在使用`link`（参考[Hard Links](https://sourceware.org/glibc/manual/latest/html_node/Hard-Links.html)）时，还发生在使用`rename`（参考[Renaming Files](https://sourceware.org/glibc/manual/latest/html_node/Renaming-Files.html)）来重命名文件时。
</div>

宏：`int` **`ENODEV`**

<div style="margin: 0 0 1em 2em;">

“No such device.”一个错误类型的设备传入了一个，需要某种设备的程序。
</div>

宏：`int` **`ENOTDIR`**

<div style="margin: 0 0 1em 2em;">

“Not a directory.”在只允许接收文件夹的情况下，指定了一个已非文件夹的文件。
</div>

宏：`int` **`EISDIR`**

<div style="margin: 0 0 1em 2em;">

“Is a directory.”你不能以写入模型打开一个目录，或者创建或移除他的硬链接（硬链接有点类似桌面快捷方式，不过不能对目录生效）。
</div>

宏：`int` **`EINVAL`**

<div style="margin: 0 0 1em 2em;">

“Invalid argument.”这用来指出各种各样传入错误参数到库函数的问题。
</div>

宏：`int` **`EMFILE`**

<div style="margin: 0 0 1em 2em;">

“Too many open files.”当前进程打开了太多文件，不能再打开文件了。重复的文件描述符也会计入此限制。
</div>

<div style="margin: 0 0 1em 2em;">

在BSD和GNU中，打开文件的数量是由一个资源限制控制，通常可以提高该限制。如果遇到此错误，你可以提高`RLIMIT_NOFILE`限制，或设置他为无限制；参考[Limiting Resource Usage](https://sourceware.org/glibc/manual/latest/html_node/Limits-on-Resources.html)。
</div>

宏：`int` **`ENFILE`**

<div style="margin: 0 0 1em 2em;">

“Too many open files in system.”在整个系统中，打开了太多不同的文件。注意，任意数量的链接通道仅记为一次文件打开；参考[Linked Channels](https://sourceware.org/glibc/manual/latest/html_node/Linked-Channels.html)。在GNU/Hurd系统上，这个错误不会出现。
</div>

宏：`int` **`ENOTTY`**

<div style="margin: 0 0 1em 2em;">

“Inappropriate ioctl for device.”表示执行了不适用的I/O控制操作，例如试图对普通文件设置终端模式。
</div>

宏：`int` **`ETXTBSY`**

<div style="margin: 0 0 1em 2em;">

“Text file busy.”表示尝试执行一个当前以写模式打开的文件，或者试图写入一个当前正在被执行的文件。使用调试器运行程序时，通常也被视为以写模式打开了该文件，因此也会引发此错误。（字面意思“文本文件忙”。）在GNU/Hurd系统上，这不是错误；系统会在必要时自动复制文本段。
</div>

宏：`int` **`EFBIG`**

<div style="margin: 0 0 1em 2em;">

“File too large.”文件大小超过系统允许的范围。
</div>

宏：`int` **`ENOSPC`**

<div style="margin: 0 0 1em 2em;">

“No space left on device.”写文件失败，因为磁盘满了。
</div>

宏：`int` **`ESPIPE`**

<div style="margin: 0 0 1em 2em;">

“Illegal seek.”无效的查找（seek）操作（例如在管道上执行了该操作）。
</div>

宏：`int` **`EROFS`**

<div style="margin: 0 0 1em 2em;">

“Read-only file system.”尝试在只读系统上修改某些东西。
</div>

宏：`int` **`EMLINK`**

<div style="margin: 0 0 1em 2em;">


“Too many links.”表示单个文件的硬链接数量将达到系统允许的上限。当对一个已经达到最大链接数的文件`rename`时，也可能触发此错误（参考[Renaming Files](https://sourceware.org/glibc/manual/latest/html_node/Renaming-Files.html)）。
</div>

宏：`int` **`EPIPE`**

<div style="margin: 0 0 1em 2em;">

“Broken pipe.”pipe的另一端没有进程在读取数据。所有报这个错误码的库函数同时也会生成一个`SIGPIPE`信号；如果这个信号没有被处理或屏蔽，这个信号会终端程序。因此，除非你的程序已经对`SIGPIPE`进行了处理或屏蔽，否则实际上永远不会收到`EPIPE`错误。
</div>

宏：`int` **`EDOM`**

<div style="margin: 0 0 1em 2em;">

“Numerical argument out of domain.”数学函数的参数值不在该函数的定义域内。
</div>

宏：`int` **`ERANGE`**

<div style="margin: 0 0 1em 2em;">

“Numerical result out of range.”数学函数的计算结果因上溢或下溢而无法表示。
</div>

宏：`int` **`EAGAIN`**

<div style="margin: 0 0 1em 2em;">

“Resource temporarily unavailable.”一会儿后再尝试，调用也许就成功了。

这个错误可能在一些不同的情况下发生：

- 对一个设置了非阻塞模式的对象进行会阻塞的操作。如果再次尝试相同操作，将会阻塞，直到某个外部的条件让他能够读，写，或连接（无论什么操作）。你可以使用`select`查询何时可以执行操作；参考[Waiting for Input or Output](https://sourceware.org/glibc/manual/latest/html_node/Waiting-for-I_002fO.html)。

- 临时的资源短缺导致操作无法完成。`fork`可能返回此错误。这表示资源资源短缺是暂时性的，因此稍后重试可能会成功。在重试前等待几秒可能会很有用，以便让其他进程有时间释放稀缺资源。此类资源短缺通常相当严重，会影响整个系统，因此交互式程序通常应向用户报告该错误，并返回到命令循环中。

<strong>可移植性说明：</strong>在the GNU C Library中，`EAGAIN`和`EWOULDBLOCK`的值相等。可移植的代码，应该同时检查两个错误码，并将他们视为同一种情况处理。
</div>

宏：`int` **`EWOULDBLOCK`**

<div style="margin: 0 0 1em 2em;">

“Operation would block.”在the GNU C Library中，这是`EAGAIN`的另一个名称（上面那个）。
</div>

宏：`int` **`EINPROGRESS`**

<div style="margin: 0 0 1em 2em;">

“Operation now in progress.”对一个已设置为非阻塞模式的对象发起一个无法立即完成的操作。有些函数必须阻塞（比如`connect`；参考[Making a Connection](https://sourceware.org/glibc/manual/latest/html_node/Connecting.html)），永远不会返回`EAGAIN`。相反他们返回`EINPROGRESS`，表明操作已经启动，但还需要一些时间才能完成。尝试在调用完成前操作那个对象，则会返回`EALREADY`。你可以使用`select`查询待处理操作何时完成；参考[Waiting for Input or Output](https://sourceware.org/glibc/manual/latest/html_node/Waiting-for-I_002fO.html)。
</div>

宏：`int` **`EALREADY`**

<div style="margin: 0 0 1em 2em;">

“Operation already in progress.”在一个已设置为非阻塞模式的对象上，已经有一个操作正在执行。
</div>

宏：`int` **`ENOTSOCK`**

<div style="margin: 0 0 1em 2em;">

“Socket operation on non-socket.”在需要socket的场合，制定了一个非socket的文件。
</div>

宏：`int` **`EMSGSIZE`**

<div style="margin: 0 0 1em 2em;">

“Message too long.”传入socket的消息大小超过了最大限制。
</div>

宏：`int` **`EPROTOTYPE`**

<div style="margin: 0 0 1em 2em;">

“Protocol wrong type for socket.”socket类型不支持所请求的通信协议。
</div>

宏：`int` **`ENOPROTOOPT`**

<div style="margin: 0 0 1em 2em;">

“Protocol not available.”你指定的socket选项对于该socket使用的特定协议没有意义。参考[Socket Options](https://sourceware.org/glibc/manual/latest/html_node/Socket-Options.html)。
</div>

宏：`int` **`EPROTONOSUPPORT`**

<div style="margin: 0 0 1em 2em;">

“Protocol not supported.”该socket域不支持所请求的通信协议（可能是因为所请求的协议完全无效）。参考[Creating a Socket](https://sourceware.org/glibc/manual/latest/html_node/Creating-a-Socket.html)。
</div>

宏：`int` **`ESOCKTNOSUPPORT`**

<div style="margin: 0 0 1em 2em;">

“Socket type not supported.”socket类型不支持。
</div>

宏：`int` **`EOPNOTSUPP`**

<div style="margin: 0 0 1em 2em;">

“Operation not supported.”你请求的操作不支持。一些socket函数不是对所有类型的socket都有意义，还有一些可能没有对所有通信协议实现。在GNU/Hurd系统上，当对象不支持特定操作时，许多调用都会产生这个错误；这是一个通用指示，服务器不知道该如何处理该调用。
</div>

<div style="margin: 0 0 1em 2em;">

<strong>可移植性说明：</strong>取决于操作系统，`EOPNOTSUPP`和`ENOTSUP`的值可能相等。可移植代码应同时检查这两种错误，并将他们视为相同情况处理。
</div>

宏：`int` **`EPFNOSUPPORT`**

<div style="margin: 0 0 1em 2em;">

“Protocol family not supported.”你所请求的socket通信协议族不受支持。
</div>

宏：`int` **`EAFNOSUPPORT`**

<div style="margin: 0 0 1em 2em;">

“Address family not supported by protocol.”对socket指定的地址族不受支持；他和socket使用的通信协议不一致。参考[Sockets](https://sourceware.org/glibc/manual/latest/html_node/Sockets.html).
</div>

宏：`int` **`EADDRINUSE`**

<div style="margin: 0 0 1em 2em;">

“Address already in use.”所请求的socket地址已经在使用中。参考[Socket Addresses](https://sourceware.org/glibc/manual/latest/html_node/Socket-Addresses.html)。
</div>

宏：`int` **`ENETDOWN`**

<div style="margin: 0 0 1em 2em;">

“Network is down.”因为网络已断开，socket操作失败。
</div>

宏：`int` **`ENETUNREACH`**

<div style="margin: 0 0 1em 2em;">

“Network is unreachable.”因为包含远程主机的子网不可达，socket操作失败。
</div>

宏：`int` **`ENETRESET`**

<div style="margin: 0 0 1em 2em;">

“Network dropped connection on reset.”因为远程主机崩溃，网络连接被重置。
</div>

宏：`int` **`ECONNABORTED`**

<div style="margin: 0 0 1em 2em;">

“Software caused connection abort.”网络连接在本地被中止。
</div>

宏：`int` **`ECONNRESET`**

<div style="margin: 0 0 1em 2em;">

“Connection reset by peer.”网络连接因本地主机无法控制的原因而关闭，例如远程机器重启，或发生了不可恢复的协议违规。
</div>

宏：`int` **`ENOBUFS`**

<div style="margin: 0 0 1em 2em;">

“No buffer space available.”内核用于I/O操作的缓冲区已全部被占用。在GNU系统中，此错误始终与`ENOMEM`同义；你在网络操作时，可能会返回其中任意一个错误码。
</div>

宏：`int` **`EISCONN`**

<div style="margin: 0 0 1em 2em;">

“Transport endpoint is already connected.”你尝试连接一个已连接的socket。参考[Making a Connection](https://sourceware.org/glibc/manual/latest/html_node/Connecting.html)。
</div>

宏：`int` **`ENOTCONN`**

<div style="margin: 0 0 1em 2em;">

“Transport endpoint is not connected.”socket未连接。当你通过socket传输数据，但没有事先指定数据的目标地址时，就会收到此错误。对于未连接的socket（例如UDP等数据报协议），则会收到`EDESTADDRREQ`。
</div>

宏：`int` **`EDESTADDRREQ`**

<div style="margin: 0 0 1em 2em;">

“Destination address required.”socket未设置默认目标地址。当你尝试通过无连接的套接字发送数据，但没有事先使用`connect`指定数据的目标地址时，便会收到此错误。
</div>

宏：`int` **`ESHUTDOWN`**

<div style="margin: 0 0 1em 2em;">

“Cannot send after transport endpoint shutdown.”socket已关闭。
</div>

宏：`int` **`ETOOMANYREFS`**

<div style="margin: 0 0 1em 2em;">

“Too many references: cannot splice.”
</div>

<div style="margin: 0 0 1em 2em;">

AI生成：这个错误源于早期的Unix网络实现（特别是BSD），当时存在一种叫做“Protocol Splicing”（协议拼接）的实验性机制。该机制允许将一个套接字的输出直接“拼接”到另一个套接字的输入上，以实现零拷贝的数据转发。当参与拼接的套接字引用计数超过系统允许的上限时，就会报出ETOOMANYREFS: Too many references: cannot splice错误。
</div>

宏：`int` **`ETIMEDOUT`**

<div style="margin: 0 0 1em 2em;">

“Connection timed out.”设置了超时时间的socket操作在指定超时时间内，未收到任何响应。
</div>

宏：`int` **`ECONNREFUSED`**

<div style="margin: 0 0 1em 2em;">

“Connection refused.”远程主机拒绝了网络连接（通常因为他没有运行所请求的服务）。
</div>

宏：`int` **`ELOOP`**

<div style="margin: 0 0 1em 2em;">

“Too many levels of symbolic links.”在解析文件名的过程中遇到了过多的符号链接（即软链接，类似与windows的桌面快捷方式，与硬链接的区别是，软链接的源文件删除后会失效，软链接可以指向目录）。这通常表明存在符号链接循环。
</div>

宏：`int` **`ENAMETOOLONG`**

<div style="margin: 0 0 1em 2em;">

“File name too long.”文件名过长（比`PATH_MAX`更长；参考[Limits on File System Capacity](https://sourceware.org/glibc/manual/latest/html_node/Limits-for-Files.html)）或主机名过长（在`gethostname`或`sethostname`中；参考[Host Identification](https://sourceware.org/glibc/manual/latest/html_node/Host-Identification.html)）。
</div>

宏：`int` **`EHOSTDOWN`**

<div style="margin: 0 0 1em 2em;">

“Host is down.”所请求的网络连接对应的远程主机已关闭。
</div>

宏：`int` **`EHOSTUNREACH`**

<div style="margin: 0 0 1em 2em;">

“No route to host.”所请求的网络连接对应的远程主机不可达。
</div>

宏：`int` **`ENOTEMPTY`**

<div style="margin: 0 0 1em 2em;">

“Directory not empty.”需要空目录的情况下，目录不为空。通常，这个错误发生在你尝试删除一个目录。
</div>

宏：`int` **`EPROCLIM`**

<div style="margin: 0 0 1em 2em;">

“Too many processes.”这意味着尝试`fork`操作将超过每个用户的进程数上限。参考[Limiting Resource Usage](https://sourceware.org/glibc/manual/latest/html_node/Limits-on-Resources.html)，以了解有关`RLIMIT_NPROC`限制的详细信息。
</div>

宏：`int` **`EUSERS`**

<div style="margin: 0 0 1em 2em;">

“Too many users.”由于用户数量过多，文件配额系统出现混乱。
</div>

宏：`int` **`EDQUOT`**

<div style="margin: 0 0 1em 2em;">

“Disk quota exceeded.”用户磁盘配额已超额。
</div>

宏：`int` **`ESTALE`**

<div style="margin: 0 0 1em 2em;">

“Stale file handle.”这表明文件系统内部出现混乱，通常是由于NFS文件系统的服务器端进行了文件系统重组，或其他文件系统发生了损坏。要修复这个问题，通常需要卸载文件系统，可能能修复，然后重新挂载。
</div>

宏：`int` **`EREMOTE`**

<div style="margin: 0 0 1em 2em;">

“Object is remote.”尝试通过NFS挂载一个远程文件系统时，所使用的文件名本身已经指向了一个NFS挂载的文件。（在某些操作系统上这属于错误，但是我们希望在GNU/Hurd系统上能正常工作，因此不会出现这个错误码。）
</div>

宏：`int` **`EBADRPC`**

<div style="margin: 0 0 1em 2em;">

“RPC struct is bad.”
</div>

宏：`int` **`ERPCMISMATCH`**

<div style="margin: 0 0 1em 2em;">

“RPC version wrong.”
</div>

宏：`int` **`EPROGUNAVAIL`**

<div style="margin: 0 0 1em 2em;">

“RPC program not available.”
</div>

宏：`int` **`EPROGMISMATCH`**

<div style="margin: 0 0 1em 2em;">

“RPC program version wrong.”
</div>

宏：`int` **`EPROCUNAVAIL`**

<div style="margin: 0 0 1em 2em;">

“RPC bad procedure for program.”
</div>

宏：`int` **`ENOLCK`**

<div style="margin: 0 0 1em 2em;">

“No locks available.”这个是文件锁功能来使用的；参考[File Locks](https://sourceware.org/glibc/manual/latest/html_node/File-Locks.html)。这个错误不会出现在GNU/Hurd系统上，但是可能作为一个对其他操作系统上的NFS服务器的操作的结果返回。
</div>

宏：`int` **`EFTYPE`**

<div style="margin: 0 0 1em 2em;">

“Inappropriate file type or format.”该操作的文件类型不正确，或数据文件的格式不正确。
</div>

<div style="margin: 0 0 1em 2em;">

在某些系统上，如果尝试对非目录文件设置粘滞位（sticky bit），`chmod`会返回此错误；参考[Assigning File Permissions](https://sourceware.org/glibc/manual/latest/html_node/Setting-Permissions.html)。
</div>

宏：`int` **`EAUTH`**

<div style="margin: 0 0 1em 2em;">

“Authentication error.”
</div>

宏：`int` **`ENEEDAUTH`**

<div style="margin: 0 0 1em 2em;">

“Need authenticator.”
</div>

宏：`int` **`ENOSYS`**

<div style="margin: 0 0 1em 2em;">

“Function not implemented.”这表明调用的函数没有实现，无论在C库还是在操作系统中。当你遇到这个错误时，除非你安装新版本的C库或操作系统，不然某个函数会一直报`ENOSYS`错误。
</div>

宏：`int` **`ELIBEXEC`**

<div style="margin: 0 0 1em 2em;">

“Cannot exec a shared library directly.”
</div>

宏：`int` **`ENOTSUP`**

<div style="margin: 0 0 1em 2em;">

“Not supported.”某个函数收到的参数的值是有效的，但是那个参数请求的功能不可用，则函数会返回这个错误。这意味着函数根本没有实现某个命令，选项值，或标志位。对于通过参数传入对象进行操作的函数，这可能意味着该对象（文件描述符，端口等）不支持其他参数；不同文件描述符可能支持不同的参数值的范围。
</div>

<div style="margin: 0 0 1em 2em;">

如果整个函数在当前实现中完全不可用，则会返回`ENOSYS`。
</div>

<div style="margin: 0 0 1em 2em;">

<strong>可移植性说明：</strong>在某些系统上，`EAGAIN`和`EWOULDBLOCK`的值可能相等。可移植的代码，应该同时检查两个错误码，并将他们视为同一种情况处理。
</div>

宏：`int` **`EILSEQ`**

<div style="margin: 0 0 1em 2em;">

“Invalid or incomplete multibyte or wide character.”在解码多字节字符时，函数遇到了无效或不完整的字节序列，或者给定的宽字符无效。
</div>

宏：`int` **`EBACKGROUND`**

<div style="margin: 0 0 1em 2em;">

“Inappropriate operation for background process.”在GNU/Hurd系统上，当调用者不属于终端的前台进程组时，支持`term`协议的服务端会对某些操作返回此错误。用户通常不会直接看到这个错误，因为`read`和`write`等函数会将其转换为`SIGTTIN`或`SIGTTOU`信号。参考[Job Control](https://sourceware.org/glibc/manual/latest/html_node/Job-Control.html)，以了解有关进程组和这些信号的详细信息。
</div>

宏：`int` **`EDIED`**

<div style="margin: 0 0 1em 2em;">

“Translator died.”在GNU/Hurd系统上，当文件（这个文件可以是任何东西，比如硬件，程序等）被翻译，并且翻译器在启动中，与文件连接之前死亡了，则该程序会返回此错误。
</div>

<div style="margin: 0 0 1em 2em;">

AI生成：GNU/Hurd采用了一种独特的微内核+用户态服务器架构。与传统Unix将所有文件系统逻辑写死在内核里不同，Hurd把文件系统的实现放在了用户空间的独立程序中，这些程序就叫Translators（转换器/翻译器）。当你访问某个文件或目录时，Hurd实际上是在与一个Translator进程通信。
</div>

宏：`int` **`ED`**

<div style="margin: 0 0 1em 2em;">

“?.”有经验的用户自然能看的出来哪里错了。
</div>

宏：`int` **`EGREGIOUS`**

<div style="margin: 0 0 1em 2em;">

“You really blew it this time.（你彻底搞杂了。）”你到底干了<strong>什么</strong>？
</div>

宏：`int` **`EIEIO`**

<div style="margin: 0 0 1em 2em;">

“Computer bought the farm.（电脑报废了。）”事已至此，先吃饭吧。
</div>

宏：`int` **`EGRATUITOUS`**

<div style="margin: 0 0 1em 2em;">

“Gratuitous error.”这个错误码毫无用处。
</div>

宏：`int` **`EBADMSG`**

<div style="margin: 0 0 1em 2em;">

“Bad message.”
</div>

宏：`int` **`EIDRM`**

<div style="margin: 0 0 1em 2em;">

“Identifier removed.”
</div>

宏：`int` **`EMULTIHOP`**

<div style="margin: 0 0 1em 2em;">

“Multihop attempted.”
</div>

宏：`int` **`ENODATA`**

<div style="margin: 0 0 1em 2em;">

“No data available.”
</div>

宏：`int` **`ENOLINK`**

<div style="margin: 0 0 1em 2em;">

“Link has been severed.”
</div>

宏：`int` **`ENOMSG`**

<div style="margin: 0 0 1em 2em;">

“No message of desired type.”
</div>

宏：`int` **`ENOSR`**

<div style="margin: 0 0 1em 2em;">

“Out of streams resources.”
</div>

宏：`int` **`ENOSTR`**

<div style="margin: 0 0 1em 2em;">

“Device not a stream.”
</div>

宏：`int` **`EOVERFLOW`**

<div style="margin: 0 0 1em 2em;">

“Value too large for defined data type.”
</div>

宏：`int` **`EPROTO`**

<div style="margin: 0 0 1em 2em;">

“Protocol error.”
</div>

宏：`int` **`ETIME`**

<div style="margin: 0 0 1em 2em;">

“Timer expired.”
</div>

宏：`int` **`ECANCELED`**

<div style="margin: 0 0 1em 2em;">

“Operation canceled.”一个异步操作在完成前被取消了。参考[Perform I/O Operations in Parallel](https://sourceware.org/glibc/manual/latest/html_node/Asynchronous-I_002fO.html)。当你调用`aio_cancel`，受影响的操作的正常结果是以该错误码结束；参考[Cancellation of AIO Operations](https://sourceware.org/glibc/manual/latest/html_node/Asynchronous-I_002fO.html)。
</div>

宏：`int` **`EOWNERDEAD`**

<div style="margin: 0 0 1em 2em;">

“Owner died.”
</div>

宏：`int` **`ENOTRECOVERABLE`**

<div style="margin: 0 0 1em 2em;">

“State not recoverable.”
</div>

以下的错误码在Linux/i386内核中定义。他们还没有文档说明。

宏：`int` **`ERESTART`**

<div style="margin: 0 0 1em 2em;">

“Interrupted system call should be restarted.”
</div>

宏：`int` **`ECHRNG`**

<div style="margin: 0 0 1em 2em;">

“Channel number out of range.”
</div>

宏：`int` **`EL2NSYNC`**

<div style="margin: 0 0 1em 2em;">

“Level 2 not synchronized.”
</div>

宏：`int` **`EL3HLT`**

<div style="margin: 0 0 1em 2em;">

“Level 3 halted.”
</div>

宏：`int` **`EL3RST`**

<div style="margin: 0 0 1em 2em;">

“Level 3 reset.”
</div>

宏：`int` **`ELNRNG`**

<div style="margin: 0 0 1em 2em;">

“Link number out of range.”
</div>

宏：`int` **`EUNATCH`**

<div style="margin: 0 0 1em 2em;">

“Protocol driver not attached.”
</div>

宏：`int` **`ENOCSI`**

<div style="margin: 0 0 1em 2em;">

“No CSI structure available.”
</div>

宏：`int` **`EL2HLT`**

<div style="margin: 0 0 1em 2em;">

“Level 2 halted.”
</div>

宏：`int` **`EBADE`**

<div style="margin: 0 0 1em 2em;">

“Invalid exchange.”
</div>

宏：`int` **`EBADR`**

<div style="margin: 0 0 1em 2em;">

“Invalid request descriptor.”
</div>

宏：`int` **`EXFULL`**

<div style="margin: 0 0 1em 2em;">

“Exchange full.”
</div>

宏：`int` **`ENOANO`**

<div style="margin: 0 0 1em 2em;">

“No anode.”
</div>

宏：`int` **`EBADRQC`**

<div style="margin: 0 0 1em 2em;">

“Invalid request code.”
</div>

宏：`int` **`EBADSLT`**

<div style="margin: 0 0 1em 2em;">

“Invalid slot.”
</div>

宏：`int` **`EDEADLOCK`**

<div style="margin: 0 0 1em 2em;">

“File locking deadlock error.”
</div>

宏：`int` **`EBFONT`**

<div style="margin: 0 0 1em 2em;">

“Bad font file format.”
</div>

宏：`int` **`ENONET`**

<div style="margin: 0 0 1em 2em;">

“Machine is not on the network.”
</div>

宏：`int` **`ENOPKG`**

<div style="margin: 0 0 1em 2em;">

“Package not installed.”
</div>

宏：`int` **`EADV`**

<div style="margin: 0 0 1em 2em;">

“Advertise error.”
</div>

宏：`int` **`ESRMNT`**

<div style="margin: 0 0 1em 2em;">

“Srmount error.”
</div>

宏：`int` **`ECOMM`**

<div style="margin: 0 0 1em 2em;">

“Communication error on send.”

宏：`int` **`EDOTDOT`**
</div>

<div style="margin: 0 0 1em 2em;">

“RFS specific error.”
</div>

宏：`int` **`ENOTUNIQ`**

<div style="margin: 0 0 1em 2em;">

“Name not unique on network.”
</div>

宏：`int` **`EBADFD`**

<div style="margin: 0 0 1em 2em;">

“File descriptor in bad state.”
</div>

宏：`int` **`EREMCHG`**

<div style="margin: 0 0 1em 2em;">

“Remote address changed.”
</div>

宏：`int` **`ELIBACC`**

<div style="margin: 0 0 1em 2em;">

“Can not access a needed shared library.”
</div>

宏：`int` **`ELIBBAD`**

<div style="margin: 0 0 1em 2em;">

“Accessing a corrupted shared library.”
</div>

宏：`int` **`ELIBSCN`**

<div style="margin: 0 0 1em 2em;">

“.lib section in a.out corrupted.”
</div>

宏：`int` **`ELIBMAX`**

<div style="margin: 0 0 1em 2em;">

“Attempting to link in too many shared libraries.”
</div>

宏：`int` **`ESTRPIPE`**

<div style="margin: 0 0 1em 2em;">

“Streams pipe error.”
</div>

宏：`int` **`EUCLEAN`**

<div style="margin: 0 0 1em 2em;">

“Structure needs cleaning.”
</div>

宏：`int` **`ENOTNAM`**

<div style="margin: 0 0 1em 2em;">

“Not a XENIX named type file.”
</div>

宏：`int` **`ENAVAIL`**

<div style="margin: 0 0 1em 2em;">

“No XENIX semaphores available.”
</div>

宏：`int` **`EISNAM`**

<div style="margin: 0 0 1em 2em;">

“Is a named type file.”
</div>

宏：`int` **`EREMOTEIO`**

<div style="margin: 0 0 1em 2em;">

“Remote I/O error.”
</div>

宏：`int` **`ENOMEDIUM`**

<div style="margin: 0 0 1em 2em;">

“No medium found.”
</div>

宏：`int` **`EMEDIUMTYPE`**

<div style="margin: 0 0 1em 2em;">

“Wrong medium type.”
</div>

宏：`int` **`ENOKEY`**

<div style="margin: 0 0 1em 2em;">

“Required key not available.”
</div>

宏：`int` **`EKEYEXPIRED`**

<div style="margin: 0 0 1em 2em;">

“Key has expired.”
</div>

宏：`int` **`EKEYREVOKED`**

<div style="margin: 0 0 1em 2em;">

“Key has been revoked.”
</div>

宏：`int` **`EKEYREJECTED`**

<div style="margin: 0 0 1em 2em;">

“Key was rejected by service.”
</div>

宏：`int` **`ERFKILL`**

<div style="margin: 0 0 1em 2em;">

“Operation not possible due to RF-kill.”
</div>

宏：`int` **`EHWPOISON`**

<div style="margin: 0 0 1em 2em;">

“Memory page has hardware error.”
</div>

## 2.3 错误信息

库提供了一些函数和变量，为了方便你的程序可以客制化格式输出有关库调用失败的详细错误信息。`strerror`和`perror`函数可以根据传入的错误码，返回标准的错误信息；`program_invocation_short_name`变量可以让你获取发生错误的函数的名称。

函数：`char` `*` **`strerror`** `(` `int` *`errnum`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe | AS-Unsafe heap i18n | AC-Unsafe mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

`strerror`函数将由*errnum*参数提供的错误码（参考[Checking for Errors](https://sourceware.org/glibc/manual/latest/html_node/Checking-for-Errors.html)）映射为描述性的错误信息字符串。该字符串会根据当前区域（locale）设置进行翻译。返回值是字符串指针。
</div>

<div style="margin: 0 0 1em 2em;">

*errnum*的值通常来自于`errno`变量。
</div>

<div style="margin: 0 0 1em 2em;">

你不应该修改`strerror`返回的字符串。此外，如果后续再次调用`strerror`或`strerror_l`，或者获取该字符串的线程退出了，则返回的指针将失效。（所有`strerror`的返回值指向同一个地址。）
</div>

<div style="margin: 0 0 1em 2em;">

由于调用`strerror`之后无法恢复之前的状态，库代码不应该调用这个函数，因为这可能会影响到使用`strerror`的程序，在程序尚未使用完毕就让字符串指针失效。作为替代，可以使用`strerror_r`，带‘`%m`’或‘`%#m`’格式说明符的`snprintf`，`strerrorname_np`，或`strerrordesc_np`。
</div>

<div style="margin: 0 0 1em 2em;">

`strerror`函数不会改变`errno`的值，且不会失败。
</div>

<div style="margin: 0 0 1em 2em;">

`strerror`函数在`string.h`中声明。
</div>

函数：`char` `*` **`strerror_l`** `(` `int` *`errnum`* `,` `locale_t` *`locale`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe | AS-Unsafe heap i18n | AC-Unsafe mem |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

此函数与`strerror`类似，除了返回的字符串会根据*locale*参数翻译（而不是`strerror`使用的当前*locale*）。注意，调用`strerror_l`会是`strerror`的返回指针无效，反之亦然。
</div>

<div style="margin: 0 0 1em 2em;">

函数`strerror_l`在`string.h`中声明，由POSIX定义。
</div>

函数：`char` `*` **`strerror_r`** `(` `int` *`errnum`* `,` `char` `*` *`buf`* `,` `size_t` *`n`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe | AS-Unsafe i18n | AC-Unsafe |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

下面的描述是关于函数的GNU变体，定义`_GNU_SOURCE`时可用。参考[Feature Test Macros](https://sourceware.org/glibc/manual/latest/html_node/Feature-Test-Macros.html)。
</div>

<div style="margin: 0 0 1em 2em;">

`strerror_r`函数与`strerror`相似，但是返回值不是指向the GNU C Library管理的字符串，他可以使用用户提供的*buf*缓冲区来存储字符串。
</div>

<div style="margin: 0 0 1em 2em;">

最多向*buf*中写入*n*个字符（包括NUL字节'\0'），因此用户应该选择一个足够大的缓冲区。返回的指针是否指向*buf*数组，取决于*errnum*的值（如果*errnum*的值是GNU已有的，则会直接返回该字符串的指针，避免了复制字符串的操作）。如果返回值字符串没有存储到*buf*中，则该字符串在剩余的程序执行中不能修改。
</div>

<div style="margin: 0 0 1em 2em;">

`strerror_r`函数是GNU扩展，他在`string.h`中声明。有一个他的POSIX变体，接下来会讲。
</div>

函数：`int` `strerror_r` `(` `int` *`errnum`* `,` `char` `*` *`buf`* `,` `size_t` *`n`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Safe | AS-Unsafe i18n | AC-Unsafe |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

`strerror_r`函数变体，只有在包含`strerror_r`的标准启用了，并且没用定义`_GNU_SOURCE`。该函数的POSIX变体总会错误信息写入指定的*n*字节大小的*buf*缓冲区。
</div>

<div style="margin: 0 0 1em 2em;">

成功时，`strerror_r`返回0。两个额外的返回值用来表达失败。
</div>

<div style="margin: 0 0 1em 2em;">

`EINVAL`
</div>

<div style="margin: 0 0 1em 4em;">

*errnum*参数不与任何错误码对应。
</div>

<div style="margin: 0 0 1em 2em;">

ERANGE
</div>

<div style="margin: 0 0 1em 4em;">

缓冲区大小*n*不够装下整个错误信息。
</div>

<div style="margin: 0 0 1em 2em;">

即使报告了错误，`strerror_r`仍会尽量将错误信息写入输出缓冲区。调用`strerror_r`后，`errno`的值是不确定的。
</div>

<div style="margin: 0 0 1em 2em;">

如果你想在`_GNU_SOURCE`定义了的程序中，使用总是复制的POSIX的`strerror_r`，你可以使用带‘`%m`’转换说明符的`snprintf`，比如这样：
</div>

<div style="margin: 0 0 1em 2em;">

```c
    int saved_errno = errno;
    errno = errnum;
    int ret = snprintf (buf, n, "%m");
    errno = saved_errno;
    if (strerrorname_np (errnum) == NULL)
    return EINVAL;
    if (ret >= n)
    return ERANGE:
    return 0;
```
</div>

<div style="margin: 0 0 1em 2em;">

此函数在`string.h`中声明，如果有的话。他是POSIX扩展（至少是2008版）。
</div>

函数：`void` **`perror`** `(` `const` `char` `*` *`message`* `)`

<div style="margin: 0 0 1em 2em;">

Preliminary: | MT-Unsafe race:stderr | AS-Unsafe corrupt i18n heap lock | AC-Unsafe corrupt lock mem fd |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

此函数向标准错误流`stderr`打印一条错误消息；参考[Standard Streams](https://sourceware.org/glibc/manual/latest/html_node/Standard-Streams.html)。`stderr`的方向不会改变。AI生成：流方向（orientation）是C标准中的概念：一个流一旦被设定为字节导向或宽字符导向，就不能混用另一类I/O函数。
</div>

<div style="margin: 0 0 1em 2em;">

如果调用`perror`时，传入的*message*是一个空指针或空字符串，`perror`会打印`errno`对应的错误信息，末尾有一个换行符。
</div>

<div style="margin: 0 0 1em 2em;">

如果你提供了一个非空*message*参数，那么`perror`会将该字符串作为输出的前缀。他加了一个冒号和空格来隔开*message*和`errno`对应的错误信息。
</div>

<div style="margin: 0 0 1em 2em;">

The function `perror` is declared in `stdio.h`.函数`perror`在`stdio.h`中声明。
</div>

函数：`const` `char` `*` `strerrorname_np` `(` `int` *`errnum`* `)`

<div style="margin: 0 0 1em 2em;">

| MT-Safe | AS-Safe | AC-Safe |参考[POSIX Safety Concepts](https://sourceware.org/glibc/manual/latest/html_node/POSIX-Safety-Concepts.html)。
</div>

<div style="margin: 0 0 1em 2em;">

此函数返回描述错误*errnum*的名称，或者，如果没有已知常量对应该值，则返回`NULL`（例如，传入`EINVAL`，返回"EINVAL"）。返回的字符串在剩余的程序执行中不能修改。
</div>