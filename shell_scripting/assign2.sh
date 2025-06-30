#!/usr/bin/env bash
fun(){
	mkdir $1
	cd $1
	touch $2
}
fun "revathi" "revathi.txt"
