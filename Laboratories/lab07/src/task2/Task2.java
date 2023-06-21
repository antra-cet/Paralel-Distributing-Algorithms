package task2;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

public class Task2 implements Runnable {
	private final int[] colors;
	private final int step;
	private final ExecutorService tpe;
	private final AtomicInteger inQueue;
	
	public Task2(int[] colors, int step, ExecutorService tpe, AtomicInteger inQueue) {
		this.colors = colors;
		this.step = step;
		this.tpe = tpe;
		this.inQueue = inQueue;
	}
	
	@Override
	public void run() {
		if (step == Main.N) {
			printColors(colors);
		} else {
			// for the node at position step try all possible colors
			for (int i = 0; i < Main.COLORS; i++) {
				int[] newColors = colors.clone();
				newColors[step] = i;
				if (verifyColors(newColors, step)) {
					inQueue.incrementAndGet();
					tpe.submit(new Task2(newColors, step + 1, tpe, inQueue));
				}
			}
		}
		
		int left = inQueue.decrementAndGet();
		if (left == 0) {
			tpe.shutdown();
		}
	}
	
	private static boolean verifyColors(int[] colors, int step) {
		for (int i = 0; i < step; i++) {
			if (colors[i] == colors[step] && isEdge(i, step))
				return false;
		}
		return true;
	}
	
	private static boolean isEdge(int a, int b) {
		for (int[] ints : Main.graph) {
			if (ints[0] == a && ints[1] == b)
				return true;
		}
		return false;
	}
	
	static void printColors(int[] colors) {
		StringBuilder aux = new StringBuilder();
		for (int color : colors) {
			aux.append(color).append(" ");
		}
		System.out.println(aux);
	}
}