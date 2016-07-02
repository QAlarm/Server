#! /bin/bash
tmp=/tmp/$(uuidgen)
spec=qalarm-server.spec
version=$(cat Quellen/Parameter.h |grep VERSION|cut -d \" -f 2)

mkdir -p $tmp/SPEC
mkdir $tmp/SOURCES
mkdir $tmp/SRPM

sed -e "s|VERSION|$version|g" $spec >$tmp/SPEC/$spec

rpmbuild -bs --define "_topdir $tmp" $spec
mv $tmp/SRPM/*.srpm .
rm -rf $tmp
