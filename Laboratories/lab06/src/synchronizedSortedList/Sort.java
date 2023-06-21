package synchronizedSortedList;

import java.util.Collections;
import java.util.List;
import java.util.concurrent.Semaphore;

public class Sort extends Thread {
    private final List<Integer> list;
	Semaphore semaphore;

    public Sort(List<Integer> list, Semaphore semaphore) {
        super();
        this.list = list;
		this.semaphore = semaphore;
    }

    @Override
    public void run() {
	    try {
		    semaphore.acquire();
	    } catch (InterruptedException e) {
		    e.printStackTrace();
	    }
	
	    Collections.sort(list);
    }
}
