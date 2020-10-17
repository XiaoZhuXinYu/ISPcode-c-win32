# ISPcode-c
isp参考代码，通过C语言实现

# 20200605 13:15
项目说明：这是一个本人自己编写的ispcode，通过C语言实现。
代码部分参考之前的isp-python项目。以及网上大神的一些博客，opencv1.0源码等。
编译软件采用QT5.12.4,编译器选择MinGW_32_bit。代码基本在PC和嵌入式设备上都可以跑通。
函数中的注释会说明了两者的效率差别。本项目主要是自己学习的一个积累，之后会慢慢完善。
如果你觉得对你有帮助，欢迎下载。如果代码中有错误的地方，也欢迎指正。

# 20201017 13:44
1.  增加了 cj_histogram.c 文件，实现了直方图均衡化相关函数(HE CLHE AHE CLAHE)，主要参考了https://www.cnblogs.com/jsxyhelu/p/6435601.html?utm_source=debugrun&utm_medium=referral
但是最后 限制对比度的自适应直方图均衡 跟opencv的还有差距。具体问题还不知道。
2. 做了一下文件名的修改，添加了gitignore文件。
