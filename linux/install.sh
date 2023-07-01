#! /bin/bash
PROJ=cppknife
BASE=/usr/share/cppknife
mkdir -p $BASE
FN=upd_cppknife.sh
if [ -e $FN ]; then
  echo "already exists: $FN
  cat <<'EOS' >$FN
#! /bin/bash
ZIP=/tmp/cppknife.proj.zip

if [ ! -e "$ZIP" ]; then
  echo "+++ missing $ZIP"
else
  mkdir -p safe
  cp $ZIP safe/$(basename $ZIP).$(date +%Y.%m.%d)
  unzip -o $ZIP
  mv -v $ZIP $ZIP.deactivated
fi
EOS
chmod +x $FN
ln -s $BASE/$PROJ.so /usr/lib 
