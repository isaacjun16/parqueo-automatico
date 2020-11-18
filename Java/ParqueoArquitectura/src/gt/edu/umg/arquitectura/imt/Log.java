package gt.edu.umg.arquitectura.imt;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.Scanner;

import com.fazecast.jSerialComm.SerialPort;

public class Log {

	public static void main(String[] args) {

		SerialPort[] ports = SerialPort.getCommPorts();

		System.out.println("Ports: ");
		for(int i = 0; i < ports.length; i++) {
			System.out.println((i + 1) + ": " + ports[i].getSystemPortName());
		}
		
		Scanner s = new Scanner(System.in);
		int choosenPort = s.nextInt();

		SerialPort  port = ports[choosenPort - 1];
		
		if(port.openPort()) {
			System.out.println("Success");
			
			port.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 0, 0);
			
			try {
				Scanner data = new Scanner(port.getInputStream());
				
				while(data.hasNextLine()) {
					String log = data.nextLine();
					System.out.println(log);
					
					if(log.contains("---")) {
						saveLog(log.replace("---", ""));
					}
				}
			} catch (Exception e) {
				System.err.println(e);
			}
			
		} else {
			System.out.println("Fail");
		}
	}
	
	private static void saveLog(String log) {
		SimpleDateFormat df = new SimpleDateFormat("dd-MM-yyyy HH:mm:ss");
		
		try(FileWriter fw = new FileWriter("log.txt", true);
		    BufferedWriter bw = new BufferedWriter(fw);
		    PrintWriter out = new PrintWriter(bw)) {
			
			out.print(log);
			out.println("," + df.format(new Date()));
			out.close();
			
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
}
