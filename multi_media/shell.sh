#!/bin/bash

rm output_resized-\[*

./video

ffmpeg -f rawvideo -video_size 640x480 -pixel_format yuyv422 -framerate 30 -i video.yuv output-[%03d].png

./resizebulk

rm all_images.c

 ./image2c_full2

  scp -r all_images.c pi@mm.local:/home/pi

