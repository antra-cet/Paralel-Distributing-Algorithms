package task4;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.RecursiveTask;

public class Task4 extends RecursiveTask<Void> {
	private final ArrayList<Integer> partialPath;
	private final int destination;
	
	public Task4(ArrayList<Integer> partialPath, int destination) {
		this.partialPath = partialPath;
		this.destination = destination;
	}
	
	@Override
	protected Void compute() {
		if (partialPath.get(partialPath.size() - 1) == destination) {
			System.out.println(partialPath);
			return null;
		}
		
		// se verifica nodurile pentru a evita ciclarea in graf
		List<Task4> tasks = new ArrayList<>();
		int lastNodeInPath = partialPath.get(partialPath.size() - 1);
		for (int[] ints : Main.graph) {
			if (ints[0] == lastNodeInPath) {
				if (partialPath.contains(ints[1]))
					continue;
				
				ArrayList<Integer> newPartialPath = new ArrayList<>(partialPath);
				newPartialPath.add(ints[1]);
				
				Task4 t = new Task4(newPartialPath, destination);
				tasks.add(t);
				t.fork();
			}
		}
		for (var task: tasks) {
			task.join();
		}
		
		return null;
	}
}