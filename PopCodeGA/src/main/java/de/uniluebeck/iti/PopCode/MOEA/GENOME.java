package de.uniluebeck.iti.PopCode.MOEA;

public enum GENOME {
    SIMPLE("simple"), SIMPLECOUNT("simpleCount"), FULLCOUNT("fullCount"), TEST("test");

    private final String desc;

    GENOME(final String text) {
        this.desc = text;
    }

    @Override
    public String toString() {
        return desc;
    }
}
