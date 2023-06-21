package shortestPathsFloyd_Warshall;

import java.util.concurrent.BrokenBarrierException;

public class MyThread extends Thread {
	int id;
	int P;
	int[][] array;

	public MyThread(int id, int P, int[][] array) {
		this.id = id;
		this.P = P;
		this.array = array;
	}

	public void run() {
		int start =(int)(id * 1.0 * array.length / P);
		int end = (int)(Math.min((id + 1) * 1.0 * array.length / P, array.length));

		for (int k = 0; k < 5; k++) {
			for (int i = start; i < end; i++) {
					for (int j = 0; j < 5; j++) {
						array[i][j] = Math.min(array[i][k] + array[k][j], array[i][j]);
					}
			}
			
			try {
				Main.barrier.await();
			} catch (InterruptedException | BrokenBarrierException e) {
				e.printStackTrace();
			}
		}
	}
}
