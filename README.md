# ARGoS3-Construction
Simulation of collective construction for [ARGoS3](https://github.com/ilpincy/argos3)

This project consists of a simulation using ARGoS (see folder ''BuildingSim'') and a genetic algorithm using the MOEA-framework (see folder ''PopCodeGA''').

## Configuration

### Simulation

The simulation is configured via a '.argos'-file.
The provided file can be found at ''BuildingSim/experiments/construction.argos''
This file enables the GUI.
Another config file ''constuction-nsga2.argos'' disables the GUI and is used for the genetic algorithm.

You can configure the number of cylinders, arena size and target area with this file.
For more details please see the [official documentation](http://argos-sim.info/user_manual.php#_the_experiment_configuration_file)

### Genetic algorithm

Details about the genetic algorithm can be configured with a ''GA.properties'' file.
A default file is located at ''BuildingSim/GA.properties''.
You have to start the simulation from the same folder as the config file!

If no settings file is provided, default values are used for each parameter:
The following parameters can be configured:
* numRobots=10
* PopulationSize=100
* generations=1000
* evaluations=3: Number of simulations executed during evaluation
* tournamentSize=2
* crossoverProb=1
* mutationProb=0.01
* gType=FULLCOUNT
* resultPath=~/ArgosResults/ : Final log location

## Requirenments

* Maven
* Java 8

## Compilation

`$ mvn package`

## Run

### Genetic Algorithm

Using the script:
1. `chmod +x [gitroot]/scripts/run.sh`
1. `./[gitroot]/scripts/run.sh`

or

Direct:
1. `$ cd [gitroot]/BuildingSim`
1. `$ java -Djava.library.path=build/embedding/nsga2 -jar ../PopCodeGA/target/PopCodeGA-1.0-SNAPSHOT-jar-with-dependencies.jar`

### Single Run

1. `cd [gitroot]/BuildingSim`
1. `./build/loop_functions/construction_loop_functions/argosControl`

For use with a specific genome, change it in the ''/BuildingSim/loop_functions/construction_loop_functions/argosControl.cpp''

or

Direct with unconfigured genome. See also [ARGoS3-experiments](https://github.com/ilpincy/argos3-expermients) for more details:
1. `$ cd [gitroot]/BuildingSim`
1. `$ argos3 -c experiments/construction.argos`

