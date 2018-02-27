package de.uniluebeck.iti.PopCode.MOEA;

import org.moeaframework.core.Solution;
import org.moeaframework.core.variable.EncodingUtils;

import java.util.Arrays;

public class PopCodeUtilities {

    public static GENOME gType = GENOME.SIMPLE;

    public static int numRobots = 10;
    public static int robGenomeSize = 8;
    public static int genomeSize = 80;

    public static int[] getGenome(Solution solution) {
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

    private static int[] typeHistogram(int[] genome) {
        int[] hist = new int[numRobots];
        int[][] robGenomes = new int[numRobots][robGenomeSize];
        for (int i = 0; i < numRobots * robGenomeSize; i += robGenomeSize) {
            int[] g = Arrays.copyOfRange(genome, i, i + robGenomeSize);
            for (int j = 0; j < numRobots; j++) { //sort genome into hist
                if(hist[j] == 0) {
                    hist[j] = 1;
                    robGenomes[j] = g;
                    break;
                } else if (Arrays.equals(g, robGenomes[j])) {
                    hist[j]++;
                }
            }
        }
        return hist;
    }

    public static double sparsity(int[] genome) {
        int[] hist = typeHistogram(genome);
        //l0 measure (count zeros)
        double sparsity = 0;
        for (int i : hist) {
            sparsity += i == 0 ? 1 : 0;
        }
        return sparsity;
    }
}
