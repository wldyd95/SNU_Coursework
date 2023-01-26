# GDB Cheat Sheet

* GDB (GNU Debugger) is a powerful tool for debugging your program. For more information, please refer to https://darkdust.net/files/GDB%20Cheat%20Sheet.pdf


Invoke gdb with giving the name of your executable file as an argument. Your program should be compiled with "`-Og`" or "`-g`" flag to be used with `gdb`.

```
$ gdb ./bmpdiag
GNU gdb (Ubuntu 7.11.1-0ubuntu1~16.5) 7.11.1
Copyright (C) 2016 Free Software Foundation, Inc.
...
Reading symbols from ./bmpdiag...done.
```

The `break` command is used to set the breakpoint in your code. The following shows how to set a breakpoint at the beginning of the function `bmp_diag()` which is located in the file `bmpdiag.s`.

```
(gdb) break bmp_diag
Breakpoint 1 at 0x400b50: file bmpdiag.s, line 33.
```

If you want to run your program, use the `run` command followed by any arguments as shown below. Because we have set a breakpoint in the entry of the function `bmp_diag()`, you can see that the execution stopped at that point (just before executing the first instruction in `bmp_diag()`).

```
(gdb) run bordeaux.bmp out.bmp 100
Starting program: /home/jinsoo/ca-pa3/bmpdiag bordeaux.bmp out.bmp 100
BMP file: bordeaux.bmp (640 x 359 pixels, 24 bits/pixel)

Breakpoint 1, bmp_diag () at bmpdiag.s:33
33              movb    $0x55, (%rdi)
```

Now you can browse the source code around the current breakpoint using the `list` command.

```
(gdb) list
29
30              # --> FILL HERE <--
31
32
33              movb    $0x55, (%rdi)
34              movb    $0x88, 1(%rdi)
35              movb    $0xff, 2(%rdi)
36
37
```

You can examine the content of a register using the `print` (or simply `p`) command as follows. Note that the name of the register is given as `$rax`, not `%rax`.

```
(gdb) print $rdi
$1 = 140737353273414
```

If you want to print the value in a hexadecimal format, use the `print/x` (or simply `p/x`) command. The `%rdi` register contains the first argument for `bmp_diag()`. Therefore, the following address `0x7ffff7f2d046` is the address that points to the bitmap data.

```
(gdb) print/x $rdi
$2 = 0x7ffff7f2d046
```

You can print the value of any register. The following shows the content of the `%rsi` register that has the second argument. You can see that it has the value `640` which is the width of the input image file.

```
(gdb) p $rsi
$3 = 640
```

The `%rcx` register is used to pass the fourth argument. In this case, it has the value `100` which is the `gap` between adjacent diagonal lines (in pixels).

```
(gdb) p $rcx
$4 = 100
```

You can print the content of the memory using the `x` command. Esepcially, you can print 8 bytes in a hexademical format using the `x/8b` command as shown below. If you want to see 32 bytes, use the `x/32b` command. Initially, the `%rdi` register points to the memory address of the first pixel in the last row of the given input image. The first three bytes, `(0x04, 0x04, 0x04)`, represents the color value for blue, green, and red for that pixel, respectively.

```
(gdb) x/8b $rdi
0x7ffff7f2d046: 0x04    0x04    0x04    0x03    0x03    0x02    0x04    0x03
```

The 'step' command executes one instruction at a time. Now it has executed the first instruction in the `bmp_diag()` function (e.g., `movb  $0x55, (%rdi)`), and shows the next instruction to be executed.

```
(gdb) step
34              movb    $0x88, 1(%rdi)
```

If you examine the memory contents pointed to by the `%rdi` register, you can see that the first byte now has been modified to `0x55` as a result of executing the first instruction.

```
(gdb) x/8b $rdi
0x7ffff7f2d046: 0x55    0x04    0x04    0x03    0x03    0x02    0x04    0x03
```

Now, let's execute the second instruction. Note that the instruction shown after using `step` command is the instruction to be executed next.

```
(gdb) step
35              movb    $0xff, 2(%rdi)
```

Again, you can see that the second byte has been changed to `0x88` as a result of executing the second instruction in `bmp_diag().`

```
(gdb) x/8b $rdi
0x7ffff7f2d046: 0x55    0x88    0x04    0x03    0x03    0x02    0x04    0x03
```

You can just use `s` for the `step` command. The following executes the third instruction in `bmp_diag().`

```
(gdb) s
bmp_diag () at bmpdiag.s:49
49              ret
```

Again, the third byte has been changed to `0xff`. In this way, you can modify the color values of any pixel you want.

```
(gdb) x/8b $rdi
0x7ffff7f2d046: 0x55    0x88    0xff    0x03    0x03    0x02    0x04    0x03
```

The memory address can be given in the following way also.

```
(gdb) x/8b $rdi+600
0x7ffff7f2d29e: 0x04    0x04    0x03    0x04    0x03    0x03    0x02    0x03
```

The `cont` command continues the execution until it meets another breakpoint or until the program terminates.

```
(gdb) cont
Continuing.
[Inferior 1 (process 8747) exited normally]
```

It is always a good idea to ask for a help to `gdb`.

```
(gdb) help
List of classes of commands:

aliases -- Aliases of other commands
breakpoints -- Making program stop at certain points
data -- Examining data
files -- Specifying and examining files
internals -- Maintenance commands
obscure -- Obscure features
running -- Running the program
stack -- Examining the stack
status -- Status inquiries
support -- Support facilities
tracepoints -- Tracing of program execution without stopping the program
user-defined -- User-defined commands

Type "help" followed by a class name for a list of commands in that class.
Type "help all" for the list of all commands.
Type "help" followed by command name for full documentation.
Type "apropos word" to search for commands related to "word".
Command name abbreviations are allowed if unambiguous.
```

You know what the following means.

```
(gdb) quit
$
```


Enjoy :)


[Jin-Soo Kim](mailto:jinsoo.kim_AT_snu.ac.kr)  
[Systems Software and Architecture Laboratory](http://csl.snu.ac.kr)  
[Dept. of Computer Science and Engineering](http://cse.snu.ac.kr)  
[Seoul National University](http://www.snu.ac.kr)
