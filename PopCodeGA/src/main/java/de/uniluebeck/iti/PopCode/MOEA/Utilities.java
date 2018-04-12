package de.uniluebeck.iti.PopCode.MOEA;

import org.moeaframework.core.Solution;
import org.moeaframework.core.variable.EncodingUtils;

import java.util.Arrays;

class Utilities {

    private static GENOME gType = Run.settings.gType;
    private static int numRobots = Run.settings.numRobots;

    static int robGenomeSize = gType == GENOME.FULLCOUNT ? 15 : 8;
    static int PopCodegenomeSize = numRobots * robGenomeSize;
    static int GAgenomeSize = PopCodegenomeSize + (gType == GENOME.SIMPLE ? 0 : numRobots);

    /**
     * Create the genome used in the simulation from MOEA solution
     * @param solution
     * @return the genome used in the simulation
     */
    static int[] getGenome(Solution solution) {
        int[] a = varsToArray(solution);
        switch (gType) {
            case SIMPLE:
                return a;
            case SIMPLECOUNT:
            case FULLCOUNT:
                return combineFromIndex(Arrays.copyOfRange(a, 0, a.length - numRobots), Arrays.copyOfRange(a, a.length - numRobots, a.length));
            default:
                System.out.println("RETURNING NO GENOME");
                return null;
        }
    }

    /**
     * @param values  robotgenomes
     * @param indices which genomes should be used
     * @return
     */
    private static int[] combineFromIndex(int[] values, int[] indices) {
        int[] genome = new int[PopCodegenomeSize];
        for (int i = 0; i < numRobots; i++) {
            int begin = indices[i] * robGenomeSize;
            System.arraycopy(values, begin, genome, i * robGenomeSize, robGenomeSize);
        }
        return genome;
    }

    /**
     * Convert Solution to array of variables.
     * Cannot extract directly
     * @param solution
     * @return array of variables in solution
     */
    static int[] varsToArray(Solution solution) {
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

    /**
     * Sum up equal robot genes in genome
     * @param genome
     * @return
     */
    private static int[] typeHistogram(int[] genome) {
        int[] hist = new int[numRobots];
        int[][] robGenomes = new int[numRobots][robGenomeSize];
        for (int i = 0; i < numRobots * robGenomeSize; i += robGenomeSize) {
            int[] g = Arrays.copyOfRange(genome, i, i + robGenomeSize);
            for (int j = 0; j < numRobots; j++) { //sort genome into hist
                if (hist[j] == 0) {
                    hist[j] = 1;
                    robGenomes[j] = g;
                    break;
                } else if (Arrays.equals(g, robGenomes[j])) {
                    hist[j]++;
                    break;
                }
            }
        }
        return hist;
    }

    /**
     * Simple l0 Sparsity meassure
     * @param genome
     * @return
     */
    static double sparsity(int[] genome) {
        int[] hist = typeHistogram(genome);
        //l0 measure (count zeros)
        double sparsity = 0;
        for (int i : hist) {
            sparsity += i == 0 ? 1 : 0;
        }
        return sparsity / numRobots;
    }

    /**
     * Use Hamming distance between every gene in genome for sparsity
     * @param g
     * @return
     */
    static double hammingSparsity(int[] g) {
        double robotDist = 0;
        int compares = 0;
        for (int i = 1; i < numRobots; i++) {
            int[] currGene = Arrays.copyOfRange(g, (i - 1) * robGenomeSize, i * robGenomeSize);
            for (int j = i; j < numRobots; j++) {
                int[] compareGene = Arrays.copyOfRange(g, j * robGenomeSize, (j + 1) * robGenomeSize);
                robotDist += hammingDistance(currGene, compareGene);
                compares++;
            }
        }
        int worstcase = compares * robGenomeSize;
        return 1 - (robotDist / worstcase);
    }

    private static int hammingDistance(int[] currGene, int[] compareGene) {
        int distance = 0;
        for (int i = 0; i < currGene.length; i++) {
            if (currGene[i] != compareGene[i]) distance++;
        }
        return distance;
    }
}
