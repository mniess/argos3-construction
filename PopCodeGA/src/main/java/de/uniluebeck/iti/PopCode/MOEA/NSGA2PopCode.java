package de.uniluebeck.iti.PopCode.MOEA;

import org.moeaframework.core.Solution;
import org.moeaframework.core.variable.EncodingUtils;
import org.moeaframework.problem.AbstractProblem;

import java.util.Arrays;
import java.util.concurrent.atomic.AtomicInteger;

public class NSGA2PopCode extends AbstractProblem {

    int evaluations = 3;

    static {
        System.loadLibrary("nsga2_construction"); // Load native library at runtime
    }


    public NSGA2PopCode(int evaluations) {
        super(PopCodeUtilities.GAgenomeSize, 2);
        this.evaluations = evaluations;
        InitArgos();
    }

    public native double LaunchArgos(int[] genome, int evaluations);

    public native int InitArgos();

    public native int DestroyArgos();

    private AtomicInteger run = new AtomicInteger(0);

    public void evaluate(Solution solution) {
        int[] genome = PopCodeUtilities.getGenome(solution);
        double fitness = LaunchArgos(genome, evaluations);
        double sparsity = PopCodeUtilities.sparsity(genome);
        solution.setObjective(0, fitness);
        solution.setObjective(1, -sparsity);
        System.out.printf("%d: fitness=%.2f, sparsity=%.2f, genome=%s%n", run.incrementAndGet(), fitness, sparsity, Arrays.toString(genome));
    }

    public Solution newSolution() {
        Solution solution = new Solution(PopCodeUtilities.GAgenomeSize, 2);
        int counter = 0;
        for (int i = 0; i < PopCodeUtilities.numRobots; i++) {
            //AntiPhototaxis -> Explore
            solution.setVariable(counter, EncodingUtils.newBinaryInt(0, 4)); //Time
            solution.setVariable(counter + 1, EncodingUtils.newBinaryInt(0, 1)); //Drop/Pickup
            //Explore -> Phototaxis
            solution.setVariable(counter + 2, EncodingUtils.newBinaryInt(0, 4)); //Time
            solution.setVariable(counter + 3, EncodingUtils.newBinaryInt(-1, 1)); //Cylinder in Seen
            solution.setVariable(counter + 4, EncodingUtils.newBinaryInt(0, 1)); //Drop/Pickup
            //Phototaxis->Antiphototaxis
            solution.setVariable(counter + 5, EncodingUtils.newBinaryInt(0, 4)); //LowerBound Light
            solution.setVariable(counter + 6, EncodingUtils.newBinaryInt(0, 4)); //UpperBound Light
            solution.setVariable(counter + 7, EncodingUtils.newBinaryInt(0, 1)); //Drop/Pickup
            counter += PopCodeUtilities.robGenomeSize;
        }
        if (PopCodeUtilities.gType == GENOME.SIMPLECOUNT) {

            for (int i = 0; i < PopCodeUtilities.numRobots; i++) {
                solution.setVariable(counter + i, EncodingUtils.newBinaryInt(0, PopCodeUtilities.numRobots - 1));

            }
        }
        return solution;
    }

    private void TestJNI() {
        InitArgos();
//        LaunchArgos();
//        LaunchArgos();
        DestroyArgos();
    }

    @Override
    protected void finalize() throws Throwable {
        DestroyArgos();
        super.finalize();
    }
}
