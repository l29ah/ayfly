#/bin/sh
export EPOCROOT=/home/ander/symbian-sdks/s60_32/
cd ../group_s60_3rd
bldmake bldfiles
abld build gcce urel
cd ../sis
makesis ayfly_s60_3rd.pkg
signsis ayfly_s60_3rd.sis ayfly-s60-3rd-0.0.24.sisx gensign.cer gensign.key
rm ayfly_s60_3rd.sis
