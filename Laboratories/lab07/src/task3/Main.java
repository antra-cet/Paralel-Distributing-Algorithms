package task3;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicInteger;

public class Main {
	public static int N = 4;
	
	public static void main(String[] args) {
		int[] graph = new int[N];
		
		AtomicInteger inQueue = new AtomicInteger(0);
		ExecutorService tpe = Executors.newFixedThreadPool(4);
		
		inQueue.incrementAndGet();
		tpe.submit(new Task3(graph, 0, tpe, inQueue));
	}
}