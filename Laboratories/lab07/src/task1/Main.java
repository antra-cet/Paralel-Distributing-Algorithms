package task1;

import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicInteger;

public class Main {
	static int[][] graph = { { 0, 1 }, { 0, 4 }, { 0, 5 }, { 1, 0 }, { 1, 2 }, { 1, 6 }, { 2, 1 }, { 2, 3 }, { 2, 7 },
		{ 3, 2 }, { 3, 4 }, { 3, 8 }, { 4, 0 }, { 4, 3 }, { 4, 9 }, { 5, 0 }, { 5, 7 }, { 5, 8 }, { 6, 1 },
		{ 6, 8 }, { 6, 9 }, { 7, 2 }, { 7, 5 }, { 7, 9 }, { 8, 3 }, { 8, 5 }, { 8, 6 }, { 9, 4 }, { 9, 6 },
		{ 9, 7 } };
	
	public static void main(String[] args) {
		ArrayList<Integer> partialPath = new ArrayList<>();
		// se vor calcula toate caile de la nodul 0 la nodul 3 in cadrul grafului
		partialPath.add(0);
		
		AtomicInteger inQueue = new AtomicInteger(0);
		ExecutorService tpe = Executors.newFixedThreadPool(4);
		
		inQueue.incrementAndGet();
		tpe.submit(new Task1(partialPath, 3, tpe, inQueue));
	}
}