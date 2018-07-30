# database
database source code analysis and thinking

## mysql-server first 2 commits: https://github.com/mysql/mysql-server
```
$ git log
commit f4c589ff6c653d1d2a09c26e46ead3c8a15655d8 (HEAD)
Author: bk@work.mysql.com <>
Date:   Mon Jul 31 21:29:14 2000 +0200

    Import changeset

commit 7eec25e393727b16bb916b50d82b0aa3084e065c
Author: bk@work.mysql.com <>
Date:   Mon Jul 31 21:10:05 2000 +0200

    Initial repository create

```

## Postgres first 4 commits: https://github.com/postgres/postgres

```
commit e72ca17f77c4acfc4d50926ac30d28a6283127f3 (HEAD -> jumping-test)
Author: Marc G. Fournier <scrappy@hub.org>
Date:   Fri Jul 12 04:53:59 1996 +0000

    fixes for several potential null pointer dereferences

    submitted by: Paul "Shag" Walmsley <ccshag@cclabs.missouri.edu>

commit 950b6ab02272057811dcb4cdf2edcdc2f01b81b7
Author: Marc G. Fournier <scrappy@hub.org>
Date:   Tue Jul 9 06:39:19 1996 +0000

    Fixes: Using LIKE or ~ operator on text type files which are null valued
           causes segmentation fault.

    Thanks to: Salvador Ortiz Garcia, Robert Patrick, Paul 'Shag' Walmsley,
               and James Cooper for finding and fixing the problem.

commit 25bb71835f1d3c3ab85cfa33dbdf95419d003c2a
Author: Marc G. Fournier <scrappy@hub.org>
Date:   Tue Jul 9 06:35:38 1996 +0000

    Fix: Can't drop tables with names longer than 16 characters.

commit d31084e9d1118b25fd16580d9d8c2924b5740dff (tag: PG95-1_01)
Author: Marc G. Fournier <scrappy@hub.org>
Date:   Tue Jul 9 06:22:35 1996 +0000

    Postgres95 1.01 Distribution - Virgin Sources

```

## sqlite: https://www.sqlite.org/src/info/6f3655f79f9b6fc9

## Contents
**sqliteInt.h:** Internal interface definitions for SQLite.

**sqlite.h:** The interface that the sqlite library presents to client programs.

**dbbe.h:** The interface to the database backend (Dbbe).

**vdbe.h:** The interface to the Virtual DataBase Engine (VDBE)

**util.c:** Utility functions used throughout sqlite, and it contains functions for allocating memory, comparing strings, and stuff like that.

**tokenize.c:** C code that splits an SQL input string up into individual tokens and sends those tokens one-by-one over to the parser for analysis.

**tclsqlite.c:** A TCL Interface to SQLite

**build.c:** C code routines that are called by the parser when syntax rules are reduced.

**dbbe.c:** Implement the database baseend (DBBE) for sqlite.  The database backend is the interface between sqlite and the code that does the actually reading and writing of information to the disk.

**vdbe.c:** Implements the Virtual Database Engine (VDBE)

**shell.c:** implement the "sqlite" command line utility for accessing SQLite databases.

**main.c:** Main file for the SQLite library.  The routines in this file implement the programmer interface to the library.  Routines in other files are for internal use by SQLite and should not be accessed by users of the library.

**where.c:** This module contains C code that generates VDBE code used to process the WHERE clause of SQL statements.  Also found here are subroutines to generate VDBE code to evaluate expressions.

## 目录
**sqliteInt.h：** SQLite的内部接口定义。

**sqlite.h：** sqlite库提供给客户端程序的接口。

**dbbe.h：** 数据库后端的接口（Dbbe）。

**vdbe.h：** 虚拟数据库引擎（VDBE）的接口

**util.c：** 在整个sqlite中使用的实用程序函数，它包含用于分配内存，比较字符串和类似内容的函数。

**tokenize.c：** 将SQL输入字符串拆分为单个标记并将这些标记逐个发送到解析器进行分析的C代码。

**tclsqlite.c：** SQLite的TCL接口

**build.c：** 语法规则减少时解析器调用的C代码例程。

**dbbe.c：** 为sqlite实现数据库baseend（DBBE）。数据库后端是sqlite和实际读取和写入磁盘信息的代码之间的接口。

**vdbe.c：** 实现虚拟数据库引擎（VDBE）

**shell.c：** 实现“sqlite”命令行实用程序，用于访问SQLite数据库。

**main.c：** SQLite库的主文件。此文件中的例程实现了库的程序员接口。其他文件中的例程供SQLite内部使用，不应由库用户访问。

**where.c：** 此模块包含生成用于处理SQL语句的WHERE子句的VDBE代码的C代码。此处还可以找到生成VDBE代码以评估表达式的子例程。

