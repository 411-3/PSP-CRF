# PSP-CRF

This code is the implementation of my paper [Improving Semantic Image Segmentation with a Probabilistic Superpixel-based Dense Conditional Random Field](https://ieeexplore.ieee.org/document/8314143/), which named PSP-CRF. You can read the abstract of the paper to understand what I did quickly.

## How to run this code

(1) Dealing with image processing, so we need install **openCV** firstly. The project is controled by Cmake.

(2) download the code, and unzip. cd XXX/psp-crf/, main.cpp is the kernel code, and it need some util functions, so we need compile some Static library file.

(3) cd XXX/psp-crf/src/cnpy-master, obtaining the **libcnpy.a** by the CMakeList.txt, the libcnpy.a in the /build. This function is used for reading Probability file of CNN into memory, for the read speed mainly.

(4) cd XXX/psp-crf/src/densecrf_lib, obtaining the **libdensecrf.a** by the CMakeList.txt, libdensecrf.a in the /build/src/. This function is used for CRF API.

(5) cd XXX/psp-crf/src/lbplib, obtaining the **libopencv_lbp.a** by the CMakeList.txt, libopencv_lbp.a in the /build/. This function is used for LBP API. 

(6) cd XXX/psp-crf/src/SLICO, obtaining the **libSLIC.a** by the CMakeList.txt, libSLIC.a in the /build/src/. This function is used for SLIC API. 

After (6), we will obtain 4 .a file, and copy them to XXX/psp-crf/lib/. You will find the download code has lib/, but it is compiled on my machine platform, so you need to do (3)-(6) on your machine again.

(7) cd XXX/psp-crf/, open the CMakeLists.txt, and modify the path in this file according to your environment. Then compile exe file.(You need konw how to compile exe according to CMakeLists.txt)

if you have get the exe file, for a single image, the args are: ./psp-crf  a.jpg  a.npy  out.png 0 3 30 2 3 2 1 12 

(8) cd XXX/psp-crf/, exists the eval.py file, it is a python file for the Automatic execution for many images. In this python file, you need to modify the file path.


## Citation

Please cite PSP-CRF in your publications if it helps your research:

>     @article{PSP-CRF,
>          Author = {Zhang, Liang and Li, Huan and Shen, Peiyi and Zhu, Guangming and Song, Song and Shah, Syed Afaq Ali and Bennamoun, Mohammed and Zhang, Li},
>          Journal = {IEEE Access},
>          Title = {Improving Semantic Image Segmentation With a Probabilistic Superpixel-Based Dense Conditional Random Field},
>          Year = {2018}
>          volume={6}, 
>          Number={1}, 
>          Pages={15297-15310}, 
>       }

Email: 2250611484@qq.com, Li Huan
