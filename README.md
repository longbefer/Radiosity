# Radiosity
Radiosity(辐射度算法) is an application of the finite element method to solving the rendering equation for scenes with surfaces that reflect light diffusely.<sup>[1]</sup>  
This application is the implement of radiosity and it makes by MFC(Microsoft Foundation Classes).
It compiled and run successfully on Windows 10 with Visual Studio 2019.  
Application can render objects in the scene by using radiosity algorithms, it also can add *.obj file and change objects or move them in the scene.  

## Run
Please insure install Visual Studio 2019 first. You can double-click the `Test.sln` file to review the code and try to compile and run it by press F5 on the keyboard. If you're in a low VS version, please click `project - redirect the project solution` in the menu bar try to resolve it.
Also, remember check whether to choose **MFC development tools** in the C++ desktop development item.

## Result
![reslut](result/result(finished_in_7_Apr.).bmp)

## Refer
[1] [wikipedia(radiosity)](https://en.wikipedia.org/wiki/Radiosity_(computer_graphics))  
[2] 计算机真实感图形的算法基础-彭群生  
[3] 光线跟踪算法技术-Kevin Suffern  


## Known issues
[1] Cube Transform  
[2] Due to ray trace ignore hit itself (judge if two patchs is visited), when two object are very close, will produce the error results.  
[3] The wrong shadow in the corner(it can fix by use multipoint sampling)  

## Formula
$$
B_{i} = E_{i} + \sum_{j=0}^{N} F_{ij}*B_{j} \\  
F_{ij} = \frac{cos\theta_{i}cos\theta_{j}}{\pi\cdot{r^{2}}} \cdot A_j
$$
