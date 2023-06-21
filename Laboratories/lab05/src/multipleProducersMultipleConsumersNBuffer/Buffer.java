package multipleProducersMultipleConsumersNBuffer;

import java.util.Queue;
import java.util.concurrent.Semaphore;

public class Buffer {
    
    Queue<Integer> queue;
	private Semaphore p = new Semaphore(4);
	private Semaphore c = new Semaphore(0);
    
    public Buffer(int size) {
		queue = new LimitedQueue<>(size);
    }

	public void put(int value) {
		while (true) {
			try {
				p.acquire();
				synchronized (this) {
					queue.add(value);
				}
				c.release();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}

	public int get() {
		int a = -1;
		
		while (true) {
			try {
				c.acquire();
				synchronized (this) {
					Integer result = queue.poll();
					if (result != null) {
						a = result;
					}
				}
				
				p.release();
				return a;
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}
}
