package synchronizedSortedList;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.List;
import java.util.Scanner;
import java.util.concurrent.Semaphore;

public class Reader extends Thread {
    private final String filename;
    private final List<Integer> list;
	Semaphore semaphore;
	
    public Reader(String filename, List<Integer> list, Semaphore semaphore) {
        super();
        this.filename = filename;
        this.list = list;
		this.semaphore = semaphore;
    }

    @Override
    public void run() {
        try {
            Scanner scanner = new Scanner(new File(filename));
            while (scanner.hasNextInt()) {
                list.add(scanner.nextInt());
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
		
		semaphore.release();
    }
}
