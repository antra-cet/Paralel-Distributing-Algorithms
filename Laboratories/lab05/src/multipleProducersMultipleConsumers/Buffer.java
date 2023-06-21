package multipleProducersMultipleConsumers;

import java.util.concurrent.Semaphore;

public class Buffer {
    private int a;

    public void put(int value) {
        a = value;
    }

    public int get() {
        return a;
    }
}
