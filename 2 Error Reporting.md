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
The variable <code>errno</code> contains the system error number. You can change the value of <code>errno</code>.
</div>

<div style="margin: 0 0 1em 3em;">

</div>