rm frames/output-[*
rm frames/photos-*
ffmpeg -f rawvideo -video_size 1280x720 -pixel_format yuyv422 -framerate 10 -i video.yuv frames/output-[%03d].png
for i in {1..100}; do
    # Set the input file
    num=$(printf "%03d" $i)
    infile="frames/output-[${num}].png"
    # Set the output header file
    outfile="frames/photo-${i}.rgb"


    # Generate C array with custom name
    ffmpeg -i $infile -vf scale=176:220 -pix_fmt rgb565 $outfile
done

