#/bin/sh
cd ../group_s60_3rd
bldmake bldfiles
abld build gcce urel
cd ../sis
makesis ayfly_s60_3rd.pkg
signsis ayfly_s60_3rd.sis ayfly_s60_3rd_0.0.17.sisx gensign.cer gensign.key
