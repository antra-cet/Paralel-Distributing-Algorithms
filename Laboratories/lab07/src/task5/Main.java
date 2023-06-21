package task5;

import task4.Task4;

import java.util.concurrent.ForkJoinPool;

public class Main {
	static int N = 10;
	static int COLORS = 3;
	static int[][] graph = { { 0, 1 }, { 0, 4 }, { 0, 5 }, { 1, 0 }, { 1, 2 }, { 1, 6 }, { 2, 1 }, { 2, 3 }, { 2, 7 },
		{ 3, 2 }, { 3, 4 }, { 3, 8 }, { 4, 0 }, { 4, 3 }, { 4, 9 }, { 5, 0 }, { 5, 7 }, { 5, 8 }, { 6, 1 },
		{ 6, 8 }, { 6, 9 }, { 7, 2 }, { 7, 5 }, { 7, 9 }, { 8, 3 }, { 8, 5 }, { 8, 6 }, { 9, 4 }, { 9, 6 },
		{ 9, 7 } };
	
	
	
	
	public static void main(String[] args) {
		int[] colors = new int[N];
		//colorGraph(colors, 0);
		
		ForkJoinPool fjp = new ForkJoinPool(4);
		fjp.invoke(new Task5(colors, 0));
		fjp.shutdown();
	}
}