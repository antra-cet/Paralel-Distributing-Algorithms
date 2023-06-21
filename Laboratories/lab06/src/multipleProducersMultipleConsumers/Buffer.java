package multipleProducersMultipleConsumers;

import java.util.concurrent.ArrayBlockingQueue;

public class Buffer {
	int value;
	ArrayBlockingQueue<Integer> arr = new ArrayBlockingQueue<Integer>(10);

	void put(int value) {
		try {
			arr.put(value);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

	int get() {
		int value = -1;
		
		try {
			value = arr.take();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		
		return value;
	}
}
