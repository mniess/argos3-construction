# ARGoS3-Construction
Simulation of collective construction for [ARGoS3](https://github.com/ilpincy/argos3)

## Requirenments

* Maven
* Java 8

## Compilation

`$ mvn package`

## Run

### Genetic Algorithm

1. `$ cd [gitroot]`
1. `$ java -Djava.library.path=BuildingSim/build/embedding/nsga2 -jar PopCodeGA/target/PopCodeGA-1.0-SNAPSHOT-jar-with-dependencies.jar`

### Single Run

See also [ARGoS3-experiments](https://github.com/ilpincy/argos3-expermients) for more details
1. `$ cd [gitroot]/BuildingSim`
1. `$ argos3 -c experiments/construction.argos`

