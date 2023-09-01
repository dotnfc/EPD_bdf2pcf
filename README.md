# EPD_bdf2pcf

改进的 bdf2pcf 工具。

原始的 pcf 字体文件，保留了一些对于 FreeType2 渲染所不需要的组件、数据，为了能在嵌入式环境下使用这种字体文件，就把把他们在输出的时候剔除掉。
可以参考: bdf2pcf/pcf.h

```
#define PCF_OUT_MASK___F        (PCF_BITMAPS | PCF_METRICS | 0 | PCF_PROPERTIES)
#define PCF_OUT_MASK__F_        (0 | 0 | PCF_BDF_ENCODINGS | 0)
#define PCF_OUT_MASK_1__        (PCF_BDF_ACCELERATORS)
#define PCF_OUT_MASK            (PCF_OUT_MASK_1__ | PCF_OUT_MASK__F_ | PCF_OUT_MASK___F)
```

此外，FreeType2 在加载 pcf 文件的时候，会直接将 PCF_BDF_ENCODINGS (通常是 256 * 256 * 2) 加载到 RAM 中。我们的小字库，通常没有 256 * 256 那么多，为减小文件，就将 pcf 文件的这个组件做了 lz4 压缩，加载的时候，直接释放到 RAM 中。

# 如何编译
打开 msvc/ 目录下的工程文件编译链接，或者打开 VS 的开发者命令行执行下面的命令

```
msbuild /p:Configuration="Release" bdf2pcf.vcxproj
```

# 如何使用

见 example/conv.cmd 

```
bdf2pcf.exe -z -d chardict.txt -o wenquanyi-zh-16.pcf wenquanyi-zh-16.bdf
```

其中 -z 是压缩 PCF_BDF_ENCODINGS 组件。

wenquanyi-zh-16.bdf 是开源的[文泉驿点阵字体](http://wenq.org/wqy2/index.cgi?BitmapSong)，非常适合墨水屏显示，美观、大方。当然，使用 [unifont](https://unifoundry.com/unifont/index.html) 的 unifont-15.0.06.bdf.gz 也是可以的。


# Reference
> [lz4-dev@rev_e846165](
https://github.com/lz4/lz4/commit/e8461657c54b8a918e931d3e3522c0976dbe5feb)

> [bdftopcf](https://gitlab.freedesktop.org/xorg/util/bdftopcf/-/archive/master/bdftopcf-master.tar.gz)

> [web-bdftopcf](https://github.com/adafruit/web-bdftopcf/)