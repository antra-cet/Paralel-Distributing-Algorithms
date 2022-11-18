package oneProducerOneConsumer;

import java.util.concurrent.Semaphore;

public class Buffer {
    private int a;
	private int mutex = 0;
	private Semaphore semaphoreP = new Semaphore(-1);
	private Semaphore semaphoreC = new Semaphore(0);
	
    void put(int value) {
		try {
			semaphoreP.acquire();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	
	    try {
		    wait();
	    } catch (InterruptedException e) {
		    e.printStackTrace();
	    }
	    a = value;
		notify();
		
	    semaphoreC.release();
	}

    int get() {
	    try {
		    semaphoreC.acquire();
	    } catch (InterruptedException e) {
		    e.printStackTrace();
	    }
	
	    try {
		    wait();
	    } catch (InterruptedException e) {
		    e.printStackTrace();
	    }
	    semaphoreP.release();
		notify();
		
	    return a;
    }
}
