这是The GNU C Library Reference Manual，版本2.43。

Copyright © 1993–2026 Free Software Foundation, Inc.

遵守the terms of the GNU Free Documentation License, Version 1.3 or any later version published by the Free Software Foundation时，以下内容是许可的，复制，分发，修改这个文档；不能修改“Free Software Needs Free Documentation” and “GNU Lesser General Public License”章节，封面文字必须是“A GNU Manual”，封底文字必须是下面(a)的内容。本许可证的副本包含在题为“GNU Free Documentation License”的章节中。

(a) The FSF’s Back-Cover Text is: “You have the freedom to copy and modify this GNU manual. Buying copies from the FSF supports it in developing GNU and promoting software freedom.”

# 1 介绍

C语言内置的功能不提供输入/输出，内存管理，字符串操作等简单的操作。这些功能都定义在标准库中，你需要编译并同你的程序链接。

The GNU C Library定义了所有ISO C标准指定的函数，并同时满足POSIX和其他Unix操作系统衍生系统特有的功能，还有GNU系统特有的扩展。

这个手册的目的是告诉你他和使用the GNU C Library的功能。文档中会指出哪些特性属于哪些标准，以帮助你鉴别哪些内容可能无法移植到其他系统。但是这个手册的重点不是严格的可移植性。

## 1.1 入门

这个手册的内容是假设你已经了解了类似C语言的语言和基本的编程概念。具体而言，是ISO标准C，而不是其他的C。

The GNU C Library包含几个头文件，分别提供相关功能的定义和声明；运行你的程序时，你的编译器会用到这些信息。比如，stdio.h头文件声明了输入和输出的功能，string.h头文件声明了字符操作的功能。这个手册的组织会遵循和头文件同样的分类。

如果你第一次看这个手册，你应该看所有的引言或基础介绍，并快速浏览所有章节。the GNU C Library中有很多函数，记住每个函数是怎么用是不现实的。重要的是对the library提供的功能有整体的概念，这样写程序的时候你能知道什么时候使用library的函数，这个手册可以找到关于他们的更多信息。

## 1.2 不同的标准和可移植性

这个章节讨论the GNU C Library基于的不同的标准和其他的资源。这些资源包括ISO C标准和POSIX标准，以及System  Unix的实现。

这个手册的主要焦点是告诉你如何高效的使用the GNU C Library的功能。但是如果你关心你的程序是否符合这些标准，或者移植到GNU以外的操作系统，这会影响你如何使用the library。这个章节概况了这些标准，所以当你在手册其他地方看到他们时，你知道他们是什么。

参考Summary of Library Facilities（附录B），字母排序的the library提供的函数和其他符号列表。这个列表也标出了各个函数和符号出自什么标准。

- ISO C
- POSIX (The Portable Operating System Interface)
- Berkeley Unix
- SVID (The System V Interface Description)
- XPG (The X/Open Portability Guide)
- Linux (The Linux Kernel)

### 1.2.1 ISO C标准

The GNU C Library兼容the American National Standards Institute (ANSI)采纳的C标准：American National Standard X3.159-1989—“ANSI C”，和后来被the International Standardization Organization (ISO)采纳的C标准：ISO/IEC 9899:1990, “Programming languages—C”。我们在本文中将其统称为ISO C，因为在采纳层面上，它是一个更为通用的标准。the GNU C Library的头文件和库方法是ISO C指定的那内容的超集。

如果你非常在意是否严格遵守ISO C标准，在使用GNU C编译器编译程序时，你应该加上-ansi选项。这会告诉编译器，只定义头文件中符合ISO标准的特性，除非你显式的指定了其他特性。参考Feature Test Macros（1.3.4），以了解更多。

能够限制库仅包含 ISO C 标准的功能非常重要，因为 ISO C 对库实现可以定义的标识符（名称）有严格的限制，而 GNU 扩展并不符合这些限制。参考Reserved Names（1.3.3），以了解更多。

手册不会尝试给你完整的细节关于ISO C和其他的区别。手册会给你可移植性高的写法建议，但不会追求完美。

### 1.2.2 POSIX (The Portable Operating System Interface)

The GNU C Library同时兼容ISO POSIX系列标准，更正式的说法是the Portable Operating System Interface for Computer Environments (ISO/IEC 9945)。他们也出版了ANSI/IEEE Std 1003。POSIX标准主要衍生自各种版本的Unix操作系统。

POSIX标准指定的功能是ISO C标准的超集；POSIX对ISO C标准的函数指定了额外的特性，也制定了新函数。总的来说，POSIX定义额外的需求和功能是为了给一些系统环境提供底层支持，而不是像通用编程语言，能在不同的操作系统上运行。

The GNU C Library实现了ISO/IEC 9945-1:1996指定的所有功能，即the POSIX System Application Program Interface，简称为POSIX.1。在ISO C标准功能上的主要扩展包括 file system interface primitives (see File System Interface 第14章)，device-specific terminal control functions (see Low-Level Terminal Interface 第17章)，and process control functions (see Processes 第27章)。

the GNU C Library也实现了ISO/IEC 9945-2:1993和the POSIX Shell and Utilities standard (POSIX.2)中的有些功能。这些包括正则表达式和其他的条件匹配功能(see Pattern Matching 第10章)。

#### 1.2.2.1 POSIX安全概念

手册记录了GNU C Library功能的各种安全属性，这些属性通常标注在函数原型下方，格式如下：

Preliminary: | MT-Safe | AS-Safe | AC-Safe |

这些安全属性是依据 POSIX 标准中针对线程安全（Thread- -Safety）、异步信号安全（Async-Signal- -Safety）和异步取消安全（Async-Cancel- -Safety）等安全场景所制定的评估标准来进行评定的。下面是对这些属性的直观解释，旨在帮助理解标准定义中的确切含义。

- MT-Safe或Thread-Safe函数可在有其他线程存在时安全调用。MT，意味着Multi Thread。

  线程安全不意味着函数是原子操作，也不意味着它使用了POSIX提供给用户的任何内存同步机制。甚至调用一系列线程安全的函数的组合是线程不安全的。比如，一个线程调用两个线程安全的函数一个紧接着另一个，这样并不保证这个组合是原子化执行的，因为其他线程中的并发调用可能会以破坏性的方式产生干扰。

  全程序优化可能会跨库接口进行函数内联，这可能暴露出不安全的指令重排问题，因此不建议对the GNU C Library接口执行跨接口内联。在全程序优化下，文档中记录的线程安全不保证。但是，不过，定义在用户可见头文件中的函数在设计上已确保可安全地进行内联。

- AS-Safe或Async-Signal-Safe的函数可以从异步信号处理程序中安全的调用。AS，意味着Asynchronous Signal。

  许多异步信号安全的函数会设置errno，或修改浮点环境，但这并不妨碍他们在信号处理程序中安全使用。然而，如果异步信号处理程序修改了这种线程局部状态，程序可能会出现异常行为，并且不能指望虚拟号处理机制自动保存和恢复该状态。因此，调用可能会使用errno或修改浮点环境的函数的信号处理程序，必须保持他们原来的值，并在退出前保存。

- AC-Safe或Async-Cancel-Safe函数可以安全的调用，当异步取消是开启的。AC，意味着Asynchronous Cancellation。

  POSIX标准只定义了3个异步取消安全函数，pthread\_cancel，pthread\_setcancelstate和pthread\_setcanceltype。目前，the GNU C Library除了这3个函数之外，不提供如何其他异步取消安全保证，但确实文档化了当前哪些函数是异步取消安全的。这个文档是给the GNU C Library开发者的。

  就像信号处理函数，取消清除步骤会自行配置他们徐奥的浮点环境。步骤不确保任何浮点环境，特别是异步取消开启时。假如浮点环境的配置不是原子化的，那么遇到的环境可能出现内部不一致的情况。

- MT-Unsafe, AS-Unsafe, AC-Unsafe函数无法安全的调用上述的内容。在这样的背景下调用他们包含未定义的行为。

  文档中没有显示标注安全的函数，在安全的背景下应该被视为不安全的。

- Preliminary安全属性也被记录，指示了未来可能没有的属性。

  Preliminary指出的属性是现在实现的表现评估后的结果，而不是现在或未来锁规定和允许的内容。

  尽管我们力求遵循相关标准，但在某些情况下，即使标准未作安全要求，我们的实现仍然是安全的；而在另一些情况下，我们的实现则未能满足标准规定的安全要求。后者多数是bugs；前者，Preliminary标记了，应该不被期望，未来标准会改变，可能会不兼容。

  此外，POSIX标准不提供安全的定义。我们假设POSIX所谓的“安全的调用”是指，只要程序未引发未定义的行为，那么“安全的调用”的函数就是好的，同时不影响其他函数的好的行为。我们选择使用更松弛的安全定义，不是因为他们是最好的定义，而是手册可以与POSIX更和谐。

  请记住preliminary定义和注释，某些层面的定义仍在讨论，可能会进一步澄清或被改变。

  随着时间的推移，我们计划将这些初步的安全说明逐步发展为稳定的承诺，其稳定性将与我们的接口承诺保持一致。到时候，我们会移除Preliminary标签。然而，只要该关键字仍然存在，它们就不应被视为对未来行为的承诺。

其他关键字出现在安全标注中的，定义在后面的章节。

#### 1.2.2.2 不安全的特性

在某些情况下不安全的函数，被标注了关键字以提示什么是不安全的。AS-Unsafe特性在这个章节指的是异步信号开启时函数永远不安全的。AC-Unsafe特性指的是异步取消开启时函数是永远不安全的。本章没有MT-Unsafe标记。

- lock

  标记lock的函数意味着有AS-Unsafe特性，在持有非递归锁期间可能会被信号中断。如果型号处理程序调用了其他持有相同锁的函数，就会死锁。

  标记lock的函数，如果被异步取消，可能会释放不了锁，而锁应该被释放。一旦锁处于被持有状态，后续获取该锁的尝试将会无限期阻塞。

- corrupt

  标记corrupt的函数意味着有AS-Unsafe特性，可能会破坏数据结构，被打断后发生异常行为。不像lock，这些可以拿递归锁来规避MT-Safety问题，但这仍不足以防止信号处理程序观察到更新了一半的数据结构。此外，如果被中断的函数未能察觉到信号处理程序所做的更新，还可能引发进一步的数据损坏。

  标记corrupt的函数可能会遗留损坏的数据，只更新一半的状态。接下来使用这些数据可能会发生意料之外的行为。

- heap

  标记heap的函数可能会调用malloc/free系列函数的堆内存管理函数，因此它们的安全性完全取决于这些内存管理函数。这个标记等价于：

  | AS-Unsafe lock | AC-Unsafe lock fd mem |

- dlopen（Dynamic Loading Open）

  标记dlopen的函数只用动态装载器来将共享库加载到当前的执行映像中。这涉及了打开文件，映射到内存，分配额外的内存，解析符号，应用重定位等操作，而所有这些操作都是在持有动态加载器内部锁的情况下执行的。

  这些锁本身就足以使这些函数成为 AS-Unsafe 和 AC-Unsafe 的，但还可能引发其他问题。目前这是由dlopen引起的所有可能的安全问题的占位符。

- plugin

  标记plugin的函数可能以插件的形式运行the GNU C Library之外的代码。插件函数大概率是MT-Safe，AS-Unsafe和AC-Unsafe。比如，栈展开库（stack unwinding libraries），名字服务切换（name service switch (NSS)）和字符集转换（character set conversion (iconv)）后端。

  尽管上述作为示例的插件均通过 dlopen 加载，但“plugin”这一关键词本身并不意味着直接涉及动态链接器或 libdl 接口——这些内容已归入 dlopen 条目下讨论。比如，一个函数加载一个模块并查询这个模块某些函数的地址，而另一个函数仅仅调用这些已经解决的函数，前者会标记为dlopen，后者会标记为plugin。当一个函数有这两种行为，那么会被两者都标记。

- il8n

  标记il8n的函数可能调用gettext系列的国际化函数，其安全性与这些被调用的函数保持一致。这个标记等价于：

  | MT-Safe env | AS-Unsafe corrupt heap dlopen | AC-Unsafe corrupt |

- timer

  标记timer的函数会使用alarm函数，或类似给系统设置超时回调，或一个长时间运行的操作。在多线程程序中，超时信号可能会送到其他线程上，导致无法关闭正确的线程。除了MT-Unsafe，这些函数总是AS-Unsafe，因为信号处理器中调用他们可能会干扰到被中断代码中设置的定时器，还有AC-Unsafe，因为无法保证先设置的定时器会不会被一个异步取消重置。

#### 1.2.2.3 条件下的安全特性

对于某些导致函数在特定上下文中调用不安全的特性，除了完全避免调用该函数之外，还存在已知的规避方法。下面的关键字用来指代他们，定义中指出了程序应该怎么约束的写来规避关键字指出的安全问题。只有所有让函数不安全的因素被处理了，按照文档中要求的那样，函数才能安全的调用。

- init

  标记init的函数意味着MT-Unsafe，在第一次调用时。

  在单线程模式调用这种函数，就能消除MT-Unsafe。当没有其他MT-Unsafe存在，那么程序就可以安全的多线程。

  因init而AS-或AC-Unsafe的函数使用了内部的libc\_once机制或者类似的机制初始化内部数据结构。

  如果一个信号处理程序中断了这种初始化器，然后调用任意使用libc\_once初始化器，如果线程库已经加载，则会导致死锁。

  此外，如果初始化器被一个需要相同的初始化过程的信号处理程序中断，有些或者全部的初始化器可能会运行多次，导致资源泄露甚至数据损坏。

  因init而AS-或AC-Unsafe的函数应该在配置信号处理程序或启用取消之前，这样因libc\_once导致的AS-和AC-Unsafe就不会发生。

- race

  带有race的函数有MT-Safety，操作对象时会导致数据竞争或并发运行时导致类似形式的数据损坏。有些情况下，操作的对象是用户传入的；有些是返回值；有些内容并不会被用户看到。

  我们认为对参数的操作方式应该是避免导致数据竞争的。这个避免应该是调用者的责任。我们不会标记一个函数是MT-Unsafe或AS-Unsafe，如果是调用者没有避免传入参数有数据竞争的可能性。一般而言，如果函数文档说明该函数会读取或修改以引用方式传入的对象，用户就应当使用内存同步底层调用来避免数据竞争，就如同他们不通过调用库函数而是亲自执行这些访问时所做的那样。文件流是例外，POSIX规定库在许多操作这种特定不透明类型对象的函数中，必须防范数据竞争。我们把这个当成方便送给用户，而非一项应将其预期推广至其他类型的普遍要求。

  为了提醒用户保护某个参数是他们的责任，我们会标注接收某些类型的参数的函数。对于用户传入的对象，我们划定的界限如下：凡是类型对用户公开、且预期由用户直接访问的对象,如内存缓冲区、字符串及各种用户可见的结构体类型，均不构成将函数标注为race的理由。这样做不仅会产生大量干扰信息，而且与前述一般要求相重复；此外，当库在访问那些用户本可直接访问的对象时未提供内部同步保护，也不会令太多人感到意外。

  至于透明或者透明类似的，因为他们只能传到库函数中操作（比如FILE，DIR，obstack，iconv\_t），库可能对其内部访问协调有额外的要求。如果某个函数接受此类对象作为参数，但默认情况下并不负责同步对其的访问，我们将使用race后跟冒号及相应参数名的方式对其进行标注。例如，针对文件流的无锁版本函数将会被标注；而那些默认对文件流执行隐式加锁的函数则不会被标注，即使这种隐式加锁机制可以按单个流进行禁用。

  无论如何，用户提供的对象没有保证访问方式合理，我们不会认为哪些可能会不安全的访问用户提供对象的函数是MT-Unsafe。普遍观念是，用户需要避免提供给库的对象有数据竞争。

  然而，对于库自己控制的对象，比如内部对象，用来返回数据给用户的静态缓存，就与用户无关。当库不保护他们的并发使用时，这种情况就会被视为MT-Unsafe和AS-Unsafe（尽管race标记在AS-Unsafe中回避当作重复省略，因为MT-Unsafe中有了）。至于用户可以接触到的对象，这个标记后面可能会接一个冒号和标识符。标识符将操作某一个未保护的对象的所有函数分为一组；用户可以创建一个与该标识符相关的非递归的互斥锁，并在调用任何带有该竞态标识符的函数时始终持有该互斥锁，来规避未保护访问内部对象的MT-Unsafe问题，和用户可操作对象时相同。非递归互斥锁避免了MT-Safe问题，但是他换到了一个AS-Safe问题，因此在异步信号处理程序中使用此类函数的行为仍然是未定义的。

  当该标识符与存放返回值的静态缓冲区有关时，调用者必须在整个静态缓冲区的生命周期中持有互斥锁。许多返回静态缓冲区的指针的函数都提供了可重入版本，这些版本把返回值保存到用户提供的缓冲区中。有些情况下，比如tmpname，这个可重入版本不是通过写一个新的函数入口实现，比如tmpname\_r，而是通过传入一个保存返回值的非空指针实现的。这些可重入版本优先，在多线程程序中，虽然有些不是MT-Safe，因为使用了其他内部缓冲区，文档也标注了race。

- const

  标记const的函数意味着MT-Safe的问题，他们以非原子方式修改内部对象，但这些应该是常量，因为相当一部分函数访问这些对象时未进行同步。不像race，这个标记只会影响写。写还是有MT-和AS-Unsafe，但是他们修改的对象随后保持不变，这让写变得MT-Safe和AS-Safe（只要没其他问题）。

  读的安全说明说，const标记后的标识符将单独出现。写的时候若想规避安全问题，可以使用一个与标识符有关的非递归的读写锁。锁解决了MT-Safety问题，但是带来了一个AS-Safety问题，所以在异步信号处理中运行仍是未定义的。

- sig

  标记sig的函数意味着有MT-Safety问题（也有相同的AS-Safety问题，为了简洁而省略）可能会因为内部目的临时安装信号处理器，从而干扰该信号的其他用途，受影响的信号会在冒号后列出。

  调用期间，若信号没有其他用途，就没有安全问题。建议，在使用同一个临时信号的所有函数上加一把非递归的互斥锁；并在调用前阻塞该信号，调用后在重置其信号处理程序。

  在发生异步取消时，无法保证原始信号处理程序可以恢复，所以标记的函数也是AC-Unsafe。

  除了为规避MT-和AS-Safety问题而建议的措施外，为避免取消问题，还建议禁用异步取消，并安装一个清理处理程序，用户将信号恢复至预期状态以及释放互斥锁。

- term

  标记term的函数意味着MT-Safety问题，可能以推荐的方式更改终端设置；调用tcgetattr，修改某些标志位，然后再调用tcgetattr；这样创建一个时间窗口，其他线程的修改将丢失。因此，MT-Unsafe。同样也会让异步信号处理程序的修改丢失。这些函数也是AS-Unsafe，但是因为重复冗余而省略。

  因此，建议不要在信号处理程序中运行相关代码或者屏蔽可能用到的信号，并在和终端交互时上锁，以避免并发和可重入访问终端。这个锁也用于race:tcattr(fd)互斥，fd代表控制终端的文件描述符。调用者应该使用单一的全局互斥锁，或每一个终端一个互斥锁，即使该终端被不同的文件修饰符引用。

  标记term的函数有AC-Safety问题，应该在退出时将终端设置恢复成原来的状态，但是取消他们可能会导致无法恢复。

  除了为规避MT-和AS-Safety问题而建议的措施外，为了避免取消问题，还建议禁用异步取消，并安装一个清理处理程序，将终端设置恢复至原来的状态以及释放互斥锁。

#### 1.2.2.4 其他安全标记

函数还可能有其他关键字，指出不会导致函数调用不安全的特性，但是但是在特定类型的程序中可能需要加以考虑。

- locale

  标记locale的函数有MT-Safety问题，在读取locale对象时没有采用任何同步机制。在执行期间，标记locale的函数在locale对象改变时并发的调用，可能不会操作对应的任何locale，而是表现为这些locale之间不可预测的混合状态。

  然而，我们不曾标记那些函数为MT-和AS-Unsafe，因为那些会修改locale对象的函数被标记了const:locale并被视为不安全。既然是不安全的，后者不应该在多线程或者异步信号启用时调用，因此locale可以被视为常量，这让前者（不会修改locale对象的函数）变得安全了。

- env

  标记env的函数有MT-Safety的问题，在通过getenv或类似的方式访问环境时，在并发修改情况下，没有任何保护措施确保安全。

  然而，我们不曾标记那些函数为MT-和AS-Unsafe，因为那些会修改environment对象的函数被标记了const:env并被视为不安全。既然是不安全的，后者不应该在多线程或者异步信号启用时调用，因此environment对象可以被视为常量，这让前者（不会修改environment对象的函数）变得安全了。

- hostid

  标记hostid的函数有MT-Safety问题，从持有机器的host ID的系统级数据结构读取。这些数据结构通常无法以原子的修改。因为host ID通常不会改变，读取他的函数（gethostid）被视为安全的，而修改他的函数被标记了const:hostid，表达了需要小心可能会被异步取消。在这种特殊的情况下，特殊的小心意味着需要进行系统级（而非仅限进程内）的协调。

- sigintr

  标记sigintr的函数有MT-Safety问题，并发修改_sigintr内部数据结构情况下，没有任何保护措施确保安全。

  然而，我们不曾标记那些函数为MT-和AS-Unsafe，因为那些会修改_sigintr对象的函数被标记了const:sigintr并被视为不安全。既然是不安全的，后者不应该在多线程或者异步信号启用时调用，因此_sigintr对象可以被视为常量，这让前者（不会修改_sigintr对象的函数）变得安全了。

- fd

  标记fd的函数有AC-Safety问题，被异步取消程序打断执行，可能泄露文件描述符。

  分配或释放文件描述符的函数通常会被标记为fd。即使这些函数试图通过清理区域（cleanup regions）来保护文件描述符的分配和释放过程，分配和释放也不能原子化的执行。类似的，释放描述符并将其从原本负责释放他的数据结构中一处，也不能原子化的执行。总会有一个时间窗口，描述符，因还没有储存在清理程序的参数，无法被释放的时间窗口，或者在释放之前就从数据结构中移除了。释放之后再移除也是不行的：一个非空的描述符，可能确实需要被关闭，也有可能该地址被其他线程或信号处理程序再分配了。

  这类泄露本可以再库内部通过临时禁用异步线程取消来避免，但这会带来一定的性能开销。然而，因为分配和释放函数的调用者应该会自行处理这类泄露，因此对库而言，更合理的做法是假定调用者已自行处理，而不是在上层已解决问题的情况下，库再多余的施加性能开销，而在上层未解决问题时，库内部的防护又不足以彻底消除风险。

  这个标记本身不能让某个函数被视为AC-Unsafe。然而，此类泄露的积累效应可能会对某些程序造成问题，如果确实存在这种情况，建议在调用此类函数期间暂停异步取消。

- mem

  标记mem的函数有AC-Safety问题，在被异步线程打断执行时，有可能泄露内存。

  这个问题和文件描述符很像：不存在一种原子接口，能够在不禁用异步取消的情况下，用来分配内存并保存地址到清理处理程序的参数中，或既释放其内存并移除其地址从清理处理程序的参数中；而这些还是本身不会禁用异步取消。

  这个标记本身不能让某个函数被视为AC-Unsafe。然而，此类泄露的积累效应可能会对某些程序造成严重影响，如果确实存在这种情况，建议在调用此类函数期间暂停异步取消。

- cwd

  标记cwd的函数有MT-Safety问题，在执行期间可能会临时修改当前工作目录（current working directory），这可能会导致相对路径以非预期的解析，在其他线程中，或在异步信号处理和异步取消程序中。

  这个原因不足以让一个程序被视为MT-或AS-Unsafe，但是该行为是可选功能时（比如nftw函数中的FTW_CHDIR选项），避免使用该选项往往是一个不错的替代方案，可免去使用绝对路径名或基于文件描述符的相对路径系统调用（如openat）的麻烦。

- !posix

  当这个标记作为函数的MT-，AS-或AC-Safety说明出现时，表示该函数的实际安全状态与POSIX标准中规定的状态存在差异。比如POSIX不需要某个函数是安全的，但我们的实现是安全的，反之亦然。

  目前而言，就算没有标记!posix，也不意味着我们文档的安全属性和POSIX对应函数的要求完全一致。

- :identifier

  注释有时后面会附带标识符，其用途一是将若干函数归为一组，比如，那些以不安全方式访问数据结构的函数，如race和const，二是提供更具体的信息，比如，在标记为sig的函数中指明具体的信号名称。预计将来这一机制也可能应用与lock和corrupt。

  在大多数情况下，标识符用于命名一组函数，但也用于全局对象或函数参数，或与他们相关的可识别属性或逻辑组件，比如：buf(arg)用来表示与参数arg关联的缓冲区，或:tcattr(fd)用来表示文件描述服fd的终端属性。

  标识符最常见的用法是提供函数和参数的逻辑分组，这些函数和参数需要相同的同步原语保护，以保证在特定的环境下的安全操作。

- /condition

  有些安全注释可能是有条件的，当一个包含参数，全局变量或底层内核的布尔表达式为真时才起作用。比如，/hurd或/!linux!bsd分别表示当底层内核为HURD时，或当内核既不是Linux，也不是BSD时，/hurd或/!linux!bsd前面的标记才起作用。/!ps和/one_per_line表示前面的标记只有在ps为NULL，或全局变量one_per_line是非零的才起作用。

  当函数所有不安全标记都附加了条件，并且这些条件都不成立时，函数被视为安全的。

### 1.2.3 Berkeley Unix（Berkeley是专有名词）

The GNU C Library定义了一些功能，这些功能设施并不是标准，来自4.2 BSD，4.3 BSD和4.4 BSD Unix系统（也就是Berkeley Unix），还有SunOS（一个受欢迎的4.2 BSD的衍生版本，包含一些Unix System V的功能）。这些系统支持大多数ISO C和POSIX功能，4.4BSD和更新的SunOS系统事实上完全支持。

BSD功能包括符号链接（参考14.6 Symbolic Links），select函数（参考13.9 Waiting for Input or Output），BSD信号函数（参考25.10 BSD Signal Handling），sockets（参考16 Sockets）。

### 1.2.4 SVID (The System V Interface Description)

The System V Interface Description (SVID)是一份描述AT&T Unix System V操作系统的文档。他一定程度是POSIX的超集（全职猎人POSIX (The Portable Operating System Interface)）。

The GNU C Library为了兼容System V Unix和其他Unix系统（比如SunOS）定义了大部分SVID要求的功能，但不包含ISO C或POSIX中已经有的部分。然而，许多较为冷门且实用性不强的功能并未包含。（事实上，Unix System V本身也未完全提供这些功能。）

System V中支持的功能有，进程间通信与内存共享，hsearch和drand48系列函数，fmtmsg和若干数学函数。

### 1.2.5 XPG (The X/Open Portability Guide)

The X/Open Portability Guide，由X/Open有限公司出版，是一个比POSIX更广泛的标准。X/Open拥有Unix版权，XPG明确了想要称为Unix系统所需要的东西。

The GNU C Library遵循X/Open Portability Guide，第4.2版，还有XSI (X/Open System Interface)兼容系统所共用的扩展功能，以及全部X/Open UNIX的扩展功能。

在POSIX基础之上新增的功能，主要是System V和BSD系统已有的特性衍生出来的。不过，System V中真正的错误已经被修正。因为满足XPG标准和Unix扩展是活的Unix品牌认证的前提条件，这些功能大概率也会出现在商业Unix系统上。

### 1.2.6 Linux (The Linux Kernel)

The GNU C Library援引收录了Linux手册页6.9.1版本文档，记录了Linux内核的系统调用的用法。仅供参考，最新的Linux手册项目文档可以在Linux内核官网访问。若某一系统调用在本手册中有更具体的文档，则以更具体的文档为准。

在本手册中，当提到手册页时，比如：

&nbsp;&nbsp;&nbsp;&nbsp;sendmsg(2)参考Linux (The Linux Kernel)。

我们此处提及的手册页，主要指上文标注的特定版本（即“规范”版），通常，在安装了该版本的系统上，可以通过命令（比如）man 2 sendmsg访问。为方便起见，我们也会提供最新版在线手册，但请注意，在线版几乎总是比“规范”版更新，因此内容也会有所差异。

有关Linux系统调用接口的更多内容，可在System Calls（26.6）查看。

## 1.3 使用库

本节描述了在使用the GNU C Library涉及的一些实际问题。

- Header Files
- Macro Definitions of Functions
- Reserved Names
- Feature Test Macros

### 1.3.1 头文件

C程序调用的库由两部分构成：一，定义类型和宏，声明变量和函数的头文件；二，实际库文件，或归档文件，包含变量和函数的定义。

（回顾一下，在C语言中，声明只是提供函数或变量存在的信息，并给出他们的类型。关于函数的声明，他的参数的类型也会提供。声明的作用是让编译器正确的处理这些变量和函数的引用。定义则不同，定义确实的给变量分配了空间，或者给出了函数的具体内容。）

为了使用the GNU C Library中的功能，你应该确保你的源文件包含正确的头文件。这样编译器就可以访问这些功能的声明，并且可以正确的处理他们的引用。当你的程序被编译，链接器会解析引用到归档文件（实际的库文件）提供的定义上。

头文件通过“#include”预处理指令引入到源文件中。C语言支持两种这种指令的形式；第一种，
```c
#include "header"
```
通常用于引入你自己写的头文件；这个会包含你的项目的各个部分的接口之间的定义和声明。对应的，
```c
#include<file.h>
```
通常用于引入标注库的头文件file.h，包含了定义和声明。这个文件一般被系统管理员安装到标准目录中。你应该使用第二种形式来引用C标准库的头文件。

通常，“#include”指令应该放在源文件顶部，在所有代码之前。如果你的源文件在开头放了一些解释这个文件中有什么代码的注释（好习惯），将“#include”指令紧跟在后面，并跟在功能测试宏定义后面（参考1.3.4 Feature Test Macros）。

有关使用头文件和“#include”指令的更多信息，请参考The GNU C Preprocessor Manual中的Header Files章节。

The GNU C Library提供多个头文件，每个都包含了与一组相关功能有关的类型和宏定义，变量和函数的声明。这意味着你的程序可能需要引入多个头文件，取决于你具体要使用哪些功能。

有些库头文件自动引入了其他库头文件。然而，从编程风格角度，你不应该以来这个；最好显式引用你所使用的功能需要的所有的头文件。The GNU C Library头文件也是这样写的，不过不小心引用了多次相同的头文件没什么关系；第二次引用相同的头文件没有任何作用。同理，当你的程序需要使用引用多个不同头文件，引用他们的顺序并不重要。

兼容性说明：在任何ISO C实现中，按任何顺序引用任何标准头文件，引用任意次数，都能正常运行。然而，传统上并非如此，在一些老的C实现中。

严格来说，你没必要为了使用一个头文件中声明的函数去引用那个头文件；你可以自己显式的声明那个函数，根据规范。但是，通常，更建议引用头文件，因为头文件可能定义了类型和宏，而其他地方没有，又，头文件可能给某些函数定义了更高效的宏替代版本。使用头文件也是一种确保声明无误的做法。

### 1.3.2 函数的宏定义

如果本手册描述了某个函数，那么他有可能也有宏定义。这通常不会影响程序的运行——宏定义和函数功能相同。特别的，库函数的宏等价形式对参数的求值方式与函数调用完全一致，即每个参数仅被求值一次。有时候，宏定义能生成内联展开代码，比实际的函数调用快。

对一个函数的宏定义取地址是可行的。因为在这种情况下，函数名后面面没有紧跟左括号，而左括号是语法上识别宏调用所必须的。

你可能偶尔想避免使用函数的宏定义——可能会让你的程序的调试更简单。这有两个办法：

- 若你只想在某次特定调用中避免使用宏定义，可以将函数名用括号括起来。之所以有用，是因为这种语法背景下的函数名不会被识别为宏调用。

- 若你想在整个源文件中禁用某个宏定义，可以通过“#undef”预处理指令，除非该功能有其他明确规定。

例如，假设头文件stdlib.h中声明了一个函数abs

```c
extern int abs (int);
```

同时提供了abs的宏定义。然后：

```c
#include <stdlib.h>
int f (int *i) { return abs (++*i); }
```

对abs的引用可能是宏定义，也可能是函数。另一方面，下面的例子是对函数的引用，而不是宏定义。

```c
#include <stdlib.h>
int g (int *i) { return (abs) (++*i); }
```

```c
#undef abs
int h (int *i) { return abs (++*i); }
```

由于兼作函数的宏定义在行为上与真正的函数版本完全一致，因此通常没有必要使用上述任何一种方法。通常，移除宏定义会让你的程序更慢。

### 1.3.3 保留名

所有来自ISO C标准的库类型，宏定义，变量，函数名都被无条件保留；你的程序不能重定义这些名称。你显式引入的所有其他库中定义或声明的名称，也被保留。这个限制有多个原因：

- 举个例子，若你使用你命名的exit函数去做和标准exit函数的功能完全无关的事，别人看你的代码时会非常困惑。防止这种情况发生有助于提高程序的可读性，促进模块化和提高可维护性。

- 这避免了其他库函数需要调用的库函数被用户意外的重定义了。如果重定义被允许了，其他函数可能无法正确运行。

- 这允许编译器对这些函数的调用进行任意的优化，而无需担心他们被用户重定义。有一些库的功能，比如处理可变参数（参考附录A.2 Variadic Functions）和非本地退出（参考24 Non-Local Exits），实际上需要C编译器的大量配合，从实现角度考虑，编译器将他们视为这些语言的内置部分往往更简单。

除了这个手册中记录的名称外，保留名还包括所有以下划线（“_”）开头的外部标识符，和无论作何种用途的以两个下划线或一个下划线开头接一个大写字母的标识符。这样保证了库和头文件可以定义函数，变量和宏定义处理内部的需求，而不会与用户的程序名称冲突。

还有一类标识符被保留，为了未来C语言的扩展或POSIX.1环境。虽然在你的程序中使用这些标识符现在不会有问题，但是未来可能有，所以你应该避免使用他们。

- 以大写字母“E”开头，后接数字或大写字母的名称可能用于新增的错误码。参考2 Error Reporting。

- 以“is”或“to”开头，后接小写字母的名称可能用于新增的字符检查和转换函数。参考4 Character Handling。

- 以“LC_”开头，后接一个大写字母的名称可能用于新增的指定区域属性的宏定义。参考7 Locales and Internationalization。

- 所有以现存的数学函数（参考19 Mathematics）开头，后接“f”或“l”的名称都被保留，对应操作float和long double参数的函数。

- 以“SIG”开头，后接一个大写字母的名称可能用于新增的信号名称。参考25.2 Standard Signals。

- 以“SIG_”开头，后接一个大写字母的名称都被保留，可能用于新增的信号操作。参考25.3.1 Basic Signal Handling。

- 以“str”，“mem”或“wcs”开头，后接一个小写字母的名称都被保留，可能用于新增的字符串和数组函数。参考5 String and Array Utilities。

- 以“_t”结尾的名称都被保留，可能用于新增的类型名称。

In addition, some individual header files reserve names beyond those that they actually define. You only need to worry about these restrictions if your program includes that particular header file.此外，一些个特定的头文件会保留超出其定义范围之外的名称。只有你的程序包含了特定的头文件时，你才需要考虑这些限制。

- 头文件dirent.h保留了以“d_”开头的名称。

- 头文件fcntl.h保留了以“l_”，“F_”，“O_”和“S_”开头的名称。

- 头文件grp.h保留了以“gr_”开头的名称。

- 头文件limits.h保留了以“_MAX”开头的结尾的名称。

- 头文件pwd.h保留了以“pw_”开头的名称。

- 头文件signal.h保留了以“sa_”和“SA_”开头的名称。

- 头文件sys/stat.h保留了以“st_”和“S_”开头的名称。

- 头文件sys/times.h保留了以“tms_”开头的名称。

- 头文件termios.h保留了以“c_”，“V”，“I”，“O”和“TC”开头的名称；还有以“B”开头，后接一个数字的名称。

### 1.3.4 功能测试宏

编译源文件时，可用的功能集合，取决于你定义了哪些功能测试宏。

如果你用“gcc -ansi”编译你的程序，你只能用ISO C库的功能，除非你通过定义一个或多个功能测试宏，显式需求了额外的特性。参考The GNU CC手册中GNU CC命令选项（外部链接），以了解更多GCC选项的更多信息。

你应该在你源文件的顶部，通过使用“#define”预处理指令，来定义这些宏。这些指令必须在#include之前。最好让他们在文件的非常非常前面，你也可以使用GCC的“-D”选项，但是更好的办法是让你的源文件自己内部包含的表明他们自己的依赖需求。

这套机制的存在，是为了使库兼容多个标准。虽然不同的标准总是被描述为彼此的超集，他们通常互不兼容，因为大的标准需要的函数名，较小的标准把他们保留给客户使用了。这并非吹毛求疵——在实际应用中确实层引发过问题。例如，一些非GNU程序定义了名为getline，和本库函数中的getline毫无关系。如果不加区分的启用所有功能，这些程序将无法通过编译。

这不应被用来验证程序是否符合某个标准。这不能达到这个目的，因为他无法保护你引入标准之外的头文件，或者依赖标准未定义的语意。

宏：_POSIX_SOURCE

<div style="margin: 0 0 0 3em;">
如果你定义了这个宏，POSIX.1标准和ISO C标准的功能都支持。
</div>

<div style="margin: 0 0 1em 3em;">
如果你将宏_POSIX_C_SOURCE定义为一个正数，_POSIX_SOURCE的状态无关紧要。
</div>

宏：_POSIX_C_SOURCE

<div style="margin: 0 0 0 3em;">
将此宏定义为一个正数，以控制哪些POSIX功能可用。数值越大，可用的功能就越多。
</div>

<div style="margin: 0 0 0 3em;">
大于等于1时，1990版POSIX.1标准。
</div>

<div style="margin: 0 0 0 3em;">
大于等于2时，1992版POSIX.2标准。
</div>

<div style="margin: 0 0 0 3em;">
大于等于199309L时，1993版POSIX.1b标准。
</div>

<div style="margin: 0 0 0 3em;">
大于等于199506L时，1995版POSIX.1c标准。
</div>

<div style="margin: 0 0 0 3em;">
大于等于200112L时，2001版POSIX标准。
</div>

<div style="margin: 0 0 0 3em;">
大于等于200809L时，2008版POSIX标准。
</div>

<div style="margin: 0 0 0 3em;">
大于等于202405L时，2024版POSIX标准。
</div>

<div style="margin: 0 0 1em 3em;">
更大的_POSIX_C_SOURCE值将启用未来的扩展。POSIX标准会根据需要，定义这些值，在他们成为标准后的一段时间后，the GNU C Library应该会支持他们。1996版POSIX.1标准规定，如果将_POSIX_C_SOURCE定义为大于或等于199506L的值，即可启用1996版的功能。一般来说，在the GNU C Library中，对标准的bug修复会包含在对应的版本中；比如，200112L包含了POSIX.1-2004的内容。
</div>

宏：_XOPEN_SOURCE

宏：_XOPEN_SOURCE_EXTENDED

<div style="margin: 0 0 0 3em;">
如果你定义了这个宏，XPG（X/Open Portability Guide）的功能将会被引入。这是POSIX.1和POSIX.2功能的超集，事实上，_POSIX_SOURCE和_POSIX_C_SOURCE会自动定义。
</div>

<div style="margin: 0 0 0 3em;">
作为对所有Unix系统的统一，该宏还包含了仅在BSD和SVID中可用的功能。
</div>

<div style="margin: 0 0 0 3em;">
如果_XOPEN_SOURCE_EXTENDED也被定义了，会有更多可用的函数。这使X/Open Unix品牌认证所需要的所有功能函数都可用了。
</div>

<div style="margin: 0 0 0 3em;">
If the macro _XOPEN_SOURCE has the value 500 this includes all functionality described so far plus some new definitions from the Single Unix Specification, version 2. The value 600 (corresponding to the sixth revision) includes definitions from SUSv3, and using 700 (the seventh revision) includes definitions from SUSv4. The value 800 includes definitions from POSIX.1-2024.
</div>