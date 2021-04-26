# 分离逻辑公式求解器重构

## 前端重构

### 词法分析器

> 重构思路：
>
> ​	使用工厂设计模式将解析过程拆分。
>
> ​	原来解析过程在scanner中对输入流一直解析，所有解析函数被封装到scanner类中，缺点是不容易扩展，代码比较集中不便于修改。
>
> ​	重构思路将每一类Token的解析独立出去，方便阅读与扩展代码，比如新增加一类Token，只需新建一个类即可。
>
> 1. 字母分类
>
>    > character: a-z,A-Z,~ ! @ $ % ^ & * _ + = < > . ? /
>
>    > number: 0-9
>
>    > space: \r, \t
>
>    > special: ", ', 

> 2. TokenScanner 分类
>
>    > LeftParenthesis
>    >
>    > ​	标记: （
>    >
>    > ​	解析过程: empty
>    >
>    > ​	解析结果：Token*
>    >
>    > RightParenthesis
>    >
>    > ​	标记: )
>    >
>    > ​	解析过程: empty
>    >
>    > ​	解析结果：Token*
>    >
>    > StringLiteral
>    >
>    > ​	标记：“
>    >
>    > ​	解析过程： 解析到下一个 ” 
>    >
>    > ​	解析结果： StrToken*
>    >
>    > Symbol
>    >
>    > ​	标记：a 或者 |
>    >
>    > ​	解析过程：
>    >
>    > ​		a： 解析到第一个空字符结束
>    >
>    > ​		|： 解析到下一个|
>    >
>    > ​	解析结果： StrToken*
>    >
>    > Keyword
>    >
>    > ​	标记： ：
>    >
>    > ​	解析过程：解析到下一空字符
>    >
>    > ​	解析结果: StrToken*
>    >
>    > 
>    >
>    > NumberLiteral (FloatLiteral, IntLiteral)
>    >
>    > ​	标记： 0 或者 -
>    >
>    > ​	解析过程：下一个非数字字符
>    >
>    > ​	解析结果： FloatToken* 或者 IntToken* 或者 StrToken*
>    >
>    > Comment：
>    >
>    > ​	标记： ；
>    >
>    > ​	解析过程： 解析行尾
>    >
>    > ​	解析结果:    无

### 语法分析器

> 重构思路
>
> > 类似于词法分析重构，使用工厂模式，将不同命令的解析过程分离。

> 命令
>
> > set-logic
> >
> > declare-sort
> >
> > define-fun
> >
> > declare-fun
> >
> > check-sat
> >
> > get-model
> >
> > assert

> 组成部分
>
> > expr
> >
> > pto
> >
> > ref
> >
> > sref