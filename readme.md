# Circos in C++
本项目被启发自 [circos!](http://circos.ca)，写这个项目主要是觉得circos的图挺好看啦，但是我为什么要装perl 啊，我可是万能的C++程序员。然后就撸起袖子开干了。
当前程序的功能简单来说： 通过从规定格式的xlsx文件里读取数据，并构造可视化表示，输出到png和svg文件中。
## 依赖
语言标准 c++ 17
1. [xlsx_reader](https://github.com/huangfeidian/xlsx_reader) 用来读取excel文件 这个依赖项顺带依赖了如下项目
    1. [tinyxml2](https://github.com/leethomason/tinyxml2) 用来解析xml文件
    2. [miniz](https://github.com/richgel999/miniz) 用来解压缩xlsx文件
    3. [typed_string](https://github.com/huangfeidian/typed_string) 用来读取格式化字符串
    4. [json](https://github.com/nlohmann/json) 用来输出json数据
2. [freetype2](https://github.com/aseprite/freetype2) 用来读取字体位图
3. [libpng](https://github.com/glennrp/libpng) 用来输出png文件
4. [zlib](https://github.com/madler/zlib) libpng依赖于zlib来输出png

## 读取excel
我们所读取的excel文件是带格式的自校验文件。这个格式由`xlsx_reader`里的`typed_worksheet`定义。每一列的数据都有如下格式：
1. 第一行 列名 一个没有空格的字符串
2. 第二行 列数据的格式描述，如`str, int, uint32, float, bool`，更多描述参照`typed_string`
3. 第三行 注释行 可以不填 暂时没有什么意义
4. 第三行以后的内容都是数据 每个数据都可以通过第二行里规定的格式来做检验

然后数据文件是按照分页来组织的，每个分页都带有自己的分页类型，每个分页类型都有自己特定的列格式， 每个分页的第一列都是标识符列，作为数据的索引，在所有同类型的分页数据中不得重复。除第一列外的其他列都可以任意调换顺序，程序访问的时候采取列名来访问，二不是excel里的列编号

而定义每个分页的分页类型则在`sheet_role`这个分页里，作为数据文件的入口。完整的分页类型及相关列定义参照data目录下的template.xlsx文件。

## 分页类型
1. sheet_tole 类型 规定所有数据分页的类型 第一列 分页名， 第二列规定分页类型
2. config 类型 里面定义一些全局配置，目前有背景颜色和图片半径
3. circle 类型 定义circos里的每个圆环相关数据
4. tile 类型 定义circos里的每个tile数据 每个tile都归属于某个circle， 同一个circle里的tile按照tile_id的文本比较进行排序，每个tile都有对应的width字段，而circle的总width则是所有附属于当前circle的tile的width的总和加上circle里定义的gap，每个tile之间的具体gap是等距离的
5. value_on_tile 每一个数据绑定了某个tile上的一个范围，及在此绑定范围上的值，这个值的表示类型依赖于这个value所绑定的track
6. track_config 定义value_on_tile上的数据如何绘制，同一个track上的数据永远都属于同一个tile，目前支持饼图、点图、折线图、面积图、柱状图、扇形图这几种，具体的参考data下的point_track_test.xlsx
7. point_link 代表两个tile上的位置之间的连线，这个连线可以是直线，也可以是贝塞尔曲线，有个控制半径的参数来控制 参考data文件夹下的pi10000.xlsx
8. range_link 代表两个tile上的两个区域之间的连接表示，可以以条带的形式表示，也可以以交叉条带的形式来表示
9. circle_tick 代表某个circle上是否需要绘制刻度及刻度的间距、长度宽度
10. tile_tick 代表某个tile上是否需要绘制刻度及刻度的间距、长度宽度

相关样例excel参考data目录下的xlsx文件。
## 使用说明
按照上述的格式要求准备好excel文件之后，我们可以用工程里的xlsx_circos可执行文件来处理数据并生成图。这个命令行工具有一个必选参数，一个可选参数。
1. 必选参数就是数据文件的路径 作为第一个参数
2. 可选参数就是生成的图片文件的类型， 如果不提供可选参数，则默认生成png和svg文件，如果只需要生成png文件，则提供`-png`,类似的只生成svg文件需要提供`-svg`。

生成的文件名是原始excel文件的文件名加上当前时间戳。

## 样图
这个是三年前第一版程序所生成的图了
![圆周率](https://raw.githubusercontent.com/huangfeidian/circos/master/picture/pi_custom.png)

由data文件夹下的text_test.xlsx生成的图
![文本绘制](https://github.com/huangfeidian/circos/blob/master/picture/text_test.png?raw=true)

由data文件夹下的point_track_test.xlsx生成的图
![track数据绘制](https://github.com/huangfeidian/circos/blob/master/picture/point_track_test.png?raw=true)

由data文件夹下的pi10000.xlsx生成的图
![pi数据绘制](https://github.com/huangfeidian/circos/blob/master/picture/pi10000.png?raw=true)
