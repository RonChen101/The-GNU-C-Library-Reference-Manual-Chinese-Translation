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

<div style="margin: 0 0 0 3em;">
变量<code>errno</code>保存了系统错误码。你可以修改<code>errno</code>的值。
</div>

<div style="margin: 0 0 1em 3em;">
因为<code>errno</code>被<code>volatile</code>修饰，他可能会被信号处理器异步的修改；参考<a href="https://sourceware.org/glibc/manual/latest/html_node/Defining-Handlers.html">Defining Signal Handlers</a>。然而，编写规范的信号处理器会保存并恢复<code>errno</code>的值，因此除了编写信号处理器本身之外，通常无需担心这一问题。
</div>

<div style="margin: 0 0 1em 3em;">
程序启动时，<code>errno</code>的初始值为0。在多数情况下，当库函数遇到错误时，他会将<code>error</code>设置为非0值，以指出发生了什么错误。每个函数的文档中都列出了可能的错误条件。不是所有库函数都使用这个机制；有些函数直接返回错误码。
</div>

<div style="margin: 0 0 1em 3em;">
<strong>警告：</strong>许多库函数即使未遇到错误，甚至在其直接返回错误码的情况下，也会将<code>errno</code>设置为某个无意义的非零值。因此，通常不能通过检查<code>errno</code>的值来判断是否发生错误。正确的检查错误的方法在每个函数的文档说明中。
</div>

<div style="margin: 0 0 1em 3em;">
<strong>可移植性声明：</strong>ISO C规定了<code>errno</code>为“可修改的左值”，而非变量，这允许用宏实现他。例如，他的宏展开可能包含函数调用，比如<code>*__errno_location ()</code>。事实上，GNU/Linux和GNU/Hurd系统上就是这样的。The GNU C Library，会根据不同的系统使用不同的实现方式。
</div>

<div style="margin: 0 0 1em 3em;">
有些库函数，比如<code>sqrt</code>和<code>atan</code>，在发生错误时仍然会返回一个完全合法的值，但同时也会设置<code>errno</code>。关于这些函数，建议调用前将<code>errno</code>设置为0，然后再检查他的值。
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
“Operation not permitted.” Only the owner of the file (or other resource) or processes with special privileges can perform the operation.
</div>

