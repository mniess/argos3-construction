#!/bin/bash
cd "${0%/*}/../BuildingSim"
echo "$1$(hostname)"
java -Djava.library.path=build/embedding/nsga2 -jar ../PopCodeGA/target/PopCodeGA-1.0-SNAPSHOT-jar-with-dependencies.jar "$1$(hostname)"


