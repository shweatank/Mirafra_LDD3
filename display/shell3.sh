for i in {1..100}; do
    # Set the input file
    num=$(printf "%03d" $i)
    infile="bmpp/output-[${num}].png"
    # Set the output header file
    outfile="bmpp/photo-${i}.rgb"


    # Generate C array with custom name
    ffmpeg -i $infile -vf scale=176:220 -pix_fmt rgb565 $outfile
done
rm output-[*
