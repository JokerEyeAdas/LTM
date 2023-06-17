# 基于直方图均衡的LTM实现
* reference：
  [参考论文链接](./arctile/tiant2012.pdf)

* how to compile?

  ```
   cd ./source
   mkdir build && cd build
   cmake ..
   make
  ```
 * how to debug?

  ```
   #define DEBUG 1 //enbale debug info in main.cpp
  ``` 
  ![debug historm image](./img/debug.png)
* run result

|index| 原始灰度图| 直方图均衡| LTM     |
| ----|-------- | -------- | -------- |
|0|![原图](./img/test1/ORI.png) |![全局均衡](./img/test1/EQ_HIST.png) | ![LTM](./img/test1/LTM.png) |
|1|![原图](./img/test2/ORI.png) |![全局均衡](./img/test2/EQ_HIST.png) | ![LTM](./img/test2/LTM.png) |

* QA：局部颜色单一产生band断层

| 断层图像     |
| --------|
|![原图](./img/band.png) |