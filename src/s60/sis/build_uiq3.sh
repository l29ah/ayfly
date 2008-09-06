#/bin/sh
cd ../group_uiq3
bldmake bldfiles
abld build gcce urel
cd ../sis
makesis ayfly_uiq3.pkg
signsis ayfly_uiq3.sis ayfly_uiq3_0.0.17.sisx gensign.cer gensign.key
