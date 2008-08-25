#/bin/sh
cd ../group_s60_2nd
bldmake bldfiles
abld build armi urel
cd ../sis
makesis ayfly_s60_2nd.pkg

