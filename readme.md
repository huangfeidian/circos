# Circos in C++
本项目被启发自 [circos!](http://circos.ca)，写这个项目主要是觉得circos的图挺好看啦，但是我为什么要装perl 啊，我可是万能的C++程序员。然后就撸起袖子开干了。
##本程序所支持功能
在本程序的设计中，所有信息的承载者是环(circle)，而一个circle又由一个或多个band组成，相邻的两个band之间由的间距相等，其总间距的。circle声明语法如下。
`circle circle_id(string) inner_radius(int) outer_radius(int) gap(int) color(RGB) optional(XXX=YYY)`
其中开头的circle是固定的，circle_id代表的是当前环的唯一标识符，gap指的是在同一个circle的不同band之间的所有空隙的空间之和。inner_radius 和outer_radius分别代表当前环的内侧半径和外侧半径。color是环的颜色，其形式为RGB(xxx,yyy,zzz)。而optional代表的是所有可选的选项的设置值。
band的声明语法见下：
`band circle_id(string) band_id(string) band_begin(int) band_end(int) color(RGB) optional(XXX=YYY)`
circle_id代表的是该band所从属的circle band_id是当前band的唯一标识符。band_begin 和band_end 代表的是该band的位置区间，不同的band的区间不做任何限制。这里只在乎band_end-band_begin的差值，这是用来计算band在当前circle中所占的长度的.这里gap的距离度量是按照inner_radius来算的，所有的band的弧度之和为2*PI*inner_radius-gap,而各个band的长度正比于其band_end-band_begind的值。同一个circle的band按照其声明顺序排列。
在band上我们有四种类型的onband：一种是fill(简单填充)一种是tick（刻度），一种是label（文字），一种是value（带上数据）。原始的circos中的刻度尺需要tick和text一起配合才能表示。
fill 的声明如下
`onband band_id(string) begin(double) end(double) color(RGB)`
label 的声明如下
`onband band_id(string) begin(double) end(double) font_size(int) align(int) text(string) color(RGB)`
其中值得特殊说明的是align这个值，如果是0则是径向对齐，如果是1则是左对齐，如果是2则是居中。label的文字是画在circle外侧的。
tick 的声明如下
`onband band_id(string) begin(double) end(double) width(int) height(int)  color(RGB) `
width和height分别代表tick的宽度和高度，tick是画在circle外侧的。这里我并没有提供类似于原始circos的简单语法来做刻度尺相关的工作，每一个刻度我们都需要声明一次。
value的声明如下
`onband band_id(string) begin(double) end(double) value(double)  color(RGB) `
对于value的绘制方法，我的程序支持了热度填充、折线图、柱状图、点状图等。value_onband中并没有声明到底画哪一种图，需要在总体设置文件中写明类型。
上述各种类型的数据要求分离存放，不同类型的数据不能放在一个文件里。因为我们在总设置文件中是这样做的：
`type filepath=path`
最后贴一张我做的图啊啊啊，大家鼓掌。
