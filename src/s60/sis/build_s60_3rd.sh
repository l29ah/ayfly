#/bin/sh
export EPOCROOT=/home/andrew/symbian-sdks/s60_32/
cd ../group_s60_3rd
bldmake bldfiles
abld build gcce urel
cd ../sis
makesis ayfly_s60_3rd.pkg
signsis ayfly_s60_3rd.sis ayfly_s60_3rd_0.0.18.sisx gensign.cer gensign.key
rm ayfly_s60_3rd.sis
