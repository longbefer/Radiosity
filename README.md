# Radiosity
Radiosity is an application of the finite element method to solving the rendering equation for scenes with surfaces that reflect light diffusely.<sup>[1]</sup>  
This application is the implement of radiostiy and it maked by MFC(Microsofo Fundmation Class).
It compile in Windows 10 with Visual Studio 2019.

## Run
You can click the `Test.sln` file to review the code and try to compile it. If you're in a low VS version, please click the `menu - project - redirect the project soultion` try to resolve it.

## Result
![reslut](result/result(finished_in_7_Apr.).bmp)

## Refer
[1] [wikipedia(radiosity)](https://en.wikipedia.org/wiki/Radiosity_(computer_graphics))  
[2] 计算机真实感图形的算法基础—彭群生


## Known issues
[] UNKNOW  

## Formula
$$
B_{i} = E_{i} + \sum_{j=0}^{N} F_{ij}*B_{j}
$$

