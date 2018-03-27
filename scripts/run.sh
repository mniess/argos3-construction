#!/bin/bash
pwd
cd "${0%/*}/../BuildingSim"
pwd
java -Djava.library.path=build/embedding/nsga2 -jar ../PopCodeGA/target/PopCodeGA-1.0-SNAPSHOT-jar-with-dependencies.jar "$(hostname)"


