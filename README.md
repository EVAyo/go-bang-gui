# GoBangGUI

#### 介绍
- C++编写的五子棋，采用面向对象方式实现。在VS2019上成功运行。 采用QT实现界面(已做)，功能有本地双人对战(已做)，本地人机对战(在做)，局域网匹配对战(已做)。
- 项目同时发布在[吾爱破解](https://www.52pojie.cn/?694168)论坛，白小飞V(UID: 694168)。



#### 软件架构
软件架构说明
1. 游戏逻辑和数据通过类class Game{}实现
2. 界面逻辑和互动通过类class GoBangWidget{}实现

#### 安装教程

1.  main()函数内创建QApplication 对象
2.  在QApplication 对象上创建GoBangWidget 对象
3.  所有界面都以GoBangWidget 对象为父类
4.  界面上通过点击操作调用Game 对象的各个成员函数
5.  GoBangWidget 对象负责绘制Game 对象返回的数据

#### 使用说明

1.  导入Qt5.12.12工程
2.  编译运行
3.  按界面提示开始游戏

#### 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request


#### 特技

1.  使用 Readme\_XXX.md 来支持不同的语言，例如 Readme\_en.md, Readme\_zh.md
2.  Gitee 官方博客 [blog.gitee.com](https://blog.gitee.com)
3.  你可以 [https://gitee.com/explore](https://gitee.com/explore) 这个地址来了解 Gitee 上的优秀开源项目
4.  [GVP](https://gitee.com/gvp) 全称是 Gitee 最有价值开源项目，是综合评定出的优秀开源项目
5.  Gitee 官方提供的使用手册 [https://gitee.com/help](https://gitee.com/help)
6.  Gitee 封面人物是一档用来展示 Gitee 会员风采的栏目 [https://gitee.com/gitee-stars/](https://gitee.com/gitee-stars/)
