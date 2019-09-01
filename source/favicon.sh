


for i in 16 32 64 128
do
    echo $i
    inkscape -z -f favicon.svg -w$i -h$i -e favicon-$i.png -d 300
done

convert favicon-*.png -colors 256 favicon.ico

xxd -i favicon.ico faviconIco.h

echo
echo 'replace content of Clock/faviconIco.h and Clock/faviconSvg.h'
