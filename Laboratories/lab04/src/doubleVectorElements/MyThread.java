package doubleVectorElements;

public class MyThread extends Thread {
	int id;
	int P;
	int[] array;

	public MyThread(int id, int P, int[] array) {
		this.id = id;
		this.P = P;
		this.array = array;
	}

	@Override
	public void run() {
		int start =(int)(id * 1.0 * array.length / P);
		int end = (int)(Math.min((id + 1) * 1.0 * array.length / P, array.length));

		for (int i = start; i < end; i++) {
			array[i] *= 2;
		}
	}
}

