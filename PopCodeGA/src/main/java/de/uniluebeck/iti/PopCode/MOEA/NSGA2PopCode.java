package de.uniluebeck.iti.PopCode.MOEA;

import org.moeaframework.core.Solution;
import org.moeaframework.core.variable.EncodingUtils;
import org.moeaframework.problem.AbstractProblem;

import java.util.Arrays;
import java.util.concurrent.atomic.AtomicInteger;


public class NSGA2PopCode extends AbstractProblem {

    static {
        System.loadLibrary("nsga2_construction"); // Load native library at runtime
    }

    static int numRobots = 10;
    static int robGenomeSize = 8;

    public NSGA2PopCode() {
        super(numRobots * robGenomeSize, 2);
        InitArgos();
    }

    public native double LaunchArgos(int[] genome);

    public native int InitArgos();

    public native int DestroyArgos();

    private AtomicInteger run = new AtomicInteger(0);

    public void evaluate(Solution solution) {
        int[] genome = variablesToArray(solution);
        double fitness = LaunchArgos(genome);
        double sparsity = sparsity(typeHistogram(genome));

        solution.setObjective(0, fitness);
        solution.setObjective(1, -sparsity);
        System.out.println(run.incrementAndGet() + ": fitness=" + fitness + ", sparsity=" + sparsity + ", genome=" + Arrays.toString(genome));
    }

    public Solution newSolution() {
        Solution solution = new Solution(numRobots * robGenomeSize, 2);
        int counter = 0;
        for (int i = 0; i < numRobots; i++) {
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
            counter += robGenomeSize;
        }
        return solution;
    }

    private int[] variablesToArray(Solution solution) {
        int[] array = new int[solution.getNumberOfVariables()];
        for (int i = 0; i < solution.getNumberOfVariables(); i++) {
            try {
                array[i] = EncodingUtils.getInt(solution.getVariable(i));
            } catch (Exception e) {
                System.err.println("Encoding Error!");
                e.printStackTrace();
            }

        }
        return array;
    }

    private int[] typeHistogram(int[] genome) {
        int[] hist = new int[numRobots];
        int[][] robGenomes = new int[numRobots][robGenomeSize];
        for (int i = 0; i < numRobots * robGenomeSize; i += robGenomeSize) {
            int[] g = Arrays.copyOfRange(genome, i, i + robGenomeSize);
            for (int j = 0; j < numRobots; j++) { //sort genome into hist
                if(hist[j] == 0) {
                    hist[j] = 1;
                    robGenomes[j] = g;
                    break;
                } else if (g.equals(robGenomes[j])) {
                    hist[j]++;
                }
            }
        }
        return hist;
    }

    private double sparsity(int[] hist) {
        //l0 measure (count zeros)
        double sparsity = 0;
        for (int i : hist) {
            sparsity += i == 0 ? 1 : 0;
        }
        return sparsity;
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
