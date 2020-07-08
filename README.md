## 基于 OpenGL 的 Bezier 曲线、曲面绘制
### 1. 功能
#### 1.1. 贝塞尔曲线
1. 基本 2D 贝塞尔曲线绘制（已完成）
2. 基本 3D 贝塞尔曲线绘制（未完成）
3. 拖拽控制点重绘贝塞尔曲线（已完成）
4. 2D 网格坐标显示（未完成）
5. 3D xy 平面网格显示，z 轴轴线显示（未完成）
6. 按钮控制（未完成）
#### 1.2. 贝塞尔曲面
1. 基本贝塞尔曲面绘制（未完成）
2. 拖拽控制点重绘贝塞尔曲面（未完成）
3. 按钮控制（未完成）
4. 3D 摄像头（未完成）
### 2. 绘制思路
&emsp;&emsp;详见 ```docs/document.md```
### 3. 程序运行
&emsp;&emsp;双击 ```Bezier\x64\Release\Bezier.exe``` 或 ```Bezier\x64\Debug\Bezier.exe```
### 4. 类结构
&emsp;&emsp;待更新
### 5. 代码文件
- 入口函数：```Bezier\Bezier\src\Application.cpp```
- Bezier 类：```Bezier\Bezier\src\Bezier.cpp```
- Shader 类：```Bezier\Bezier\src\Shader.cpp```
- Shader 文件
    - ContorlPoint.shader：```Bezier\Bezier\src\shader\ControlPoint.shader```
    - Line.shader: ```Bezier\Bezier\src\Line.shader```