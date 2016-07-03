#! /bin/bash
tmp=/tmp/$(uuidgen)
spec=qalarm-server.spec
version=$(cat Quellen/Parameter.h |grep VERSION|cut -d \" -f 2)
name=qalarm-server
name_version=${name}-${version}
sourcedir=$tmp/$name_version
systemd_service=qalarm.service

mkdir -p $tmp/SPEC
mkdir $tmp/SOURCES
mkdir $tmp/SRPM
mkdir $sourcedir

sed -e "s|VERSION|$version|g" $spec >$tmp/SPEC/$spec
sed -i "s|SOURCE0|${name_version}.tar.gz|g" $tmp/SPEC/$spec

cp $tmp/SPEC/$spec $sourcedir
cp -r Quellen $sourcedir
cp $systemd_service LICENSE README.md server.ini Server.pro $sourcedir


tar -czf $tmp/SOURCES/${name_version}.tar.gz -C $tmp --exclude=$spec $name_version

rpmbuild -bs --define "_topdir $tmp" $tmp/SPEC/$spec >/dev/null
mv $tmp/SRPMS/*.src.rpm .
rm -rf $tmp
